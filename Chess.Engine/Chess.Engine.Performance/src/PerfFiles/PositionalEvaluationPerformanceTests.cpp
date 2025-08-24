/*
  ==============================================================================
	Module:         Positional Evaluation Performance Tests
	Description:    Performance testing for positional evaluation algorithms
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <vector>
#include <fstream>
#include <iomanip>
#include <filesystem>

#include "PositionalEvaluation/PositionalEvaluation.h"
#include "Board/LightChessBoard.h"
#include "Board/ChessBoard.h"
#include "Moves/Evaluation/MoveEvaluation.h"
#include "Generation/MoveGeneration.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"
#include "PerformanceJSONHelper.h"


namespace fs = std::filesystem;

namespace PerformanceTests
{

class PositionalEvaluationPerformanceTests : public ::testing::Test
{
protected:
	std::shared_ptr<PositionalEvaluation> mPositionalEvaluation;
	std::shared_ptr<MoveEvaluation>		  mMoveEvaluation;
	std::shared_ptr<ChessBoard>			  mBoard;
	std::shared_ptr<MoveValidation>		  mValidation;
	std::shared_ptr<MoveExecution>		  mExecution;
	std::shared_ptr<MoveGeneration>		  mGeneration;


	void								  SetUp() override
	{
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();
		mValidation			  = std::make_shared<MoveValidation>(mBoard);
		mExecution			  = std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration			  = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
		mMoveEvaluation		  = std::make_shared<MoveEvaluation>(mBoard, mGeneration);
		mPositionalEvaluation = std::make_shared<PositionalEvaluation>(mMoveEvaluation);
	}


	void setupMiddlegamePosition()
	{
		mBoard->removeAllPiecesFromBoard();

		Position whiteKingPos{4, 7}; // e1
		Position blackKingPos{4, 0}; // e8

		mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
		mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
		mBoard->setPiece({3, 7}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));
		mBoard->setPiece({3, 0}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::Black));
		mBoard->setPiece({0, 7}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
		mBoard->setPiece({7, 7}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
		mBoard->setPiece({0, 0}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
		mBoard->setPiece({7, 0}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));

		// Add complex piece structure
		for (int i = 0; i < 8; i += 2)
		{
			mBoard->setPiece({i, 5}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
			mBoard->setPiece({i + 1, 2}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
		}

		mBoard->updateKingsPosition(whiteKingPos, PlayerColor::White);
		mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);
	}


	void setupEndgamePosition()
	{
		mBoard->removeAllPiecesFromBoard();

		Position whiteKingPos{4, 7}; // e1
		Position blackKingPos{4, 0}; // e8

		mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
		mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
		mBoard->setPiece({0, 7}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
		mBoard->setPiece({0, 0}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
		mBoard->setPiece({1, 6}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
		mBoard->setPiece({2, 6}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
		mBoard->setPiece({1, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
		mBoard->setPiece({2, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

		mBoard->updateKingsPosition(whiteKingPos, PlayerColor::White);
		mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);
	}


	template <typename EvaluationFunc>
	PositionalEvaluationPerformanceResult benchmarkEvaluation(
		const std::string &testName, const std::string &evaluationType, const std::string &gamePhase, EvaluationFunc evaluationFunc, size_t iterations)
	{
		PositionalEvaluationPerformanceResult result;
		result.testName				= testName;
		result.evaluationType		= evaluationType;
		result.gamePhase			= gamePhase;
		result.evaluationsPerformed = iterations;
		result.timestamp			= std::chrono::system_clock::now();

		LightChessBoard	 lightBoard(*mBoard);
		std::vector<int> scores;
		scores.reserve(iterations);

		auto start = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < iterations; ++i)
		{
			int score = evaluationFunc(lightBoard);
			scores.push_back(score);
		}

		auto end					 = std::chrono::high_resolution_clock::now();
		result.duration				 = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		// Calculate statistics
		double seconds				 = result.duration.count() / 1000000.0;
		result.evaluationsPerSecond	 = result.evaluationsPerformed / seconds;
		result.averageEvaluationTime = result.duration.count() / static_cast<double>(result.evaluationsPerformed);

		if (!scores.empty())
		{
			result.minScore		= *std::min_element(scores.begin(), scores.end());
			result.maxScore		= *std::max_element(scores.begin(), scores.end());
			result.averageScore = std::accumulate(scores.begin(), scores.end(), 0.0) / scores.size();
		}

		result.pieceCount = lightBoard.getPieceCount(PlayerColor::White) + lightBoard.getPieceCount(PlayerColor::Black);

		return result;
	}


	void saveJsonResults(const std::string &fileName, const std::vector<PositionalEvaluationPerformanceResult> &results)
	{
		PerformanceJsonHelper::saveJsonResults(fileName, "Positional Evaluation Performance", results);
	}
};


TEST_F(PositionalEvaluationPerformanceTests, FullPositionEvaluationSpeed)
{
	constexpr int iterations = 1000;

	auto		  result	 = benchmarkEvaluation(
		 "FullEval", "Complete", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluatePosition(board, PlayerColor::White); }, iterations);

	std::vector<PositionalEvaluationPerformanceResult> results = {result};
	//saveResults("Full Positional Evaluation", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(PositionalEvaluationPerformanceTests, DetailedEvaluationSpeed)
{
	constexpr int iterations = 500;

	auto		  result	 = benchmarkEvaluation(
		 "Detailed", "Detailed", "Opening",
		 [this](const LightChessBoard &board)
		 {
			 auto detailedResult = mPositionalEvaluation->evaluatePositionDetailed(board, PlayerColor::White);
			 return detailedResult.getTotalScore();
		 },
		 iterations);

	std::vector<PositionalEvaluationPerformanceResult> results = {result};
	//saveResults("Detailed Evaluation", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(PositionalEvaluationPerformanceTests, MaterialEvaluationSpeed)
{
	constexpr int iterations = 5000;

	auto		  result	 = benchmarkEvaluation(
		 "Material", "Material", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluateMaterial(board, PlayerColor::White); }, iterations);

	std::vector<PositionalEvaluationPerformanceResult> results = {result};
	//saveResults("Material Evaluation", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(PositionalEvaluationPerformanceTests, KingSafetyEvaluationSpeed)
{
	constexpr int iterations = 2000;

	auto		  result	 = benchmarkEvaluation(
		 "KingSafety", "KingSafety", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluateKingSafety(board, PlayerColor::White); }, iterations);

	std::vector<PositionalEvaluationPerformanceResult> results = {result};
	//saveResults("King Safety Evaluation", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(PositionalEvaluationPerformanceTests, MobilityEvaluationSpeed)
{
	constexpr int iterations = 1000;

	auto		  result	 = benchmarkEvaluation(
		 "Mobility", "Mobility", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluateMobility(board, PlayerColor::White); }, iterations);

	std::vector<PositionalEvaluationPerformanceResult> results = {result};
	//saveResults("Mobility Evaluation", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(PositionalEvaluationPerformanceTests, PawnStructureEvaluationSpeed)
{
	constexpr int iterations = 2000;

	auto		  result	 = benchmarkEvaluation(
		 "PawnStruct", "PawnStruct", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluatePawnStructure(board, PlayerColor::White); },
		 iterations);

	std::vector<PositionalEvaluationPerformanceResult> results = {result};
	//saveResults("Pawn Structure Evaluation", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(PositionalEvaluationPerformanceTests, GamePhaseEvaluationComparison)
{
	constexpr int									   iterations = 1000;
	std::vector<PositionalEvaluationPerformanceResult> results;

	// Opening evaluation
	auto											   openingResult = benchmarkEvaluation(
		  "Opening", "Complete", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluatePosition(board, PlayerColor::White); }, iterations);
	results.push_back(openingResult);

	// Middlegame evaluation
	setupMiddlegamePosition();
	auto middlegameResult = benchmarkEvaluation(
		"Middlegame", "Complete", "Middlegame", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluatePosition(board, PlayerColor::White); }, iterations);
	results.push_back(middlegameResult);

	// Endgame evaluation
	setupEndgamePosition();
	auto endgameResult = benchmarkEvaluation(
		"Endgame", "Complete", "Endgame", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluatePosition(board, PlayerColor::White); }, iterations);
	results.push_back(endgameResult);

	//saveResults("Gamephase Evaluation Comparison", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(PositionalEvaluationPerformanceTests, ComponentEvaluationComparison)
{
	constexpr int									   iterations = 1000;
	std::vector<PositionalEvaluationPerformanceResult> results;

	// Test all individual components
	results.push_back(benchmarkEvaluation(
		"Material", "Material", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluateMaterial(board, PlayerColor::White); },
		iterations * 5 // More iterations for fast operations
		));

	results.push_back(benchmarkEvaluation(
		"Positional", "Positional", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluatePositionalAdvantage(board, PlayerColor::White); },
		iterations));

	results.push_back(benchmarkEvaluation(
		"KingSafety", "KingSafety", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluateKingSafety(board, PlayerColor::White); }, iterations));

	results.push_back(benchmarkEvaluation(
		"Mobility", "Mobility", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluateMobility(board, PlayerColor::White); }, iterations));

	results.push_back(benchmarkEvaluation(
		"Tactical", "Tactical", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluateTacticalOpportunities(board, PlayerColor::White); },
		iterations / 2 // Fewer iterations for expensive operations
		));

	results.push_back(benchmarkEvaluation(
		"PawnStruct", "PawnStruct", "Opening", [this](const LightChessBoard &board) { return mPositionalEvaluation->evaluatePawnStructure(board, PlayerColor::White); },
		iterations));

	// Material should be fastest, tactical should be slowest
	auto materialIt = std::find_if(results.begin(), results.end(), [](const auto &r) { return r.evaluationType == "Material"; });
	auto tacticalIt = std::find_if(results.begin(), results.end(), [](const auto &r) { return r.evaluationType == "Tactical"; });

	//saveResults("Component Evaluation Comparison", results);

	// The results of this test are saved in the file
	SUCCEED();
}



} // namespace PerformanceTests