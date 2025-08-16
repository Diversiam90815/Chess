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
	int						  operationsPerformed{};
	double					  operationsPerSecond{};
	double					  averageOperationTime{};
	std::string				  boardConfiguration{};
	int						  moveCount{};
	int						  positionCount{};
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


TEST_F(LightChessBoardPerformanceTests, MakeMoveUnmoveMoveSpeed)
{
	auto		  moves		 = mBoard->generateLegalMoves(PlayerColor::White);
	constexpr int iterations = 10000;

	auto		  result	 = benchmarkOperation(
		 "MakeUnmake", "Make/Unmake", "Opening",
		 [this, &moves]()
		 {
			 for (const auto &move : moves)
			 {
				 auto undoInfo = mBoard->makeMove(move);
				 mBoard->unmakeMove(undoInfo);
			 }
		 },
		 iterations);

	result.moveCount									  = moves.size();
	result.operationsPerformed							  = iterations * moves.size();

	// Recalculate with correct operation count
	double seconds										  = result.duration.count() / 1000000.0;
	result.operationsPerSecond							  = result.operationsPerformed / seconds;

	std::vector<LightChessBoardPerformanceResult> results = {result};
	saveResults("Make-Unmake Move", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(LightChessBoardPerformanceTests, MoveGenerationSpeed)
{
	constexpr int iterations = 5000;

	auto		  result	 = benchmarkOperation(
		 "MoveGen", "GenerateMoves", "Opening",
		 [this]()
		 {
			 auto moves = mBoard->generateLegalMoves(PlayerColor::White);
			 (void)moves; // Suppress unused variable warning
		 },
		 iterations);

	std::vector<LightChessBoardPerformanceResult> results = {result};
	saveResults("Move Generation", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(LightChessBoardPerformanceTests, PositionEvaluationSpeed)
{
	constexpr int iterations = 10000;

	auto		  result	 = benchmarkOperation(
		 "PosEval", "GetMaterial", "Opening",
		 [this]()
		 {
			 int whiteValue = mBoard->getMaterialValue(PlayerColor::White);
			 int blackValue = mBoard->getMaterialValue(PlayerColor::Black);
			 (void)whiteValue;
			 (void)blackValue; // Suppress warnings
		 },
		 iterations);

	std::vector<LightChessBoardPerformanceResult> results = {result};
	saveResults("Position Evaluation", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(LightChessBoardPerformanceTests, BoardCopyingSpeed)
{
	constexpr int iterations = 1000;

	auto		  result	 = benchmarkOperation(
		 "BoardCopy", "CopyBoard", "Opening",
		 [this]()
		 {
			 LightChessBoard copy(*mBoard);
			 (void)copy; // Suppress warning
		 },
		 iterations);

	std::vector<LightChessBoardPerformanceResult> results = {result};
	saveResults("Board Copying", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(LightChessBoardPerformanceTests, CheckDetectionSpeed)
{
	constexpr int iterations = 5000;

	auto		  result	 = benchmarkOperation(
		 "CheckDet", "IsInCheck", "Opening",
		 [this]()
		 {
			 bool whiteCheck = mBoard->isInCheck(PlayerColor::White);
			 bool blackCheck = mBoard->isInCheck(PlayerColor::Black);
			 (void)whiteCheck;
			 (void)blackCheck; // Suppress warnings
		 },
		 iterations);

	std::vector<LightChessBoardPerformanceResult> results = {result};
	saveResults("Check Detection", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(LightChessBoardPerformanceTests, MiddlegamePerformanceComparison)
{
	setupMiddlegamePosition();

	std::vector<LightChessBoardPerformanceResult> results;

	// Test different operations on middlegame position
	auto										  moves			   = mBoard->generateLegalMoves(PlayerColor::White);

	// Make/Unmake performance
	auto										  makeUnmakeResult = benchmarkOperation(
		 "MakeUnmake", "Make/Unmake", "Middlegame",
		 [this, &moves]()
		 {
			 for (const auto &move : moves)
			 {
				 auto undoInfo = mBoard->makeMove(move);
				 mBoard->unmakeMove(undoInfo);
			 }
		 },
		 1000);
	makeUnmakeResult.moveCount			 = moves.size();
	makeUnmakeResult.operationsPerformed = 1000 * moves.size();
	double seconds						 = makeUnmakeResult.duration.count() / 1000000.0;
	makeUnmakeResult.operationsPerSecond = makeUnmakeResult.operationsPerformed / seconds;
	results.push_back(makeUnmakeResult);

	// Move generation performance
	auto moveGenResult = benchmarkOperation(
		"MoveGen", "GenerateMoves", "Middlegame",
		[this]()
		{
			auto moves = mBoard->generateLegalMoves(PlayerColor::White);
			(void)moves;
		},
		2000);
	results.push_back(moveGenResult);

	// Material evaluation performance
	auto materialResult = benchmarkOperation(
		"Material", "GetMaterial", "Middlegame",
		[this]()
		{
			int value = mBoard->getMaterialValue(PlayerColor::White);
			(void)value;
		},
		5000);
	results.push_back(materialResult);

	saveResults("Middle Game Comparison", results);

	// The results of this test are saved in the file
	SUCCEED();
}


TEST_F(LightChessBoardPerformanceTests, EndgamePerformanceComparison)
{
	setupEndgamePosition();

	std::vector<LightChessBoardPerformanceResult> results;

	auto										  moves			   = mBoard->generateLegalMoves(PlayerColor::White);

	// Make/Unmake performance in endgame
	auto										  makeUnmakeResult = benchmarkOperation(
		 "MakeUnmake", "Make/Unmake", "Endgame",
		 [this, &moves]()
		 {
			 for (const auto &move : moves)
			 {
				 auto undoInfo = mBoard->makeMove(move);
				 mBoard->unmakeMove(undoInfo);
			 }
		 },
		 2000);
	makeUnmakeResult.moveCount			 = moves.size();
	makeUnmakeResult.operationsPerformed = 2000 * moves.size();
	double seconds						 = makeUnmakeResult.duration.count() / 1000000.0;
	makeUnmakeResult.operationsPerSecond = makeUnmakeResult.operationsPerformed / seconds;
	results.push_back(makeUnmakeResult);

	// Move generation in endgame should be faster
	auto moveGenResult = benchmarkOperation(
		"MoveGen", "GenerateMoves", "Endgame",
		[this]()
		{
			auto moves = mBoard->generateLegalMoves(PlayerColor::White);
			(void)moves;
		},
		5000);
	results.push_back(moveGenResult);

	saveResults("Endgame Performance", results);

	// The results of this test are saved in the file
	SUCCEED();
}


} // namespace PerformanceTests
