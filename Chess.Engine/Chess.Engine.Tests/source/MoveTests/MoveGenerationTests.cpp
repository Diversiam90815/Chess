/*
  ==============================================================================
	Module:			Move Generation Tests
	Description:    Testing the general move generation module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveGeneration.h"
#include "MoveValidation.h"
#include "MoveExecution.h"
#include "TestHelper.h"

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

	// Set up position with white pawn at e4 and black pieces at d5 and f5
	mBoard->setPiece({4, 4}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::White));
	mBoard->setPiece({3, 3}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));
	mBoard->setPiece({5, 3}, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

	Position pawnPos = Position{4, 4};
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
