/*
  ==============================================================================
	Module:			Undo Move Tests
	Description:    Testing the Undo Move operations
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "GameEngine.h"


namespace GameMechanicTests
{

class UndoMoveTests : public ::testing::Test
{
protected:
	GameEngine *mEngine;

	void		SetUp() override
	{
		mEngine->init();
		mEngine->startGame();
	}

	void		 TearDown() override { mEngine->resetGame(); }

	// Helper to create a basic move
	PossibleMove CreateMove(Position start, Position end, MoveType type = MoveType::Normal, PieceType promotion = PieceType::DefaultType)
	{
		PossibleMove move;
		move.start			= start;
		move.end			= end;
		move.type			= type;
		move.promotionPiece = promotion;
		return move;
	}

	// Helper to execute a move through GameManager
	void ExecuteMove(const PossibleMove &move)
	{
		mEngine->calculateAllMovesForPlayer();
		mEngine->initiateMove(move.start);

		PossibleMove mutableMove = move;
		mEngine->executeMove(mutableMove);
		mEngine->switchTurns();
	}

	// Helper to get piece at position
	PieceType GetPieceAtPosition(Position pos)
	{
		int boardState[8][8];
		mEngine->getBoardState(boardState);

		int encoded = boardState[pos.y][pos.x];
		int typeVal = encoded & 0xF;

		return static_cast<PieceType>(typeVal);
	}

	// Helper to get piece color at position
	PlayerColor GetPieceColorAtPosition(Position pos)
	{
		int boardState[8][8];
		mEngine->getBoardState(boardState);

		int encoded	 = boardState[pos.y][pos.x];
		int colorVal = (encoded >> 4) & 0xF;

		return static_cast<PlayerColor>(colorVal);
	}
};


TEST_F(UndoMoveTests, UndoMoveWithoutAnyMoves)
{
	// Test undoing when no moves have been made
	EXPECT_NO_THROW(mEngine->undoMove()) << "undoMove should not throw when no moves exist";

	// Board should still be in initial state
	EXPECT_EQ(GetPieceAtPosition({4, 6}), PieceType::Pawn) << "White pawn should still be at e2";
	EXPECT_EQ(GetPieceColorAtPosition({4, 6}), PlayerColor::White) << "Piece at e2 should be white";
}


TEST_F(UndoMoveTests, UndoSimplePawnMove)
{
	// Execute a simple pawn move (e2-e4)
	PossibleMove pawnMove = CreateMove({4, 6}, {4, 4}, MoveType::Normal);
	ExecuteMove(pawnMove);

	// Verify move was executed
	EXPECT_EQ(GetPieceAtPosition({4, 4}), PieceType::Pawn) << "Pawn should be at e4 after move";
	EXPECT_EQ(GetPieceAtPosition({4, 6}), PieceType::DefaultType) << "e2 should be empty after move";

	// Undo the move
	mEngine->undoMove();

	// Verify move was undone
	EXPECT_EQ(GetPieceAtPosition({4, 6}), PieceType::Pawn) << "Pawn should be back at e2 after undo";
	EXPECT_EQ(GetPieceColorAtPosition({4, 6}), PlayerColor::White) << "Pawn at e2 should be white";
	EXPECT_EQ(GetPieceAtPosition({4, 4}), PieceType::DefaultType) << "e4 should be empty after undo";
}


TEST_F(UndoMoveTests, UndoCaptureMove)
{
	// Set up a capture scenario
	ExecuteMove(CreateMove({4, 6}, {4, 4}, MoveType::Normal)); // e2-e4
	ExecuteMove(CreateMove({3, 1}, {3, 3}, MoveType::Normal)); // d7-d5
	ExecuteMove(CreateMove({4, 4}, {3, 3}, MoveType::Capture)); // exd5

	// Verify capture was executed
	EXPECT_EQ(GetPieceAtPosition({3, 3}), PieceType::Pawn) << "White pawn should be at d5 after capture";
	EXPECT_EQ(GetPieceColorAtPosition({3, 3}), PlayerColor::White) << "Piece at d5 should be white";

	// Undo the capture
	mEngine->undoMove();

	// Verify capture was undone
	EXPECT_EQ(GetPieceAtPosition({4, 4}), PieceType::Pawn) << "White pawn should be back at e4";
	EXPECT_EQ(GetPieceColorAtPosition({4, 4}), PlayerColor::White) << "Pawn at e4 should be white";
	EXPECT_EQ(GetPieceAtPosition({3, 3}), PieceType::Pawn) << "Black pawn should be restored at d5";
	EXPECT_EQ(GetPieceColorAtPosition({3, 3}), PlayerColor::Black) << "Pawn at d5 should be black";
}



TEST_F(UndoMoveTests, UndoMultipleMoves)
{
	// Execute multiple moves
	ExecuteMove(CreateMove({4, 6}, {4, 4}, MoveType::Normal)); // e2-e4
	ExecuteMove(CreateMove({4, 1}, {4, 3}, MoveType::Normal)); // e7-e5
	ExecuteMove(CreateMove({6, 7}, {5, 5}, MoveType::Normal)); // g1-f3

	// Verify final state
	EXPECT_EQ(GetPieceAtPosition({5, 5}), PieceType::Knight) << "Knight should be at f3";

	// Undo last move (knight)
	mEngine->undoMove();
	EXPECT_EQ(GetPieceAtPosition({6, 7}), PieceType::Knight) << "Knight should be back at g1";
	EXPECT_EQ(GetPieceAtPosition({5, 5}), PieceType::DefaultType) << "f3 should be empty";

	// Undo second move (black pawn)
	mEngine->undoMove();
	EXPECT_EQ(GetPieceAtPosition({4, 1}), PieceType::Pawn) << "Black pawn should be back at e7";
	EXPECT_EQ(GetPieceAtPosition({4, 3}), PieceType::DefaultType) << "e5 should be empty";

	// Undo first move (white pawn)
	mEngine->undoMove();
	EXPECT_EQ(GetPieceAtPosition({4, 6}), PieceType::Pawn) << "White pawn should be back at e2";
	EXPECT_EQ(GetPieceAtPosition({4, 4}), PieceType::DefaultType) << "e4 should be empty";
}



TEST_F(UndoMoveTests, MultipleUndoCallsWithoutMoves)
{
	// Call undo multiple times without any moves
	EXPECT_NO_THROW(mEngine->undoMove()) << "First undo without moves should not throw";
	EXPECT_NO_THROW(mEngine->undoMove()) << "Second undo without moves should not throw";
	EXPECT_NO_THROW(mEngine->undoMove()) << "Third undo without moves should not throw";
}


TEST_F(UndoMoveTests, UndoMoreMovesThanExecuted)
{
	// Execute one move
	ExecuteMove(CreateMove({4, 6}, {4, 4}, MoveType::Normal));

	// Undo it
	mEngine->undoMove();

	// Try to undo again (should handle gracefully)
	EXPECT_NO_THROW(mEngine->undoMove()) << "Undo beyond available moves should not throw";
}


} // namespace GameMechanicTests
