/*
  ==============================================================================
	Module:         Move Generation Performance Tests
	Description:    Performance testing for move generation algorithms
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <vector>
#include <random>
#include <fstream>
#include <iomanip>

#include "Generation/MoveGeneration.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"
#include "ChessBoard.h"

namespace fs = std::filesystem;

namespace PerformanceTests
{
struct MoveGenerationPerformanceResult
{
	std::string				  testName{};
	std::chrono::microseconds duration{};
	size_t					  movesGenerated{};
	double					  movesPerSecond{};
	size_t					  positionsEvaluated{};
	std::string				  boardConfiguration{};
};


class MoveGenerationPerformanceTests : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;
	std::shared_ptr<MoveGeneration> mGeneration;


	void							SetUp() override
	{
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();

		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
	}


	void setupOpeningPosition()
	{
		mBoard->removeAllPiecesFromBoard();
		mBoard->initializeBoard();
	}


	void setupMiddlegamePosition()
	{
		mBoard->removeAllPiecesFromBoard();

		Position whiteKingPos = Position{4, 7};
		Position blackKingPos = Position{4, 0};

		// Setup a complex middlegame position
		mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
		mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
		mBoard->setPiece({3, 7}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));
		mBoard->setPiece({3, 0}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::Black));
		mBoard->setPiece({0, 7}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
		mBoard->setPiece({7, 7}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
		mBoard->setPiece({0, 0}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
		mBoard->setPiece({7, 0}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
		mBoard->setPiece({2, 7}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White));
		mBoard->setPiece({5, 7}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White));
		mBoard->setPiece({2, 0}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::Black));
		mBoard->setPiece({5, 0}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::Black));

		// Add scattered pawns and pieces for complexity
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

		Position whiteKingPos = Position{4, 7};
		Position blackKingPos = Position{4, 0};

		// Simple endgame with kings, rooks and pawns
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


	MoveGenerationPerformanceResult benchmarkMoveGeneration(const std::string &testName, const std::string &boardConfig, int iterations = 1000)
	{
		MoveGenerationPerformanceResult result;
		result.testName			  = testName;
		result.boardConfiguration = boardConfig;
		result.movesGenerated	  = 0;
		result.positionsEvaluated = 0;

		auto start				  = std::chrono::high_resolution_clock::now();

		for (int i = 0; i < iterations; ++i)
		{
			// Generate moves for white
			mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);

			// Count moves for all white pieces
			auto whitePieces = mBoard->getPiecesFromPlayer(PlayerColor::White);
			for (const auto &[pos, piece] : whitePieces)
			{
				auto moves = mGeneration->getMovesForPosition(pos);
				result.movesGenerated += moves.size();
				result.positionsEvaluated++;
			}

			// Generate moves for black
			mGeneration->calculateAllLegalBasicMoves(PlayerColor::Black);

			// Count moves for all black pieces
			auto blackPieces = mBoard->getPiecesFromPlayer(PlayerColor::Black);
			for (const auto &[pos, piece] : blackPieces)
			{
				auto moves = mGeneration->getMovesForPosition(pos);
				result.movesGenerated += moves.size();
				result.positionsEvaluated++;
			}
		}

		auto end			  = std::chrono::high_resolution_clock::now();
		result.duration		  = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		double seconds		  = result.duration.count() / 1000000.0;
		result.movesPerSecond = result.movesGenerated / seconds;

		return result;
	}


	void saveResults(const std::string fileName, const std::vector<MoveGenerationPerformanceResult> &results)
	{
		// Create directory if not exists yet
		fs::path resultDir = "MoveGeneration_Results";

		if (!fs::exists(resultDir))
			fs::create_directories(resultDir);

		fs::path	  fullPath = resultDir / fileName;

		std::ofstream file(fullPath, std::ios::app);

		if (!file.is_open())
			return;

		file << "=== Move Generation Performance Test Results ===" << std::endl;
		file << std::setw(20) << "Test Name" << std::setw(15) << "Duration (μs)" << std::setw(15) << "Moves Gen" << std::setw(15) << "Moves/Sec" << std::setw(15) << "Positions"
			 << std::setw(20) << "Board Config" << std::endl;
		file << std::string(100, '-') << std::endl;

		for (const auto &result : results)
		{
			file << std::setw(20) << result.testName << std::setw(15) << result.duration.count() << std::setw(15) << result.movesGenerated << std::setw(15)
				 << static_cast<int>(result.movesPerSecond) << std::setw(15) << result.positionsEvaluated << std::setw(20) << result.boardConfiguration << std::endl;
		}

		file << std::endl;
		file.close();
	}
};


TEST_F(MoveGenerationPerformanceTests, OpeningPositionPerformance)
{

	setupOpeningPosition();
	auto result = benchmarkMoveGeneration("Opening", "Standart_Start", 1000);

	EXPECT_GT(result.movesPerSecond, 10000) << "Move generation should be fast enough";

	std::vector<MoveGenerationPerformanceResult> results = {result};
	saveResults("Opening Game Position", results);
}


TEST_F(MoveGenerationPerformanceTests, MiddlegamePositionPerformance)
{
	setupMiddlegamePosition();
	auto result = benchmarkMoveGeneration("Middlegame", "Complex_Middle", 1000);

	EXPECT_GT(result.movesPerSecond, 5000) << "Complex position generation should still be reasonable";

	std::vector<MoveGenerationPerformanceResult> results = {result};
	saveResults("Middle Game Position" , results);
}


TEST_F(MoveGenerationPerformanceTests, EndgamePositionPerformance)
{
	setupEndgamePosition();
	auto result = benchmarkMoveGeneration("Endgame", "Simple_End", 1000);

	EXPECT_GT(result.movesPerSecond, 15000) << "Endgame generation should be very fast";

	std::vector<MoveGenerationPerformanceResult> results = {result};
	saveResults("End Game Position", results);
}


TEST_F(MoveGenerationPerformanceTests, ComprehensivePositionComparison)
{
	std::vector<MoveGenerationPerformanceResult> results;

	// Test all three position types
	setupOpeningPosition();
	results.push_back(benchmarkMoveGeneration("Opening", "Standard", 500));

	setupMiddlegamePosition();
	results.push_back(benchmarkMoveGeneration("Middlegame", "Complex", 500));

	setupEndgamePosition();
	results.push_back(benchmarkMoveGeneration("Endgame", "Simple", 500));

	// Performance should generally be: Endgame > Opening > Middlegame
	EXPECT_GT(results[2].movesPerSecond, results[0].movesPerSecond) << "Endgame should be faster than opening";
	EXPECT_GT(results[0].movesPerSecond, results[1].movesPerSecond) << "Opening should be faster than complex middlegame";
}


} // namespace PerformanceTests