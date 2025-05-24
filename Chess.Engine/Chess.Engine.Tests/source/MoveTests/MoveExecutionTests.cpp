/*
  ==============================================================================
	Module:			Move Execution Tests
	Description:    Testing the general move execution module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveExecution.h"
#include "MoveValidation.h"
#include "TestHelper.h"


class MoveExecutionTest : public ::testing::Test
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
};


TEST_F(MoveExecutionTest, ExecuteMoveUpdatesBoard)
{
	// Move white pawn e2 to e4
	Position	 start = {4, 6};
	Position	 end   = {4, 4};
	PossibleMove move{start, end, MoveType::Normal, PieceType::DefaultType};

	auto		 result = mExecution->executeMove(move);

	// The pawn should now be at e4
	auto		 piece	= mBoard->getPiece(end);

	ASSERT_NE(piece, nullptr) << "Expected a piece at the destination";
	EXPECT_EQ(piece->getType(), PieceType::Pawn) << "Expected a pawn at the destination";
	EXPECT_EQ(piece->getColor(), PlayerColor::White) << "Expected a white piece at the destination";
}


TEST_F(MoveExecutionTest, ExecuteCaptureMove)
{
	mBoard->removeAllPiecesFromBoard();

	// Place white knight at e4, black pawn at f5
	Position whiteKnightPos = {4, 4}; // e4
	Position blackPawnPos	= {5, 3}; // f5

	mBoard->setPiece(whiteKnightPos, ChessPiece::CreatePiece(PieceType::Knight, PlayerColor::White));
	mBoard->setPiece(blackPawnPos, ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black));

	// Execute capture move
	PossibleMove captureMove{whiteKnightPos, blackPawnPos, MoveType::Capture, PieceType::DefaultType};
	auto		 result	   = mExecution->executeMove(captureMove);

	// Verify: White knight now at f5, no piece at e4
	auto		 pieceAtF5 = mBoard->getPiece(blackPawnPos);
	ASSERT_NE(pieceAtF5, nullptr) << "Expected the capturing knight at destination";
	EXPECT_EQ(pieceAtF5->getType(), PieceType::Knight) << "Expected a knight at the destination";
	EXPECT_EQ(pieceAtF5->getColor(), PlayerColor::White) << "Expected a white piece at the destination";
	EXPECT_TRUE(mBoard->isEmpty(whiteKnightPos)) << "Original position should be empty after capture";

	// Verify: capture was recorded in the move
	const Move *lastMove = mExecution->getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Expected a move in the history";
	EXPECT_EQ(lastMove->capturedPiece, PieceType::Pawn) << "Move should record the captured pawn";
}


TEST_F(MoveExecutionTest, AddAndRemoveFromMoveHistory)
{
	// Execute the move
	Position	 start = {4, 1};
	Position	 end   = {4, 3};
	PossibleMove move{start, end, MoveType::Normal, PieceType::DefaultType};
	Move		 executedMove(move);

	// Check for move added to move history
	mExecution->addMoveToHistory(executedMove);
	EXPECT_NE(mExecution->getLastMove(), nullptr) << "Last move should not be null after adding to history";

	// Check for removing the move
	mExecution->removeLastMove();
	EXPECT_EQ(mExecution->getLastMove(), nullptr) << "Last move should be null after removing from history";
}


