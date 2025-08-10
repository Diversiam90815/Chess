/*
  ==============================================================================
	Module:         Move Evaluation Performance Tests
	Description:    Performance testing for move evaluation algorithms
  ==============================================================================
*/
#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <vector>
#include <random>
#include <fstream>
#include <iomanip>

#include "Moves/Evaluation/MoveEvaluation.h"
#include "Generation/MoveGeneration.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"
#include "ChessBoard.h"

namespace fs = std::filesystem;

namespace PerformanceTests
{

struct MoveEvaluationPerformanceResult
{
	std::string				  testName{};
	std::string				  evaluationType{};
	std::chrono::microseconds duration{};
	size_t					  movesEvaluated{};
	double					  evaluationsPerSecond{};
	double					  averageEvaluationTime{};
	int						  minScore{};
	int						  maxScore{};
	double					  averageScore{};
};


class MoveEvaluationPerformanceTests : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;
	std::shared_ptr<MoveGeneration> mGeneration;
	std::shared_ptr<MoveEvaluation> mEvaluation;


	void							SetUp() override
	{
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();
		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
		mEvaluation = std::make_shared<MoveEvaluation>(mBoard, mGeneration);
	}


	std::vector<PossibleMove> generateTestMoves(int count = 1000)
	{
		std::vector<PossibleMove> moves;
		std::random_device		  rd;
		std::mt19937			  gen(rd());

		// Generate moves from various positions

		mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
		auto whitePieces = mBoard->getPiecesFromPlayer(PlayerColor::White);

		for (const auto &[pos, piece] : whitePieces)
		{
			auto pieceMoves = mGeneration->getMovesForPosition(pos);
			moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
		}

		mGeneration->calculateAllLegalBasicMoves(PlayerColor::Black);
		auto blackPieces = mBoard->getPiecesFromPlayer(PlayerColor::Black);

		for (const auto &[pos, piece] : blackPieces)
		{
			auto pieceMoves = mGeneration->getMovesForPosition(pos);
			moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
		}

		// Shuffle and limit to requested count
		std::shuffle(moves.begin(), moves.end(), gen);

		if (moves.size() > count)
			moves.resize(count);

		return moves;
	}


	template <typename EvaluationFunc>
	MoveEvaluationPerformanceResult benchmarkEvaluation(
		const std::string &testName, const std::string &evaluationName, EvaluationFunc evaluationFunc, const std::vector<PossibleMove> &moves, PlayerColor player)
	{
		MoveEvaluationPerformanceResult result;
		result.testName		  = testName;
		result.evaluationType = evaluationName;
		result.movesEvaluated = moves.size();

		std::vector<int> scores;
		scores.reserve(moves.size());

		auto start = std::chrono::high_resolution_clock::now();

		for (const auto &move : moves)
		{
			int score = evaluationFunc(move, player);
			scores.push_back(score);
		}

		auto end					 = std::chrono::high_resolution_clock::now();
		result.duration				 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		// Calculate statistics
		double seconds				 = result.duration.count() / 1000000.0;
		result.evaluationsPerSecond	 = result.movesEvaluated / seconds;
		result.averageEvaluationTime = result.duration.count() / static_cast<double>(result.movesEvaluated);

		if (!scores.empty())
		{
			result.minScore		= *std::min_element(scores.begin(), scores.end());
			result.maxScore		= *std::max_element(scores.begin(), scores.end());
			result.averageScore = std::accumulate(scores.begin(), scores.end(), 0.0) / scores.size();
		}

		return result;
	}


	void saveResults(std::string fileName, const std::vector<MoveEvaluationPerformanceResult> &results)
	{
		// Create directory if not exists yet
		fs::path resultDir = "MoveEvaluation_Results";

		if (!fs::exists(resultDir))
			fs::create_directories(resultDir);

		fs::path	  fullPath = resultDir / fileName;

		std::ofstream file(fullPath, std::ios::app);

		if (!file.is_open())
			return;

		file << "=== Move Evaluation Performance Test Results ===" << std::endl;
		file << std::setw(15) << "Test Name" << std::setw(12) << "Type" << std::setw(12) << "Duration(μs)" << std::setw(12) << "Count" << std::setw(12) << "Eval/Sec"
			 << std::setw(12) << "Avg Time" << std::setw(10) << "Min Score" << std::setw(10) << "Max Score" << std::setw(12) << "Avg Score" << std::endl;
		file << std::string(120, '-') << std::endl;

		for (const auto &result : results)
		{
			file << std::setw(15) << result.testName << std::setw(12) << result.evaluationType << std::setw(12) << result.duration.count() << std::setw(12) << result.movesEvaluated
				 << std::setw(12) << static_cast<int>(result.evaluationsPerSecond) << std::setw(12) << std::fixed << std::setprecision(2) << result.averageEvaluationTime
				 << std::setw(10) << result.minScore << std::setw(10) << result.maxScore << std::setw(12) << std::fixed << std::setprecision(1) << result.averageScore << std::endl;
		}
		file << std::endl;
		file.close();
	}
};


TEST_F(MoveEvaluationPerformanceTests, BasicEvaluationPerformance)
{
	auto moves = generateTestMoves(1000);

	auto result =
		benchmarkEvaluation("Basic", "Basic", [this](const PossibleMove &move, PlayerColor player) { return mEvaluation->getBasicEvaluation(move); }, moves, PlayerColor::White);

	EXPECT_GT(result.evaluationsPerSecond, 10000) << "Basic evaluation should be very fast";

	std::vector<MoveEvaluationPerformanceResult> results = {result};
	saveResults("BasicEvaluation", results);
}


TEST_F(MoveEvaluationPerformanceTests, MediumEvaluationPerformance)
{
	auto moves	= generateTestMoves(1000);

	auto result = benchmarkEvaluation(
		"Medium", "Medium", [this](const PossibleMove &move, PlayerColor player) { return mEvaluation->getMediumEvaluation(move, player); }, moves, PlayerColor::White);

	EXPECT_GT(result.evaluationsPerSecond, 5000) << "Medium evaluation should be reasonably fast";

	std::vector<MoveEvaluationPerformanceResult> results = {result};
	saveResults("MediumEvaluation", results);
}


TEST_F(MoveEvaluationPerformanceTests, AdvancedEvaluationPerformance)
{
	auto moves	= generateTestMoves(500); // Fewer moves for advanced evaluation

	auto result = benchmarkEvaluation(
		"Advanced", "Advanced", [this](const PossibleMove &move, PlayerColor player) { return mEvaluation->getAdvancedEvaluation(move, player); }, moves, PlayerColor::White);

	EXPECT_GT(result.evaluationsPerSecond, 1000) << "Advanced evaluation should complete in reasonable time";

	std::vector<MoveEvaluationPerformanceResult> results = {result};
	saveResults("Advanced Evaluation", results);
}


TEST_F(MoveEvaluationPerformanceTests, EvaluationTypeComparison)
{
	auto										 moves = generateTestMoves(500);
	std::vector<MoveEvaluationPerformanceResult> results;

	// Test all three evaluation types
	results.push_back(benchmarkEvaluation(
		"Comparison", "Basic", [this](const PossibleMove &move, PlayerColor player) { return mEvaluation->getBasicEvaluation(move); }, moves, PlayerColor::White));

	results.push_back(benchmarkEvaluation(
		"Comparison", "Medium", [this](const PossibleMove &move, PlayerColor player) { return mEvaluation->getMediumEvaluation(move, player); }, moves, PlayerColor::White));

	results.push_back(benchmarkEvaluation(
		"Comparison", "Advanced", [this](const PossibleMove &move, PlayerColor player) { return mEvaluation->getAdvancedEvaluation(move, player); }, moves, PlayerColor::White));

	// Performance should be: Basic > Medium > Advanced
	EXPECT_GT(results[0].evaluationsPerSecond, results[1].evaluationsPerSecond) << "Basic evaluation should be faster than medium";
	EXPECT_GT(results[1].evaluationsPerSecond, results[2].evaluationsPerSecond) << "Medium evaluation should be faster than advanced";
}


TEST_F(MoveEvaluationPerformanceTests, TacticalEvaluationPerformance)
{
	auto moves	= generateTestMoves(200);

	auto result = benchmarkEvaluation(
		"Tactical", "Tactical", [this](const PossibleMove &move, PlayerColor player) { return mEvaluation->getTacticalEvaluation(move, player); }, moves, PlayerColor::White);

	EXPECT_GT(result.evaluationsPerSecond, 500) << "Tactical evaluation should complete in reasonable time";

	std::vector<MoveEvaluationPerformanceResult> results = {result};
	saveResults("Tactical Evaluation", results);
}


TEST_F(MoveEvaluationPerformanceTests, StrategicEvaluationPerformance)
{
	auto moves	= generateTestMoves(200);

	auto result = benchmarkEvaluation(
		"Strategic", "Strategic", [this](const PossibleMove &move, PlayerColor player) { return mEvaluation->getStrategicEvaluation(move, player); }, moves, PlayerColor::White);

	EXPECT_GT(result.evaluationsPerSecond, 500) << "Strategic evaluation should complete in reasonable time";

	std::vector<MoveEvaluationPerformanceResult> results = {result};
	saveResults("Strategic Evaluation", results);
}


} // namespace PerformanceTests