/*
  ==============================================================================
	Module:			Move History Tests
	Description:    Testing move history management and undo functionality
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "GameEngine.h"
#include "Execution/MoveExecution.h"

namespace MoveTests
{

class MoveHistoryTests : public ::testing::Test
{
protected:
	std::shared_ptr<ChessBoard>		mBoard;
	std::shared_ptr<MoveValidation> mValidation;
	std::shared_ptr<MoveExecution>	mExecution;

	void							SetUp() override
	{
		mBoard = std::make_shared<ChessBoard>();
		mBoard->initializeBoard();
		mValidation = std::make_shared<MoveValidation>(mBoard);
		mExecution	= std::make_shared<MoveExecution>(mBoard, mValidation);
	}

	void TearDown() override
	{
		mExecution->clearMoveHistory();
		mBoard.reset();
		mValidation.reset();
		mExecution.reset();
	}

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

	// Helper to create a Move object directly
	Move CreateMoveObject(Position start, Position end, PieceType movedPiece, PieceType capturedPiece = PieceType::DefaultType, PlayerColor player = PlayerColor::White)
	{
		Move move(start, end, movedPiece, capturedPiece);
		move.player = player;
		return move;
	}
};


TEST_F(MoveHistoryTests, InitiallyEmptyHistory)
{
	// Move history should be empty initially
	const Move *lastMove = mExecution->getLastMove();
	EXPECT_EQ(lastMove, nullptr) << "Move history should be empty initially";
}


TEST_F(MoveHistoryTests, AddMoveToHistoryBasic)
{
	// Create and add a move to history
	Move testMove = CreateMoveObject({4, 6}, {4, 4}, PieceType::Pawn, PieceType::DefaultType, PlayerColor::White);
	mExecution->addMoveToHistory(testMove);

	// Verify move was added
	const Move *lastMove = mExecution->getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Last move should not be null after adding move";
	EXPECT_EQ(lastMove->startingPosition, Position({4, 6})) << "Starting position should match";
	EXPECT_EQ(lastMove->endingPosition, Position({4, 4})) << "Ending position should match";
	EXPECT_EQ(lastMove->movedPiece, PieceType::Pawn) << "Moved piece should be pawn";
	EXPECT_EQ(lastMove->player, PlayerColor::White) << "Player should be white";
	EXPECT_EQ(lastMove->number, 1) << "First move should have number 1";
}


TEST_F(MoveHistoryTests, AddMultipleMovesToHistory)
{
	// Add multiple moves
	Move move1 = CreateMoveObject({4, 6}, {4, 4}, PieceType::Pawn, PieceType::DefaultType, PlayerColor::White);
	Move move2 = CreateMoveObject({4, 1}, {4, 3}, PieceType::Pawn, PieceType::DefaultType, PlayerColor::Black);
	Move move3 = CreateMoveObject({6, 7}, {5, 5}, PieceType::Knight, PieceType::DefaultType, PlayerColor::White);

	mExecution->addMoveToHistory(move1);
	mExecution->addMoveToHistory(move2);
	mExecution->addMoveToHistory(move3);

	// Verify the last move is correct
	const Move *lastMove = mExecution->getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Last move should not be null";
	EXPECT_EQ(lastMove->movedPiece, PieceType::Knight) << "Last move should be knight";
	EXPECT_EQ(lastMove->player, PlayerColor::White) << "Last move should be by white";
	EXPECT_EQ(lastMove->number, 3) << "Third move should have number 3";
}


TEST_F(MoveHistoryTests, MoveNumberingSequential)
{
	// Add several moves and verify numbering
	for (int i = 0; i < 5; ++i)
	{
		Move move = CreateMoveObject({i, 6}, {i, 4}, PieceType::Pawn);
		mExecution->addMoveToHistory(move);

		const Move *lastMove = mExecution->getLastMove();
		ASSERT_NE(lastMove, nullptr) << "Move should be added to history";
		EXPECT_EQ(lastMove->number, i + 1) << "Move number should be sequential";
	}
}


TEST_F(MoveHistoryTests, RemoveLastMoveFromHistory)
{
	// Add two moves
	Move move1 = CreateMoveObject({4, 6}, {4, 4}, PieceType::Pawn, PieceType::DefaultType, PlayerColor::White);
	Move move2 = CreateMoveObject({4, 1}, {4, 3}, PieceType::Pawn, PieceType::DefaultType, PlayerColor::Black);

	mExecution->addMoveToHistory(move1);
	mExecution->addMoveToHistory(move2);

	// Verify we have the second move as last
	const Move *lastMove = mExecution->getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Should have a last move";
	EXPECT_EQ(lastMove->player, PlayerColor::Black) << "Last move should be by black";

	// Remove last move
	mExecution->removeLastMove();

	// Verify the first move is now the last move
	lastMove = mExecution->getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Should still have a move after removal";
	EXPECT_EQ(lastMove->player, PlayerColor::White) << "Last move should now be by white";
	EXPECT_EQ(lastMove->number, 1) << "Last move should be the first move";
}


TEST_F(MoveHistoryTests, RemoveLastMoveFromEmptyHistory)
{
	// Try to remove from empty history - should not crash
	EXPECT_NO_THROW(mExecution->removeLastMove()) << "Removing from empty history should not throw";

	const Move *lastMove = mExecution->getLastMove();
	EXPECT_EQ(lastMove, nullptr) << "History should still be empty";
}


TEST_F(MoveHistoryTests, RemoveAllMovesFromHistory)
{
	// Add several moves
	for (int i = 0; i < 3; ++i)
	{
		Move move = CreateMoveObject({i, 6}, {i, 4}, PieceType::Pawn);
		mExecution->addMoveToHistory(move);
	}

	// Remove all moves one by one
	mExecution->removeLastMove();
	mExecution->removeLastMove();
	mExecution->removeLastMove();

	// History should be empty
	const Move *lastMove = mExecution->getLastMove();
	EXPECT_EQ(lastMove, nullptr) << "History should be empty after removing all moves";

	// Try to remove one more - should not crash
	EXPECT_NO_THROW(mExecution->removeLastMove()) << "Removing from empty history should not throw";
}


TEST_F(MoveHistoryTests, ClearMoveHistory)
{
	// Add several moves
	for (int i = 0; i < 5; ++i)
	{
		Move move = CreateMoveObject({i, 6}, {i, 4}, PieceType::Pawn);
		mExecution->addMoveToHistory(move);
	}

	// Verify we have moves
	const Move *lastMove = mExecution->getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Should have moves before clearing";

	// Clear history
	mExecution->clearMoveHistory();

	// Verify history is empty
	lastMove = mExecution->getLastMove();
	EXPECT_EQ(lastMove, nullptr) << "History should be empty after clearing";
}


TEST_F(MoveHistoryTests, ClearEmptyHistory)
{
	// Clear already empty history - should not crash
	EXPECT_NO_THROW(mExecution->clearMoveHistory()) << "Clearing empty history should not throw";

	const Move *lastMove = mExecution->getLastMove();
	EXPECT_EQ(lastMove, nullptr) << "History should remain empty";
}


TEST_F(MoveHistoryTests, ExecuteMoveAddsToHistory)
{
	// Execute a valid pawn move
	PossibleMove pawnMove	  = CreateMove({4, 6}, {4, 4}, MoveType::Normal); // e2-e4

	Move		 executedMove = mExecution->executeMove(pawnMove);

	// Verify move was added to history
	const Move	*lastMove	  = mExecution->getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Executed move should be added to history";
	EXPECT_EQ(lastMove->startingPosition, pawnMove.start) << "Starting position should match";
	EXPECT_EQ(lastMove->endingPosition, pawnMove.end) << "Ending position should match";
	EXPECT_EQ(lastMove->movedPiece, PieceType::Pawn) << "Moved piece should be pawn";
	EXPECT_EQ(lastMove->number, 1) << "First executed move should have number 1";
}


TEST_F(MoveHistoryTests, ExecuteMultipleMovesWithHistory)
{
	// Execute multiple moves
	PossibleMove move1 = CreateMove({4, 6}, {4, 4}, MoveType::Normal); // e2-e4
	PossibleMove move2 = CreateMove({4, 1}, {4, 3}, MoveType::Normal); // e7-e5

	mExecution->executeMove(move1);
	mExecution->executeMove(move2);

	// Verify both moves are in history with correct numbering
	const Move *lastMove = mExecution->getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Should have last move";
	EXPECT_EQ(lastMove->number, 2) << "Second move should have number 2";
	EXPECT_EQ(lastMove->startingPosition, move2.start) << "Last move should be second move";
}


TEST_F(MoveHistoryTests, ExecuteCaptureMoveSetsCorrectType)
{
	// Set up a capture scenario by placing an opponent piece
	// First move white pawn to e4
	PossibleMove move1 = CreateMove({4, 6}, {4, 4}, MoveType::Normal);
	mExecution->executeMove(move1);

	// Manually place black pawn at d5 for capture
	auto blackPawn = ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black);
	mBoard->setPiece({3, 3}, blackPawn); // d5

	// Execute capture move exd5
	PossibleMove captureMove = CreateMove({4, 4}, {3, 3}, MoveType::Capture);
	mExecution->executeMove(captureMove);

	// Verify capture is recorded in history
	const Move *lastMove = mExecution->getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Capture move should be in history";
	EXPECT_EQ(lastMove->capturedPiece, PieceType::Pawn) << "Should record captured pawn";
	EXPECT_TRUE((lastMove->type & MoveType::Capture) == MoveType::Capture) << "Move type should include capture";
}



} // namespace GameMechanicsTests
