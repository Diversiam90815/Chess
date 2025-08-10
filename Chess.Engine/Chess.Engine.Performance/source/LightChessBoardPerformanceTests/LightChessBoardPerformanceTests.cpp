/*
  ==============================================================================
	Module:         LightChessBoard Performance Tests
	Description:    Performance testing for fast board operations in search algorithms
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <chrono>
#include "Board/LightChessBoard.h"

namespace PerformanceTests
{

struct LightChessBoardPerformanceResult
{
	std::string				  testName{};
	std::string				  operation{};
	std::chrono::microseconds duration{};
	size_t					  operationsPerformed{};
	double					  operationsPerSecond{};
	double					  averageOperationTime{};
	std::string				  boardConfiguration{};
	size_t					  moveCount{};
	size_t					  positionCount{};
};


class LightChessBoardPerformanceTests : public ::testing::Test
{
protected:
	std::unique_ptr<LightChessBoard> mBoard;


	void							 SetUp() override
	{
		ChessBoard board;
		board.initializeBoard();
		mBoard = std::make_unique<LightChessBoard>(board);
	}


	void setupMiddlegamePosition()
	{
		ChessBoard tempBoard;
		tempBoard.removeAllPiecesFromBoard();

		// Complex middlegame position
		Position whiteKingPos{4, 7}; // e1
		Position blackKingPos{4, 0}; // e8

		tempBoard.setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
		tempBoard.setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
		tempBoard.setPiece({3, 7}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));
		tempBoard.setPiece({3, 0}, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::Black));
		tempBoard.setPiece({0, 7}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
		tempBoard.setPiece({7, 7}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
		tempBoard.setPiece({0, 0}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
		tempBoard.setPiece({7, 0}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));

		// Add scattered pieces for complexity
		for (int i = 0; i < 8; i += 2)
		{
			tempBoard.setPiece({i, 5}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
			tempBoard.setPiece({i + 1, 2}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
		}

		tempBoard.updateKingsPosition(whiteKingPos, PlayerColor::White);
		tempBoard.updateKingsPosition(blackKingPos, PlayerColor::Black);

		mBoard = std::make_unique<LightChessBoard>(tempBoard);
	}


	void setupEndgamePosition()
	{
		ChessBoard tempBoard;
		tempBoard.removeAllPiecesFromBoard();

		Position whiteKingPos{4, 7}; // e1
		Position blackKingPos{4, 0}; // e8

		tempBoard.setPiece(whiteKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
		tempBoard.setPiece(blackKingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::Black));
		tempBoard.setPiece({0, 7}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
		tempBoard.setPiece({0, 0}, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::Black));
		tempBoard.setPiece({1, 6}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
		tempBoard.setPiece({2, 6}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
		tempBoard.setPiece({1, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
		tempBoard.setPiece({2, 1}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

		tempBoard.updateKingsPosition(whiteKingPos, PlayerColor::White);
		tempBoard.updateKingsPosition(blackKingPos, PlayerColor::Black);

		mBoard = std::make_unique<LightChessBoard>(tempBoard);
	}


	LightChessBoardPerformanceResult benchmarkOperation(
		const std::string &testName, const std::string &operation, const std::string &boardConfig, std::function<void()> operationFunc, size_t iterations)
	{
		LightChessBoardPerformanceResult result;
		result.testName			   = testName;
		result.operation		   = operation;
		result.boardConfiguration  = boardConfig;
		result.operationsPerformed = iterations;

		auto start				   = std::chrono::high_resolution_clock::now();

		for (size_t i = 0; i < iterations; ++i)
		{
			operationFunc();
		}

		auto end					= std::chrono::high_resolution_clock::now();
		result.duration				= std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		double seconds				= result.duration.count() / 1000000.0;
		result.operationsPerSecond	= result.operationsPerformed / seconds;
		result.averageOperationTime = result.duration.count() / static_cast<double>(result.operationsPerformed);

		return result;
	}


	void saveResults(const std::string &fileName, const std::vector<LightChessBoardPerformanceResult> &results)
	{
		// Create directory if not exists yet
		fs::path resultDir = "LightChessBoard_Results";

		if (!fs::exists(resultDir))
			fs::create_directories(resultDir);

		fs::path	  fullPath = resultDir / fileName;

		std::ofstream file(fullPath, std::ios::app);

		if (!file.is_open())
			return;

		file << "=== LightChessBoard Performance Test Results ===" << std::endl;
		file << std::setw(15) << "Test Name" << std::setw(15) << "Operation" << std::setw(15) << "Duration(μs)" << std::setw(12) << "Operations" << std::setw(15) << "Ops/Sec"
			 << std::setw(15) << "Avg Time(μs)" << std::setw(15) << "Board Config" << std::setw(12) << "Moves" << std::setw(12) << "Positions" << std::endl;
		file << std::string(140, '-') << std::endl;

		for (const auto &result : results)
		{
			file << std::setw(15) << result.testName << std::setw(15) << result.operation << std::setw(15) << result.duration.count() << std::setw(12) << result.operationsPerformed
				 << std::setw(15) << static_cast<int>(result.operationsPerSecond) << std::setw(15) << std::fixed << std::setprecision(2) << result.averageOperationTime
				 << std::setw(15) << result.boardConfiguration << std::setw(12) << result.moveCount << std::setw(12) << result.positionCount << std::endl;
		}
		file << std::endl;
		file.close();
	}
};



} // namespace PerformanceTests
