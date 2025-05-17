/*
  ==============================================================================
	Module:			Move Execution Tests
	Description:    Testing the move execution module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveExecution.h"
#include "MoveValidation.h"

#include "TestHelper.h"


TEST(MoveExecutionTest, Constructor)
{
	auto board		= CreateDefaultBoard();
	auto validation = std::make_shared<MoveValidation>(board);

	EXPECT_NO_THROW({ MoveExecution exec(board, validation); });
}


TEST(MoveExecutionTest, ExecuteMoveUpdatesBoard)
{
	auto		  board		 = CreateDefaultBoard();
	auto		  validation = std::make_shared<MoveValidation>(board);
	MoveExecution execution(board, validation);

	// Move white pawn e2 to e4
	Position	  start = {4, 1};
	Position	  end	= {4, 3};
	PossibleMove  move{start, end, MoveType::Normal, PieceType::DefaultType};

	auto		  result = execution.executeMove(move);

	// The pawn should now be at e4
	auto		  piece	 = board->getPiece(end);

	ASSERT_NE(piece, nullptr);						  // Check if there is a piece
	EXPECT_EQ(piece->getType(), PieceType::Pawn);	  // Check for piece type
	EXPECT_EQ(piece->getColor(), PlayerColor::White); // Check for piece color
}


TEST(MoveExecutionTest, AddAndRemoveFromMoveHistory)
{
	auto		  board		 = CreateDefaultBoard();
	auto		  validation = std::make_shared<MoveValidation>(board);
	MoveExecution execution(board, validation);

	// Execute the move
	Position	  start = {4, 1};
	Position	  end	= {4, 3};
	PossibleMove  move{start, end, MoveType::Normal, PieceType::DefaultType};
	Move		  executedMove(move);

	// Check for move added to move history
	execution.addMoveToHistory(executedMove);
	EXPECT_NE(execution.getLastMove(), nullptr);

	// Check for removing the move
	execution.removeLastMove();
	EXPECT_EQ(execution.getLastMove(), nullptr);
}
