/*
  ==============================================================================
	Module:			Chessboard Tests
	Description:    Testing the chessboard module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "ChessBoard.h"


namespace BoardTests
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


TEST_F(ChessBoardTest, GetBoardStateInitialPosition)
{
	// Test getBoardState with initial board setup
	BoardStateArray boardState;
	bool			result = mBoard.getBoardState(boardState);

	EXPECT_TRUE(result) << "getBoardState should return true for successful operation";

	// Test some key positions for initial board setup

	// White pieces (bottom of board)
	int whiteRookEncoded = (static_cast<int>(PlayerColor::White) << 4) | static_cast<int>(PieceType::Rook);
	EXPECT_EQ(boardState[7][0], whiteRookEncoded) << "a1 should contain white rook";
	EXPECT_EQ(boardState[7][7], whiteRookEncoded) << "h1 should contain white rook";

	int whiteKnightEncoded = (static_cast<int>(PlayerColor::White) << 4) | static_cast<int>(PieceType::Knight);
	EXPECT_EQ(boardState[7][1], whiteKnightEncoded) << "b1 should contain white knight";
	EXPECT_EQ(boardState[7][6], whiteKnightEncoded) << "g1 should contain white knight";

	int whiteBishopEncoded = (static_cast<int>(PlayerColor::White) << 4) | static_cast<int>(PieceType::Bishop);
	EXPECT_EQ(boardState[7][2], whiteBishopEncoded) << "c1 should contain white bishop";
	EXPECT_EQ(boardState[7][5], whiteBishopEncoded) << "f1 should contain white bishop";

	int whiteQueenEncoded = (static_cast<int>(PlayerColor::White) << 4) | static_cast<int>(PieceType::Queen);
	EXPECT_EQ(boardState[7][3], whiteQueenEncoded) << "d1 should contain white queen";

	int whiteKingEncoded = (static_cast<int>(PlayerColor::White) << 4) | static_cast<int>(PieceType::King);
	EXPECT_EQ(boardState[7][4], whiteKingEncoded) << "e1 should contain white king";

	// White pawns
	int whitePawnEncoded = (static_cast<int>(PlayerColor::White) << 4) | static_cast<int>(PieceType::Pawn);
	for (int x = 0; x < 8; x++)
	{
		EXPECT_EQ(boardState[6][x], whitePawnEncoded) << "White pawn should be at rank 2 (array index 6), file " << x;
	}

	// Black pieces (top of board)
	int blackRookEncoded = (static_cast<int>(PlayerColor::Black) << 4) | static_cast<int>(PieceType::Rook);
	EXPECT_EQ(boardState[0][0], blackRookEncoded) << "a8 should contain black rook";
	EXPECT_EQ(boardState[0][7], blackRookEncoded) << "h8 should contain black rook";

	int blackKnightEncoded = (static_cast<int>(PlayerColor::Black) << 4) | static_cast<int>(PieceType::Knight);
	EXPECT_EQ(boardState[0][1], blackKnightEncoded) << "b8 should contain black knight";
	EXPECT_EQ(boardState[0][6], blackKnightEncoded) << "g8 should contain black knight";

	int blackBishopEncoded = (static_cast<int>(PlayerColor::Black) << 4) | static_cast<int>(PieceType::Bishop);
	EXPECT_EQ(boardState[0][2], blackBishopEncoded) << "c8 should contain black bishop";
	EXPECT_EQ(boardState[0][5], blackBishopEncoded) << "f8 should contain black bishop";

	int blackQueenEncoded = (static_cast<int>(PlayerColor::Black) << 4) | static_cast<int>(PieceType::Queen);
	EXPECT_EQ(boardState[0][3], blackQueenEncoded) << "d8 should contain black queen";

	int blackKingEncoded = (static_cast<int>(PlayerColor::Black) << 4) | static_cast<int>(PieceType::King);
	EXPECT_EQ(boardState[0][4], blackKingEncoded) << "e8 should contain black king";

	// Black pawns
	int blackPawnEncoded = (static_cast<int>(PlayerColor::Black) << 4) | static_cast<int>(PieceType::Pawn);
	for (int x = 0; x < 8; x++)
	{
		EXPECT_EQ(boardState[1][x], blackPawnEncoded) << "Black pawn should be at rank 7 (array index 1), file " << x;
	}

	// Empty squares in the middle
	for (int y = 2; y < 6; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			EXPECT_EQ(boardState[y][x], 0) << "Middle squares should be empty at (" << x << "," << y << ")";
		}
	}
}


TEST_F(ChessBoardTest, GetBoardStateEmptyBoard)
{
	// Remove all pieces and test getBoardState
	mBoard.removeAllPiecesFromBoard();

	BoardStateArray boardState;
	bool			result = mBoard.getBoardState(boardState);

	EXPECT_TRUE(result) << "getBoardState should return true for empty board";

	// All squares should be empty (value 0)
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			EXPECT_EQ(boardState[y][x], 0) << "Empty board square should be 0 at (" << x << "," << y << ")";
		}
	}
}


TEST_F(ChessBoardTest, GetBoardStateAfterMoves)
{
	// Make some moves and verify the board state
	Position pawnStart = {4, 6}; // e2
	Position pawnEnd   = {4, 4}; // e4
	mBoard.movePiece(pawnStart, pawnEnd);

	BoardStateArray boardState;
	bool			result = mBoard.getBoardState(boardState);

	EXPECT_TRUE(result) << "getBoardState should return true after moves";

	// Original position should be empty
	EXPECT_EQ(boardState[6][4], 0) << "e2 should be empty after pawn move";

	// New position should contain the pawn
	int whitePawnEncoded = (static_cast<int>(PlayerColor::White) << 4) | static_cast<int>(PieceType::Pawn);
	EXPECT_EQ(boardState[4][4], whitePawnEncoded) << "e4 should contain white pawn after move";
}


TEST_F(ChessBoardTest, GetBoardStateAfterPieceRemoval)
{
	// Remove a specific piece and verify the board state
	Position rookPos = {0, 0}; // a8
	mBoard.removePiece(rookPos);

	BoardStateArray boardState;
	bool			result = mBoard.getBoardState(boardState);

	EXPECT_TRUE(result) << "getBoardState should return true after piece removal";

	// Removed position should be empty
	EXPECT_EQ(boardState[0][0], 0) << "a8 should be empty after rook removal";

	// Other pieces should remain unchanged
	int blackKnightEncoded = (static_cast<int>(PlayerColor::Black) << 4) | static_cast<int>(PieceType::Knight);
	EXPECT_EQ(boardState[0][1], blackKnightEncoded) << "b8 should still contain black knight";
}


TEST_F(ChessBoardTest, GetBoardStateAfterPieceAddition)
{
	// Add a piece to an empty square and verify the board state
	Position emptyPos = {4, 4}; // e4
	auto	 piece	  = ChessPiece::CreatePiece(PieceType::Queen, PlayerColor::White);
	mBoard.setPiece(emptyPos, piece);

	BoardStateArray boardState;
	bool			result = mBoard.getBoardState(boardState);

	EXPECT_TRUE(result) << "getBoardState should return true after piece addition";

	// Added position should contain the piece
	int whiteQueenEncoded = (static_cast<int>(PlayerColor::White) << 4) | static_cast<int>(PieceType::Queen);
	EXPECT_EQ(boardState[4][4], whiteQueenEncoded) << "e4 should contain white queen after addition";
}


TEST_F(ChessBoardTest, GetBoardStateEncoding)
{
	// Test the encoding format: color in high nibble, piece type in low nibble
	mBoard.removeAllPiecesFromBoard();

	// Add a specific piece and test its encoding
	Position testPos = {3, 3}; // d4
	auto	 piece	 = ChessPiece::CreatePiece(PieceType::Bishop, PlayerColor::Black);
	mBoard.setPiece(testPos, piece);

	BoardStateArray boardState;
	mBoard.getBoardState(boardState);

	int encoded	 = boardState[3][3];
	int colorVal = (encoded >> 4) & 0xF;
	int typeVal	 = encoded & 0xF;

	EXPECT_EQ(colorVal, static_cast<int>(PlayerColor::Black)) << "Color should be correctly encoded in high nibble";
	EXPECT_EQ(typeVal, static_cast<int>(PieceType::Bishop)) << "Piece type should be correctly encoded in low nibble";
}


} // namespace BoardTests
