/*
  ==============================================================================
	Module:			Move Generation Tests
	Description:    Testing the general move generation module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Generation/MoveGeneration.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"


namespace MoveTests
{


class MoveGenerationTest : public ::testing::Test
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

	bool isCaptureMove(const PossibleMove &move) { return (move.type & MoveType::Capture) == MoveType::Capture; }
};


TEST_F(MoveGenerationTest, GetMovesForPositionReturnsLegalMoves)
{
	// e2 pawn
	Position pawnPos = {4, 6};
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves = mGeneration->getMovesForPosition(pawnPos);

	EXPECT_FALSE(moves.empty()) << "Should have at least one move";
}


TEST_F(MoveGenerationTest, CalculateAllLegalBasicMovesReturnsTrue)
{
	bool result = mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);

	EXPECT_TRUE(result) << "Must return true when calculated more than one legal moves";
}


TEST_F(MoveGenerationTest, PawnInitialMovesCalculatedCorrectly)
{
	// Test pawn at e2
	Position pawnPos = {4, 6};
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves = mGeneration->getMovesForPosition(pawnPos);

	// Verify: Should have exactly two moves (e2->e3, e2->e4)
	EXPECT_EQ(moves.size(), 2) << "Pawn at e2 should have excactly two moves (e3 and e4)";

	// Check that one is single and one is double step
	bool hasSingleStep = false;
	bool hasDoubleStep = false;

	for (const auto &move : moves)
	{
		if (move.end.y == 5) // e3
			hasSingleStep = true;
		if (move.end.y == 4) // e4
			hasDoubleStep = true;
	}

	EXPECT_TRUE(hasSingleStep) << "Pawn should be able to move one square forward (e2->e3)";
	EXPECT_TRUE(hasDoubleStep) << "Pawn should be abe to move two squares forward (e2->e4)";
}


TEST_F(MoveGenerationTest, PawnCapturesCalculatedCorrectly)
{
	mBoard->removeAllPiecesFromBoard();

	// Create pawn & increase move counter of the pawn to prohibit double pawn push
	auto pawn = ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White);
	pawn->increaseMoveCounter();

	// Set up position with white pawn at e4 and black pieces at d5 and f5
	Position pawnPos = Position{4, 4};
	mBoard->setPiece(pawnPos, pawn);
	mBoard->setPiece({3, 3}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
	mBoard->setPiece({5, 3}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

	// Generate moves
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves = mGeneration->getMovesForPosition(pawnPos);

	// Verify: Should have 3 moves: e4-e5, e4-d5 (capture), and e4-f5 (capture)
	EXPECT_EQ(moves.size(), 3) << "Pawn should have 3 moves: forward and two captures";

	// check for move types
	bool hasForwardMove	 = false;
	bool hasLeftCapture	 = false;
	bool hasRightCapture = false;

	for (const auto &move : moves)
	{
		if (move.end.x == 4 && move.end.y == 3)								// e5
			hasForwardMove = true;
		else if (move.end.x == 3 && move.end.y == 3 && isCaptureMove(move)) // d5
			hasLeftCapture = true;
		else if (move.end.x == 5 && move.end.y == 3 && isCaptureMove(move)) // f5
			hasRightCapture = true;
	}

	EXPECT_TRUE(hasForwardMove) << "Pawn should be able to move forward";
	EXPECT_TRUE(hasLeftCapture) << "Pawn should be able to capture diagonally left";
	EXPECT_TRUE(hasRightCapture) << "Pawn should be able to capture diagonally right";
}


TEST_F(MoveGenerationTest, BlockedPawnGeneratesNoMove)
{
	mBoard->removeAllPiecesFromBoard();

	// White pawn at e4 blocked by a piece at e5
	mBoard->setPiece({4, 4}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
	mBoard->setPiece({4, 3}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

	// Generate moves
	Position pawnPos = {4, 4}; // e4
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves			= mGeneration->getMovesForPosition(pawnPos);

	// Check for forward move
	bool hasForwardMove = false;

	for (const auto &move : moves)
	{
		if (move.end.x == 4 && move.end.y == 3) // e5
			hasForwardMove = true;
	}

	// Verify: Pawn has no forward move since it's been blocked
	EXPECT_FALSE(hasForwardMove) << "Blocked pawn should not be able to move forward";
}


TEST_F(MoveGenerationTest, KnightMovesCalculatedCorrectly)
{
	mBoard->removeAllPiecesFromBoard();

	// Place Knight on empty  board at e4
	Position knightPos = {4, 4};
	mBoard->setPiece(knightPos, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));

	// Generate moves
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto				  moves			= mGeneration->getMovesForPosition(knightPos);

	// Define all 8 possible L-shaped moves from e4
	std::vector<Position> expectedMoves = {
		{2, 3}, // c5
		{2, 5}, // c3
		{3, 2}, // d6
		{3, 6}, // d2
		{5, 2}, // f6
		{5, 6}, // f2
		{6, 3}, // g5
		{6, 5}	// g3
	};

	// Verify : Knight moves calculated correctly
	EXPECT_EQ(moves.size(), expectedMoves.size()) << "Knight in the center should have 8 possible L-shaped moves";

	for (const auto &expected : expectedMoves)
	{
		bool moveFound = false;
		for (const auto &move : moves)
		{
			if (move.end.x == expected.x && move.end.y == expected.y)
			{
				moveFound = true;
				break;
			}
		}
		EXPECT_TRUE(moveFound) << "Knight should be able to move to position (" << expected.x << "," << expected.y << ")";
	}
}


TEST_F(MoveGenerationTest, KnightAtCornerCalculatedCorrectly)
{
	mBoard->removeAllPiecesFromBoard();

	// Place knight at corner a1
	Position knightPos = {0, 7};
	mBoard->setPiece(knightPos, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));

	// Generate moves
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto				  moves			= mGeneration->getMovesForPosition(knightPos);

	// Define possible moves
	std::vector<Position> expectedMoves = {
		{1, 5}, // b3
		{2, 6}	// c2
	};

	// Verify: Knight should have 2 possible moves
	EXPECT_EQ(moves.size(), expectedMoves.size());

	for (const auto &expected : expectedMoves)
	{
		bool moveFound = false;
		for (const auto &move : moves)
		{
			if (move.end.x == expected.x && move.end.y == expected.y)
			{
				moveFound = true;
				break;
			}
		}
		EXPECT_TRUE(moveFound) << "Knight should be able to move to position (" << expected.x << "," << expected.y << ")";
	}
}


TEST_F(MoveGenerationTest, BishopMovesCalculatedCorrectly)
{
	mBoard->removeAllPiecesFromBoard();

	// Place Bishop on an empty board at e4
	Position bishopPos = {4, 4};
	mBoard->setPiece(bishopPos, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White));

	// Generate moves
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves = mGeneration->getMovesForPosition(bishopPos);

	// Verify: Bishop should have 13 possible moves in every diagonal direction

	EXPECT_EQ(moves.size(), 13) << "Bishop in the center should have 13 diagonal moves";

	bool hasNorthEast = false;
	bool hasNorthWest = false;
	bool hasSouthEast = false;
	bool hasSouthWest = false;

	for (const auto &move : moves)
	{
		// Northeast diagonal
		if (move.end.x > bishopPos.x && move.end.y < bishopPos.y)
			hasNorthEast = true;
		// Northwest diagonal
		else if (move.end.x < bishopPos.x && move.end.y < bishopPos.y)
			hasNorthWest = true;
		// Southeast diagonal
		else if (move.end.x > bishopPos.x && move.end.y > bishopPos.y)
			hasSouthEast = true;
		// Southwest diagonal
		else if (move.end.x < bishopPos.x && move.end.y > bishopPos.y)
			hasSouthWest = true;
	}

	EXPECT_TRUE(hasNorthEast) << "Bishop should be able to move northeast";
	EXPECT_TRUE(hasNorthWest) << "Bishop should be able to move northwest";
	EXPECT_TRUE(hasSouthEast) << "Bishop should be able to move southeast";
	EXPECT_TRUE(hasSouthWest) << "Bishop should be able to move southwest";
}


TEST_F(MoveGenerationTest, BishopBlockedMovesCalculatedCorrectly)
{
	mBoard->removeAllPiecesFromBoard();

	// Place bishop at e4 with blocking pieces at c6 (northwest) and f5 (northeast)
	Position bishopPos = {4, 4}; // e4
	mBoard->setPiece(bishopPos, ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::White));
	mBoard->setPiece({2, 2}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
	mBoard->setPiece({5, 3}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

	// Generate moves
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves			 = mGeneration->getMovesForPosition(bishopPos);

	// Verify: Bishop's moves are blocked by piece and can capture black pawn at f5
	bool canCaptureF5	 = false;
	bool canMoveBeyondC6 = false;
	bool canMoveToD5	 = false; // Square between bishop and blocking piece

	for (const auto &move : moves)
	{
		if (move.end.x == 5 && move.end.y == 3) // f5
			canCaptureF5 = true;
		if (move.end.x == 1 && move.end.y == 1) // b7 (beyond c6)
			canMoveBeyondC6 = true;
		if (move.end.x == 3 && move.end.y == 3) // d5
			canMoveToD5 = true;
	}

	EXPECT_TRUE(canCaptureF5) << "Bishop should be able to capture enemy piece at f5";
	EXPECT_FALSE(canMoveBeyondC6) << "Bishop should not be able to move beyond the blocking piece at c6";
	EXPECT_TRUE(canMoveToD5) << "Bishop should be able to move to d5, which is before the blocking piece";
}


TEST_F(MoveGenerationTest, RookMovesCalculatedCorrectly)
{
	mBoard->removeAllPiecesFromBoard();

	// Place rook on empty board at e4
	Position rookPos = {4, 4};
	mBoard->setPiece(rookPos, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));

	// Generate moves
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves = mGeneration->getMovesForPosition(rookPos);

	// Verify: Rook at e4 should have 14 possible moves (horizontal and vertical)
	EXPECT_EQ(moves.size(), 14) << "Rook in the center should have 14 straight-line moves";

	bool hasNorth = false;
	bool hasSouth = false;
	bool hasEast  = false;
	bool hasWest  = false;

	for (const auto &move : moves)
	{
		if (move.end.x == rookPos.x && move.end.y < rookPos.y)		// North direction
			hasNorth = true;
		else if (move.end.x == rookPos.x && move.end.y > rookPos.y) // South direction
			hasSouth = true;
		else if (move.end.x > rookPos.x && move.end.y == rookPos.y) // East direction
			hasEast = true;
		else if (move.end.x < rookPos.x && move.end.y == rookPos.y) // West direction
			hasWest = true;
	}

	EXPECT_TRUE(hasNorth) << "Rook should be able to move north";
	EXPECT_TRUE(hasSouth) << "Rook should be able to move south";
	EXPECT_TRUE(hasEast) << "Rook should be able to move east";
	EXPECT_TRUE(hasWest) << "Rook should be able to move west";
}


TEST_F(MoveGenerationTest, QueenMovesCalculatedCorrectly)
{
	mBoard->removeAllPiecesFromBoard();

	// Queen on an empty board at e4
	Position queenPos = {4, 4}; // e4
	mBoard->setPiece(queenPos, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White));

	// Generate moves
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto moves = mGeneration->getMovesForPosition(queenPos);

	// Verify: Queen in e4 should have 27 possible moves (8 directions)
	EXPECT_EQ(moves.size(), 27) << "Queen in the center should have 27 moves combining rook and bishop moves";

	// Test that moves exist in all eight directions
	bool hasNorth	  = false;
	bool hasSouth	  = false;
	bool hasEast	  = false;
	bool hasWest	  = false;
	bool hasNorthEast = false;
	bool hasNorthWest = false;
	bool hasSouthEast = false;
	bool hasSouthWest = false;

	for (const auto &move : moves)
	{
		if (move.end.x == queenPos.x && move.end.y < queenPos.y)	  // North direction
			hasNorth = true;
		else if (move.end.x == queenPos.x && move.end.y > queenPos.y) // South direction
			hasSouth = true;
		else if (move.end.x > queenPos.x && move.end.y == queenPos.y) // East direction
			hasEast = true;
		else if (move.end.x < queenPos.x && move.end.y == queenPos.y) // West direction
			hasWest = true;
		else if (move.end.x > queenPos.x && move.end.y < queenPos.y)  // Northeast diagonal
			hasNorthEast = true;
		else if (move.end.x < queenPos.x && move.end.y < queenPos.y)  // Northwest diagonal
			hasNorthWest = true;
		else if (move.end.x > queenPos.x && move.end.y > queenPos.y)  // Southeast diagonal
			hasSouthEast = true;
		else if (move.end.x < queenPos.x && move.end.y > queenPos.y)  // Southwest diagonal
			hasSouthWest = true;
	}

	EXPECT_TRUE(hasNorth) << "Queen should be able to move north";
	EXPECT_TRUE(hasSouth) << "Queen should be able to move south";
	EXPECT_TRUE(hasEast) << "Queen should be able to move east";
	EXPECT_TRUE(hasWest) << "Queen should be able to move west";
	EXPECT_TRUE(hasNorthEast) << "Queen should be able to move northeast";
	EXPECT_TRUE(hasNorthWest) << "Queen should be able to move northwest";
	EXPECT_TRUE(hasSouthEast) << "Queen should be able to move southeast";
	EXPECT_TRUE(hasSouthWest) << "Queen should be able to move southwest";
}


TEST_F(MoveGenerationTest, KingMovesCalculatedCorrectly)
{
	mBoard->removeAllPiecesFromBoard();

	// Place King in the middle of the board at e4
	Position kingPos = {4, 4}; // e4
	mBoard->setPiece(kingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->updateKingsPosition(kingPos, PlayerColor::White);

	// Generate moves
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto				  moves			= mGeneration->getMovesForPosition(kingPos);

	// Define all 8 possible one-square moves from e4
	std::vector<Position> expectedMoves = {
		{3, 3}, // d5
		{3, 4}, // d4
		{3, 5}, // d3
		{4, 3}, // e5
		{4, 5}, // e3
		{5, 3}, // f5
		{5, 4}, // f4
		{5, 5}	// f3
	};

	// Verify: King in e4 should have 8 possible moves (one square in each direction)
	EXPECT_EQ(moves.size(), expectedMoves.size()) << "King in the center should have 8 moves (one in each direction)";

	for (const auto &expected : expectedMoves)
	{
		bool moveFound = false;
		for (const auto &move : moves)
		{
			if (move.end.x == expected.x && move.end.y == expected.y)
			{
				moveFound = true;
				break;
			}
		}
		EXPECT_TRUE(moveFound) << "King should be able to move to position (" << expected.x << "," << expected.y << ")";
	}
}


TEST_F(MoveGenerationTest, PiecesCannotMoveIntoCheck)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup: White king at e1, white rook at d1, black queen at h1
	// The rook can't move because it's pinned to the king
	Position kingPos  = {4, 7}; // e1
	Position rookPos  = {3, 7}; // d1
	Position queenPos = {7, 7}; // h1

	mBoard->setPiece(kingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(rookPos, ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White));
	mBoard->setPiece(queenPos, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::Black));

	mBoard->updateKingsPosition(kingPos, PlayerColor::White);

	// Generate Moves
	mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto rookMoves = mGeneration->getMovesForPosition(rookPos);
	auto kingMoves = mGeneration->getMovesForPosition(kingPos);

	// Verify: Rook should not be able to move because it's pinned to the king
	EXPECT_TRUE(rookMoves.empty()) << "Pinned rook should not have any legal moves";

	// Verify: King should only be able to move up or block the check by moving to f1
	bool canMoveNorth  = false;
	bool canBlockCheck = false;

	for (const auto &move : kingMoves)
	{
		if (move.end.x == 4 && move.end.y == 6) // e2 (north)
			canMoveNorth = true;
		if (move.end.x == 5 && move.end.y == 7) // f1 (block check)
			canBlockCheck = true;
	}

	EXPECT_TRUE(canMoveNorth) << "King should be able to escape check by moving north";
	EXPECT_FALSE(canBlockCheck) << "King should not be able move in check by moving to f1";
}


TEST_F(MoveGenerationTest, CheckmatePositionHasNoLegalMoves)
{
	mBoard->removeAllPiecesFromBoard();

	// Setup a basic checkmate position: White king at h1, black queens at g1 and g2
	Position kingPos   = {7, 7}; // h1
	Position queen1Pos = {6, 7}; // g1
	Position queen2Pos = {6, 6}; // g2

	mBoard->setPiece(kingPos, ChessPiece::CreatePiece(PieceType::King, PlayerColor::White));
	mBoard->setPiece(queen1Pos, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::Black));
	mBoard->setPiece(queen2Pos, ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::Black));

	mBoard->updateKingsPosition(kingPos, PlayerColor::White);

	// Calculate all legal moves for White
	bool hasLegalMoves = mGeneration->calculateAllLegalBasicMoves(PlayerColor::White);
	auto kingMoves	   = mGeneration->getMovesForPosition(kingPos);

	// In checkmate, there should be no legal moves
	EXPECT_FALSE(hasLegalMoves) << "In checkmate position, calculating legal moves should return false";
	EXPECT_TRUE(kingMoves.empty()) << "King in checkmate should have no legal moves";
}

} // namespace MoveTest
