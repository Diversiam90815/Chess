/*
  ==============================================================================
	Module:			Move Validation Tests
	Description:    Testing the move validation module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveValidation.h"


TEST(MoveValidationTest, ConstructionAndInit)
{
	auto board = std::make_shared<ChessBoard>();
	board->initializeBoard();

	MoveValidation validation;
	validation.init(board);

	// No crash, board is set
	SUCCEED();
}


TEST(MoveValidationTest, ValidateMoveReturnsTrueForLegalMove)
{
	auto board = std::make_shared<ChessBoard>();
	board->initializeBoard();

	MoveValidation validation(board);

	// Move white pawn from e2 to e4
	Position	   start = {4, 1};
	Position	   end	 = {4, 3};
	Move		   move(start, end, PieceType::Pawn);

	EXPECT_TRUE(validation.validateMove(move, PlayerColor::White));
}


TEST(MoveValidationTest, IsKingInCheckReturnsFalseOnInitialBoardForWhite)
{
	auto board = std::make_shared<ChessBoard>();
	board->initializeBoard();

	MoveValidation validation(board);
	PlayerColor	   currentPlayer = PlayerColor::White;

	Position	   kingPos		 = board->getKingsPosition(currentPlayer);

	EXPECT_FALSE(validation.isKingInCheck(kingPos, currentPlayer));
}


TEST(MoveValidationTest, IsKingInCheckReturnsFalseOnInitialBoardForBlack)
{
	auto board = std::make_shared<ChessBoard>();
	board->initializeBoard();

	MoveValidation validation(board);
	PlayerColor	   currentPlayer = PlayerColor::Black;

	Position	   kingPos		 = board->getKingsPosition(currentPlayer);

	EXPECT_FALSE(validation.isKingInCheck(kingPos, currentPlayer));
}



TEST(MoveValidationTest, IsCheckmateReturnsFalseOnInitialBoardForWhite)
{
	auto board = std::make_shared<ChessBoard>();
	board->initializeBoard();

	MoveValidation validation(board);

	EXPECT_FALSE(validation.isCheckmate(PlayerColor::White));
}


TEST(MoveValidationTest, IsCheckmateReturnsFalseOnInitialBoardForBlack)
{
	auto board = std::make_shared<ChessBoard>();
	board->initializeBoard();

	MoveValidation validation(board);

	EXPECT_FALSE(validation.isCheckmate(PlayerColor::Black));
}


TEST(MoveValidationTest, IsStaleMateReturnsFalseOnInitialBoardForBlack)
{
	auto board = std::make_shared<ChessBoard>();
	board->initializeBoard();

	MoveValidation validation(board);

	EXPECT_FALSE(validation.isStalemate(PlayerColor::Black));
}
