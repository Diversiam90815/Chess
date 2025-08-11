/*
  ==============================================================================
	Module:         CPU Player Performance Tests
	Description:    Performance testing for CPU player algorithms (minimax, alpha-beta)
  ==============================================================================
*/


#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <vector>
#include <fstream>
#include <iomanip>

#include "Player/CPUPlayer.h"
#include "Generation/MoveGeneration.h"
#include "Moves/Evaluation/MoveEvaluation.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"
#include "ChessBoard.h"

namespace fs = std::filesystem;

namespace PerformanceTests
{
struct CPUAlgorithmPerformanceResult
{
	std::string				  algorithmName{};
	int						  depth;
	std::chrono::milliseconds duration;
	PossibleMove			  selectedMove;
	std::string				  position;
};


class CPUPlayerPerformanceTests : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;
	std::shared_ptr<MoveGeneration> mGeneration;
	std::shared_ptr<MoveEvaluation> mEvaluation;
	std::shared_ptr<CPUPlayer>		mCPUPlayer;


	void							SetUp() override
	{
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();

		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
		mGeneration = std::make_shared<MoveGeneration>(mBoard, mValidation, mExecution);
		mEvaluation = std::make_shared<MoveEvaluation>(mBoard, mGeneration);
		mCPUPlayer	= std::make_shared<CPUPlayer>(mGeneration, mEvaluation, mBoard);
	}


	std::vector<PossibleMove> getAllLegalMoves(PlayerColor player)
	{
		std::vector<PossibleMove> allMoves;
		mGeneration->calculateAllLegalBasicMoves(player);

		auto playerPieces = mBoard->getPiecesFromPlayer(player);
		for (const auto &[pos, piece] : playerPieces)
		{
			auto moves = mGeneration->getMovesForPosition(pos);
			allMoves.insert(allMoves.end(), moves.begin(), moves.end());
		}

		return allMoves;
	}


	void setupComplexPosition()
	{
		mBoard->removeAllPiecesFromBoard();

		Position whiteKingPos = {4, 7};
		Position blackKingPos = {4, 0};

		// Setup a tactically rich middlegame position
		mBoard->setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
		mBoard->setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
		mBoard->setPiece({3, 6}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));
		mBoard->setPiece({3, 1}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::Black));
		mBoard->setPiece({0, 7}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
		mBoard->setPiece({7, 7}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
		mBoard->setPiece({0, 0}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
		mBoard->setPiece({7, 0}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
		mBoard->setPiece({2, 5}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White));
		mBoard->setPiece({5, 4}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White));
		mBoard->setPiece({2, 2}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::Black));
		mBoard->setPiece({5, 3}, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::Black));
		mBoard->setPiece({1, 5}, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));
		mBoard->setPiece({6, 4}, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));
		mBoard->setPiece({1, 2}, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::Black));
		mBoard->setPiece({6, 3}, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::Black));

		// Add some pawns for tactical complexity
		mBoard->setPiece({0, 6}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
		mBoard->setPiece({2, 6}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
		mBoard->setPiece({4, 5}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
		mBoard->setPiece({7, 6}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
		mBoard->setPiece({0, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
		mBoard->setPiece({3, 2}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
		mBoard->setPiece({4, 2}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
		mBoard->setPiece({7, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

		mBoard->updateKingsPosition(whiteKingPos, PlayerColor::White);
		mBoard->updateKingsPosition(blackKingPos, PlayerColor::Black);
	}


	CPUAlgorithmPerformanceResult benchmarkAlgorithm(const std::string													&algorithmName,
													 int																 depth,
													 const std::string													&position,
													 std::function<PossibleMove(const std::vector<PossibleMove> &, int)> algorithmFunc)
	{
		CPUAlgorithmPerformanceResult result;
		result.algorithmName = algorithmName;
		result.depth		 = depth;
		result.position		 = position;

		auto moves			 = getAllLegalMoves(PlayerColor::White);

		auto start			 = std::chrono::high_resolution_clock::now();
		result.selectedMove	 = algorithmFunc(moves, depth);
		auto end			 = std::chrono::high_resolution_clock::now();

		result.duration		 = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		return result;
	}


	void saveResults(const std::string fileName, const std::vector<CPUAlgorithmPerformanceResult> &results)
	{
		// Create directory if not exists yet
		fs::path resultDir = "CPUPlayer_Results";

		if (!fs::exists(resultDir))
			fs::create_directories(resultDir);

		fs::path	  fullPath = resultDir / fileName;

		std::ofstream file(fullPath, std::ios::app);

		if (!file.is_open())
			return;

		file << "=== CPU Algorithm Performance Test Results ===" << std::endl;
		file << std::setw(12) << "Algorithm" << std::setw(8) << "Depth" << std::setw(12) << "Duration(ms)" << std::setw(12) << "Position" << std::setw(15) << "Selected Move"
			 << std::endl;
		file << std::string(90, '-') << std::endl;

		for (const auto &result : results)
		{
			file << std::setw(12) << result.algorithmName << std::setw(8) << result.depth << std::setw(12) << result.duration.count() << std::setw(12) << result.position
				 << std::setw(15) << "Move" << std::endl; // Could format move notation here
		}
		file << std::endl;
		file.close();
	}
};


TEST_F(CPUPlayerPerformanceTests, MinimaxDepthComparison)
{
	std::vector<CPUAlgorithmPerformanceResult> results;

	auto									   minimaxFunc = [this](const std::vector<PossibleMove> &moves, int depth) { return mCPUPlayer->getMiniMaxMove(moves, depth); };

	// Test different depths
	for (int depth = 2; depth <= 5; ++depth)
	{
		auto result = benchmarkAlgorithm("Minimax", depth, "Opening", minimaxFunc);
		results.push_back(result);
	}

	saveResults("MiniMax Depth Comparison", results);

		// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(CPUPlayerPerformanceTests, AlphaBetaDepthComparison)
{
	std::vector<CPUAlgorithmPerformanceResult> results;

	auto									   alphaBetaFunc = [this](const std::vector<PossibleMove> &moves, int depth) { return mCPUPlayer->getAlphaBetaMove(moves, depth); };

	// Test different depths
	for (int depth = 2; depth <= 6; ++depth)
	{
		auto result = benchmarkAlgorithm("AlphaBeta", depth, "Opening", alphaBetaFunc);
		results.push_back(result);
	}

	saveResults("Alpha Beta Comparison", results);

		// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(CPUPlayerPerformanceTests, AlgorithmComparison)
{
	std::vector<CPUAlgorithmPerformanceResult> results;

	auto									   minimaxFunc	   = [this](const std::vector<PossibleMove> &moves, int depth) { return mCPUPlayer->getMiniMaxMove(moves, depth); };

	auto									   alphaBetaFunc   = [this](const std::vector<PossibleMove> &moves, int depth) { return mCPUPlayer->getAlphaBetaMove(moves, depth); };

	int										   testDepth	   = 4;

	// Test both algorithms at same depth
	auto									   minimaxResult   = benchmarkAlgorithm("Minimax", testDepth, "Opening", minimaxFunc);
	auto									   alphaBetaResult = benchmarkAlgorithm("AlphaBeta", testDepth, "Opening", alphaBetaFunc);

	results.push_back(minimaxResult);
	results.push_back(alphaBetaResult);

	saveResults("Algorithm Comparison", results);

		// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(CPUPlayerPerformanceTests, ComplexPositionPerformance)
{
	setupComplexPosition();

	std::vector<CPUAlgorithmPerformanceResult> results;

	auto									   alphaBetaFunc = [this](const std::vector<PossibleMove> &moves, int depth) { return mCPUPlayer->getAlphaBetaMove(moves, depth); };

	// Test performance on complex tactical position
	auto									   result		 = benchmarkAlgorithm("AlphaBeta", 4, "Complex", alphaBetaFunc);
	results.push_back(result);

	saveResults("Complex Position", results);

		// The results of this test are saved in the file
	SUCCEED();
}


} // namespace PerformanceTests
