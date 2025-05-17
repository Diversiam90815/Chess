/*
  ==============================================================================
	Module:			Move Validation Tests
	Description:    Testing the move validation module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveValidation.h"

// Helper to create a default board
std::shared_ptr<ChessBoard> CreateDefaultBoard()
{
	auto board = std::make_shared<ChessBoard>();
	board->initializeBoard();
	return board;
}


TEST(MoveValidationTest, ConstructionAndInit)
{
	auto		   board = CreateDefaultBoard();
	MoveValidation validation;
	validation.init(board);

	// No crash, board is set
	SUCCEED();
}


TEST(MoveValidationTest, ValidateMoveReturnsTrueForLegalMove)
{
	auto		   board = CreateDefaultBoard();
	MoveValidation validation(board);

	// Move white pawn from e2 to e4
	Position	   start = {4, 1};
	Position	   end	 = {4, 3};
	Move		   move(start, end, PieceType::Pawn);

	EXPECT_TRUE(validation.validateMove(move, PlayerColor::White));
}


TEST(MoveValidationTest, IsKingInCheckReturnsFalseOnInitialBoardForWhite)
{
	auto		   board = CreateDefaultBoard();
	MoveValidation validation(board);
	PlayerColor	   currentPlayer = PlayerColor::White;

	Position	   kingPos		 = board->getKingsPosition(currentPlayer);

	EXPECT_FALSE(validation.isKingInCheck(kingPos, currentPlayer));
}


TEST(MoveValidationTest, IsKingInCheckReturnsFalseOnInitialBoardForBlack)
{
	auto		   board = CreateDefaultBoard();
	MoveValidation validation(board);
	PlayerColor	   currentPlayer = PlayerColor::Black;

	Position	   kingPos		 = board->getKingsPosition(currentPlayer);

	EXPECT_FALSE(validation.isKingInCheck(kingPos, currentPlayer));
}



TEST(MoveValidationTest, IsCheckmateReturnsFalseOnInitialBoardForWhite)
{
	auto		   board = CreateDefaultBoard();
	MoveValidation validation(board);

	EXPECT_FALSE(validation.isCheckmate(PlayerColor::White));
}


TEST(MoveValidationTest, IsCheckmateReturnsFalseOnInitialBoardForBlack)
{
	auto		   board = CreateDefaultBoard();
	MoveValidation validation(board);

	EXPECT_FALSE(validation.isCheckmate(PlayerColor::Black));
}


TEST(MoveValidationTest, IsStaleMateReturnsFalseOnInitialBoardForBlack)
{
	auto		   board = CreateDefaultBoard();
	MoveValidation validation(board);

	EXPECT_FALSE(validation.isStalemate(PlayerColor::Black));
}
