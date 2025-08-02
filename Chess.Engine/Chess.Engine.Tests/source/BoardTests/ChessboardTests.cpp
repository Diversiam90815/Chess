/*
  ==============================================================================
	Module:			Chessboard Tests
	Description:    Testing the chessboard module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "ChessBoard.h"


namespace GameMechanicTests
{


class ChessBoardTest : public ::testing::Test
{
protected:
	void	   SetUp() override { mBoard.initializeBoard(); }

	ChessBoard mBoard;
};


TEST_F(ChessBoardTest, ConstructAndInitialization)
{
	ChessBoard board;
	EXPECT_NO_THROW(board.initializeBoard());
}


TEST_F(ChessBoardTest, CopyConstructor)
{
	// Set up the original board with some modifications
	Position pawnPos = {4, 6}; // e2
	mBoard.removePiece(pawnPos);

	// Create a copy
	ChessBoard copyBoard(mBoard);

	// Verify the copy has the same state
	EXPECT_TRUE(copyBoard.isEmpty(pawnPos)) << "Copy should reflect the removed piece";

	// Modify the copy and check that the original is unchanged
	Position rookPos = {0, 7}; // a1
	copyBoard.removePiece(rookPos);

	EXPECT_TRUE(copyBoard.isEmpty(rookPos)) << "Piece should be removed from copy";
	EXPECT_FALSE(mBoard.isEmpty(rookPos)) << "Original board should be unchanged";
}


TEST_F(ChessBoardTest, SetAndGetPiece)
{
	Position pos   = {0, 0};
	auto	 piece = ChessPiece::CreatePiece(PieceType::Rook, PlayerColor::White);
	mBoard.setPiece(pos, piece);

	auto retrieved = mBoard.getPiece(pos);

	ASSERT_NE(retrieved, nullptr) << "Should get a valid chess piece";
	EXPECT_EQ(retrieved->getType(), PieceType::Rook) << "Retrieved piece should be a Rook";
	EXPECT_EQ(retrieved->getColor(), PlayerColor::White) << "Retrieved piece should be White";
}


TEST_F(ChessBoardTest, MovePiece)
{
	Position start = {4, 1}; // e2
	Position end   = {4, 3}; // e4

	bool	 moved = mBoard.movePiece(start, end);
	EXPECT_TRUE(moved);

	auto piece = mBoard.getPiece(end);
	ASSERT_NE(piece, nullptr);
	EXPECT_EQ(piece->getType(), PieceType::Pawn);
}


TEST_F(ChessBoardTest, MovePieceInvalidPositions)
{
	// Try moving from an empty square
	Position emptyPos = {3, 3}; // d5
	Position validPos = {3, 4}; // d4
	EXPECT_FALSE(mBoard.movePiece(emptyPos, validPos)) << "Moving from empty position should fail";

	// Try moving to an out-of-bounds position
	Position pawnPos		= {4, 6};  // e2
	Position outOfBoundsPos = {-1, 3}; // out of bounds
	EXPECT_FALSE(mBoard.movePiece(pawnPos, outOfBoundsPos)) << "Moving to out of bounds should fail";
}


TEST_F(ChessBoardTest, MovePieceCaptures)
{
	// Move a white pawn to a position where we'll place a black piece
	Position whitePawnPos = {4, 6}; // e2
	Position targetPos	  = {4, 4}; // e4
	mBoard.movePiece(whitePawnPos, targetPos);

	// Place a black pawn at a position to be captured
	Position blackPawnPos = {3, 4}; // d4
	auto	 blackPawn	  = ChessPiece::CreatePiece(PieceType::Pawn, PlayerColor::Black);
	mBoard.setPiece(blackPawnPos, blackPawn);

	// Execute the capture
	bool captured = mBoard.movePiece(targetPos, blackPawnPos);

	EXPECT_TRUE(captured) << "Capture move should succeed";
	auto capturer = mBoard.getPiece(blackPawnPos);
	ASSERT_NE(capturer, nullptr) << "Capturing piece should be at target position";
	EXPECT_EQ(capturer->getType(), PieceType::Pawn) << "Capturing piece should be a pawn";
	EXPECT_EQ(capturer->getColor(), PlayerColor::White) << "Capturing piece should be white";
}


TEST_F(ChessBoardTest, RemovePiece)
{
	Position pos = {0, 1}; // a2
	mBoard.removePiece(pos);

	EXPECT_TRUE(mBoard.isEmpty(pos));
}


TEST_F(ChessBoardTest, RemovePieceNonexistent)
{
	// Try removing a piece from an already empty position
	Position emptyPos = {4, 4}; // e4
	EXPECT_TRUE(mBoard.isEmpty(emptyPos)) << "Position should be empty initially";

	// Should not throw or cause issues
	EXPECT_NO_THROW(mBoard.removePiece(emptyPos));
	EXPECT_TRUE(mBoard.isEmpty(emptyPos)) << "Position should still be empty after removing from empty spot";
}


TEST_F(ChessBoardTest, IsEmpty)
{
	Position occupied = {0, 0}; // a8
	Position empty	  = {4, 4}; // e4

	EXPECT_FALSE(mBoard.isEmpty(occupied)) << "Position with piece should not be empty";
	EXPECT_TRUE(mBoard.isEmpty(empty)) << "Position without piece should be empty";
}


TEST_F(ChessBoardTest, GetKingsPosition)
{
	Position whiteKing = mBoard.getKingsPosition(PlayerColor::White);
	Position blackKing = mBoard.getKingsPosition(PlayerColor::Black);

	EXPECT_EQ(whiteKing.x, 4) << "White king's X position should be correct";
	EXPECT_EQ(whiteKing.y, 7) << "White king's Y position should be correct";
	EXPECT_EQ(blackKing.x, 4) << "Black king's X position should be correct";
	EXPECT_EQ(blackKing.y, 0) << "Black king's Y position should be correct";
}



TEST_F(ChessBoardTest, UpdateKingsPosition)
{
	Position newWhiteKingPos = {4, 5}; // e3

	// Move the king
	mBoard.updateKingsPosition(newWhiteKingPos, PlayerColor::White);

	Position updatedPos = mBoard.getKingsPosition(PlayerColor::White);
	EXPECT_EQ(updatedPos.x, newWhiteKingPos.x) << "King's X position should be updated";
	EXPECT_EQ(updatedPos.y, newWhiteKingPos.y) << "King's Y position should be updated";
}


TEST_F(ChessBoardTest, GetPiecesFromPlayer)
{
	// Get all white pieces
	auto whitePieces = mBoard.getPiecesFromPlayer(PlayerColor::White);

	// A new board should have 16 white pieces
	EXPECT_EQ(whitePieces.size(), 16) << "Should have 16 white pieces on initial board";

	// Verify we got the correct piece types
	int pawnCount	= 0;
	int rookCount	= 0;
	int knightCount = 0;
	int bishopCount = 0;
	int queenCount	= 0;
	int kingCount	= 0;

	for (const auto &playerPiece : whitePieces)
	{
		auto piece = playerPiece.second;
		switch (piece->getType())
		{
		case PieceType::Pawn: pawnCount++; break;
		case PieceType::Rook: rookCount++; break;
		case PieceType::Knight: knightCount++; break;
		case PieceType::Bishop: bishopCount++; break;
		case PieceType::Queen: queenCount++; break;
		case PieceType::King: kingCount++; break;
		default: break;
		}
	}

	EXPECT_EQ(pawnCount, 8) << "Should have 8 white pawns";
	EXPECT_EQ(rookCount, 2) << "Should have 2 white rooks";
	EXPECT_EQ(knightCount, 2) << "Should have 2 white knights";
	EXPECT_EQ(bishopCount, 2) << "Should have 2 white bishops";
	EXPECT_EQ(queenCount, 1) << "Should have 1 white queen";
	EXPECT_EQ(kingCount, 1) << "Should have 1 white king";
}


TEST_F(ChessBoardTest, GetPiecesAfterRemoval)
{
	// Remove a white pawn
	mBoard.removePiece(Position{0, 6}); // a2

	// Get all white pieces
	auto whitePieces = mBoard.getPiecesFromPlayer(PlayerColor::White);

	// Should now have 15 white pieces
	EXPECT_EQ(whitePieces.size(), 15) << "Should have 15 white pieces after removal";
}


TEST_F(ChessBoardTest, GetSquare)
{
	// Get a square and check its properties
	Position pos	= {0, 0}; // a8
	Square	&square = mBoard.getSquare(pos);

	// On a new board, a8 should have a black rook
	auto	 piece	= square.piece;
	ASSERT_NE(piece, nullptr) << "Square should have a piece";
	EXPECT_EQ(piece->getType(), PieceType::Rook) << "Piece should be a rook";
	EXPECT_EQ(piece->getColor(), PlayerColor::Black) << "Piece should be black";
}


TEST_F(ChessBoardTest, RemoveAllPieces)
{
	// Remove all pieces
	mBoard.removeAllPiecesFromBoard();

	// Check that all squares are empty
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			Position pos{x, y};
			EXPECT_TRUE(mBoard.isEmpty(pos)) << "Position (" << x << "," << y << ") should be empty";
		}
	}

	// Check that no pieces are returned for either player
	auto whitePieces = mBoard.getPiecesFromPlayer(PlayerColor::White);
	auto blackPieces = mBoard.getPiecesFromPlayer(PlayerColor::Black);

	EXPECT_EQ(whitePieces.size(), 0) << "No white pieces should remain";
	EXPECT_EQ(blackPieces.size(), 0) << "No black pieces should remain";
}


TEST_F(ChessBoardTest, InitialBoardSetup)
{
	// Test that all the expected pieces are in their correct initial positions

	// Back row pieces for White
	EXPECT_EQ(mBoard.getPiece(Position{0, 7})->getType(), PieceType::Rook) << "a1 should have a rook";
	EXPECT_EQ(mBoard.getPiece(Position{1, 7})->getType(), PieceType::Knight) << "b1 should have a knight";
	EXPECT_EQ(mBoard.getPiece(Position{2, 7})->getType(), PieceType::Bishop) << "c1 should have a bishop";
	EXPECT_EQ(mBoard.getPiece(Position{3, 7})->getType(), PieceType::Queen) << "d1 should have a queen";
	EXPECT_EQ(mBoard.getPiece(Position{4, 7})->getType(), PieceType::King) << "e1 should have a king";
	EXPECT_EQ(mBoard.getPiece(Position{5, 7})->getType(), PieceType::Bishop) << "f1 should have a bishop";
	EXPECT_EQ(mBoard.getPiece(Position{6, 7})->getType(), PieceType::Knight) << "g1 should have a knight";
	EXPECT_EQ(mBoard.getPiece(Position{7, 7})->getType(), PieceType::Rook) << "h1 should have a rook";

	// Pawns for White
	for (int x = 0; x < 8; x++)
	{
		EXPECT_EQ(mBoard.getPiece(Position{x, 6})->getType(), PieceType::Pawn) << "White pawn should be at position (" << x << ",6)";
		EXPECT_EQ(mBoard.getPiece(Position{x, 6})->getColor(), PlayerColor::White) << "Pawn at position (" << x << ",6) should be white";
	}

	// Back row pieces for Black
	EXPECT_EQ(mBoard.getPiece(Position{0, 0})->getType(), PieceType::Rook) << "a8 should have a rook";
	EXPECT_EQ(mBoard.getPiece(Position{1, 0})->getType(), PieceType::Knight) << "b8 should have a knight";
	EXPECT_EQ(mBoard.getPiece(Position{2, 0})->getType(), PieceType::Bishop) << "c8 should have a bishop";
	EXPECT_EQ(mBoard.getPiece(Position{3, 0})->getType(), PieceType::Queen) << "d8 should have a queen";
	EXPECT_EQ(mBoard.getPiece(Position{4, 0})->getType(), PieceType::King) << "e8 should have a king";
	EXPECT_EQ(mBoard.getPiece(Position{5, 0})->getType(), PieceType::Bishop) << "f8 should have a bishop";
	EXPECT_EQ(mBoard.getPiece(Position{6, 0})->getType(), PieceType::Knight) << "g8 should have a knight";
	EXPECT_EQ(mBoard.getPiece(Position{7, 0})->getType(), PieceType::Rook) << "h8 should have a rook";

	// Pawns for Black
	for (int x = 0; x < 8; x++)
	{
		EXPECT_EQ(mBoard.getPiece(Position{x, 1})->getType(), PieceType::Pawn) << "Black pawn should be at position (" << x << ",1)";
		EXPECT_EQ(mBoard.getPiece(Position{x, 1})->getColor(), PlayerColor::Black) << "Pawn at position (" << x << ",1) should be black";
	}

	// Middle of the board should be empty
	for (int x = 0; x < 8; x++)
	{
		for (int y = 2; y < 6; y++)
		{
			EXPECT_TRUE(mBoard.isEmpty(Position{x, y})) << "Middle position (" << x << "," << y << ") should be empty";
		}
	}
}


TEST_F(ChessBoardTest, GetPieceAtPosition)
{
	// Test the getPieceAtPosition method for a few positions
	auto piece1 = mBoard.getPiece(Position{0, 0});
	ASSERT_NE(piece1, nullptr) << "Should get a valid piece at a8";
	EXPECT_EQ(piece1->getType(), PieceType::Rook) << "Should be a rook at a8";
	EXPECT_EQ(piece1->getColor(), PlayerColor::Black) << "Should be black at a8";

	auto piece2 = mBoard.getPiece(Position{4, 4});
	EXPECT_EQ(piece2, nullptr) << "Should get nullptr for empty position e4";

	// Test with out-of-bounds position
	auto piece3 = mBoard.getPiece(Position{8, 8});
	EXPECT_EQ(piece3, nullptr) << "Should get nullptr for out-of-bounds position";
}

} // namespace GameMechanicTests
