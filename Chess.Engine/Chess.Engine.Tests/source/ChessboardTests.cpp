/*
  ==============================================================================
	Module:			Chessboard Tests
	Description:    Testing the chessboard module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "ChessBoard.h"


TEST(ChessboardTest, ConstructAndInitialization)
{
	ChessBoard board;
	EXPECT_NO_THROW(board.initializeBoard());
}


TEST(ChessboardTest, SetAndGetPiece)
{
	ChessBoard board;
	board.initializeBoard();

	Position pos   = {0, 0};
	auto	 piece = ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White);
	board.setPiece(pos, piece);

	auto retrieved = board.getPiece(pos);

	ASSERT_NE(retrieved, nullptr);						  // We should get a valid chess piece
	EXPECT_EQ(retrieved->getType(), PieceType::Rook);	  // Check for the piece type
	EXPECT_EQ(retrieved->getColor(), PlayerColor::White); // Check for the piece color
}


TEST(ChessboardTest, MovePiece)
{
	ChessBoard board;
	board.initializeBoard();

	Position start = {4, 1}; // e2
	Position end   = {4, 3}; // e4

	bool	 moved = board.movePiece(start, end);
	EXPECT_TRUE(moved);

	auto piece = board.getPiece(end);
	ASSERT_NE(piece, nullptr);
	EXPECT_EQ(piece->getType(), PieceType::Pawn);
}


TEST(ChessBoardTest, RemovePiece)
{
	ChessBoard board;
	board.initializeBoard();

	Position pos = {0, 1}; // a2
	board.removePiece(pos);

	EXPECT_TRUE(board.isEmpty(pos));
}


TEST(ChessBoardTest, GetKingsPosition)
{
	ChessBoard board;
	board.initializeBoard();

	Position whiteKing = board.getKingsPosition(PlayerColor::White);
	Position blackKing = board.getKingsPosition(PlayerColor::Black);

	EXPECT_EQ(whiteKing.x, 4);
	EXPECT_EQ(whiteKing.y, 7);
	EXPECT_EQ(blackKing.x, 4);
	EXPECT_EQ(blackKing.y, 0);
}