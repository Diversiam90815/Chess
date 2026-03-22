/*
  ==============================================================================
	Module:			Chessboard Tests
	Description:    Testing the chessboard module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Chessboard.h"


namespace BoardTests
{

class ChessboardTest : public ::testing::Test
{
protected:
	void	   SetUp() override { mBoard.init(); }

	Chessboard mBoard;
};


TEST_F(ChessboardTest, ConstructAndInitialization)
{
	Chessboard board;
	EXPECT_NO_THROW(board.init());
}


TEST_F(ChessboardTest, InitialSideIsWhite)
{
	EXPECT_EQ(mBoard.getCurrentSide(), Side::White) << "White should move first";
}


TEST_F(ChessboardTest, InitialCastlingRightsAllAvailable)
{
	Castling rights = mBoard.getCurrentCastlingRights();

	EXPECT_TRUE(static_cast<bool>(rights & Castling::WK)) << "White kingside castling should be available";
	EXPECT_TRUE(static_cast<bool>(rights & Castling::WQ)) << "White queenside castling should be available";
	EXPECT_TRUE(static_cast<bool>(rights & Castling::BK)) << "Black kingside castling should be available";
	EXPECT_TRUE(static_cast<bool>(rights & Castling::BQ)) << "Black queenside castling should be available";
}


TEST_F(ChessboardTest, InitialEnPassantSquareIsNone)
{
	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::None) << "No en passant square on initial board";
}


TEST_F(ChessboardTest, InitialHalfMoveClockIsZero)
{
	EXPECT_EQ(mBoard.getHalfMoveClock(), 0) << "Half move clock should be 0 on initial board";
}


TEST_F(ChessboardTest, PieceAtReturnsCorrectPiece)
{
	// White pieces on back rank
	EXPECT_EQ(mBoard.pieceAt(Square::a1), PieceType::WRook) << "a1 should have white rook";
	EXPECT_EQ(mBoard.pieceAt(Square::b1), PieceType::WKnight) << "b1 should have white knight";
	EXPECT_EQ(mBoard.pieceAt(Square::c1), PieceType::WBishop) << "c1 should have white bishop";
	EXPECT_EQ(mBoard.pieceAt(Square::d1), PieceType::WQueen) << "d1 should have white queen";
	EXPECT_EQ(mBoard.pieceAt(Square::e1), PieceType::WKing) << "e1 should have white king";
	EXPECT_EQ(mBoard.pieceAt(Square::f1), PieceType::WBishop) << "f1 should have white bishop";
	EXPECT_EQ(mBoard.pieceAt(Square::g1), PieceType::WKnight) << "g1 should have white knight";
	EXPECT_EQ(mBoard.pieceAt(Square::h1), PieceType::WRook) << "h1 should have white rook";

	// White pawns on rank 2
	EXPECT_EQ(mBoard.pieceAt(Square::a2), PieceType::WPawn) << "a2 should have white pawn";
	EXPECT_EQ(mBoard.pieceAt(Square::e2), PieceType::WPawn) << "e2 should have white pawn";
	EXPECT_EQ(mBoard.pieceAt(Square::h2), PieceType::WPawn) << "h2 should have white pawn";

	// Black pieces on back rank
	EXPECT_EQ(mBoard.pieceAt(Square::a8), PieceType::BRook) << "a8 should have black rook";
	EXPECT_EQ(mBoard.pieceAt(Square::b8), PieceType::BKnight) << "b8 should have black knight";
	EXPECT_EQ(mBoard.pieceAt(Square::c8), PieceType::BBishop) << "c8 should have black bishop";
	EXPECT_EQ(mBoard.pieceAt(Square::d8), PieceType::BQueen) << "d8 should have black queen";
	EXPECT_EQ(mBoard.pieceAt(Square::e8), PieceType::BKing) << "e8 should have black king";
	EXPECT_EQ(mBoard.pieceAt(Square::f8), PieceType::BBishop) << "f8 should have black bishop";
	EXPECT_EQ(mBoard.pieceAt(Square::g8), PieceType::BKnight) << "g8 should have black knight";
	EXPECT_EQ(mBoard.pieceAt(Square::h8), PieceType::BRook) << "h8 should have black rook";

	// Black pawns on rank 7
	EXPECT_EQ(mBoard.pieceAt(Square::a7), PieceType::BPawn) << "a7 should have black pawn";
	EXPECT_EQ(mBoard.pieceAt(Square::e7), PieceType::BPawn) << "e7 should have black pawn";
	EXPECT_EQ(mBoard.pieceAt(Square::h7), PieceType::BPawn) << "h7 should have black pawn";
}


TEST_F(ChessboardTest, PieceAtReturnsNoneForEmptySquares)
{
	// Middle of the board should be empty
	EXPECT_EQ(mBoard.pieceAt(Square::e4), PieceType::None) << "e4 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::d5), PieceType::None) << "d5 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::a3), PieceType::None) << "a3 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::h6), PieceType::None) << "h6 should be empty";
}


TEST_F(ChessboardTest, AddPieceUpdatesBoard)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WQueen, Square::e4);
	mBoard.updateOccupancies();

	EXPECT_EQ(mBoard.pieceAt(Square::e4), PieceType::WQueen) << "e4 should have white queen after adding";
}


TEST_F(ChessboardTest, RemovePieceUpdatesBoard)
{
	// Remove white pawn from e2
	mBoard.removePiece(PieceType::WPawn, Square::e2);
	mBoard.updateOccupancies();

	EXPECT_EQ(mBoard.pieceAt(Square::e2), PieceType::None) << "e2 should be empty after removal";
}


TEST_F(ChessboardTest, MovePieceUpdatesBoard)
{
	// Move white pawn e2 to e4
	mBoard.movePiece(PieceType::WPawn, Square::e2, Square::e4);
	mBoard.updateOccupancies();

	EXPECT_EQ(mBoard.pieceAt(Square::e2), PieceType::None) << "e2 should be empty after move";
	EXPECT_EQ(mBoard.pieceAt(Square::e4), PieceType::WPawn) << "e4 should have white pawn after move";
}


TEST_F(ChessboardTest, ClearRemovesAllPieces)
{
	mBoard.clear();

	// Check that all squares are empty
	for (int sq = 0; sq < 64; ++sq)
	{
		Square square = static_cast<Square>(sq);
		EXPECT_EQ(mBoard.pieceAt(square), PieceType::None) << "Square " << sq << " should be empty after clear";
	}
}


TEST_F(ChessboardTest, FlipSideChangesSide)
{
	EXPECT_EQ(mBoard.getCurrentSide(), Side::White) << "Initial side should be white";

	mBoard.flipSide();
	EXPECT_EQ(mBoard.getCurrentSide(), Side::Black) << "Side should be black after flip";

	mBoard.flipSide();
	EXPECT_EQ(mBoard.getCurrentSide(), Side::White) << "Side should be white after second flip";
}


TEST_F(ChessboardTest, SetSideUpdatesCorrectly)
{
	mBoard.setSide(Side::Black);
	EXPECT_EQ(mBoard.getCurrentSide(), Side::Black) << "Side should be black after setSide";

	mBoard.setSide(Side::White);
	EXPECT_EQ(mBoard.getCurrentSide(), Side::White) << "Side should be white after setSide";
}


TEST_F(ChessboardTest, SetCastlingRightsUpdatesCorrectly)
{
	mBoard.setCastlingRights(Castling::WK);

	Castling rights = mBoard.getCurrentCastlingRights();
	EXPECT_TRUE(static_cast<bool>(rights & Castling::WK)) << "White kingside should be set";
	EXPECT_FALSE(static_cast<bool>(rights & Castling::WQ)) << "White queenside should not be set";
	EXPECT_FALSE(static_cast<bool>(rights & Castling::BK)) << "Black kingside should not be set";
	EXPECT_FALSE(static_cast<bool>(rights & Castling::BQ)) << "Black queenside should not be set";
}


TEST_F(ChessboardTest, SetEnPassantSquareUpdatesCorrectly)
{
	mBoard.setEnPassantSquare(Square::e3);

	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::e3) << "En passant square should be e3";
}


TEST_F(ChessboardTest, SaveAndRestoreState)
{
	// Modify the board state
	mBoard.flipSide();
	mBoard.setEnPassantSquare(Square::d6);
	mBoard.setCastlingRights(Castling::WK | Castling::BQ);
	mBoard.setHalfMoveClock(15);

	// Save state
	BoardState state = mBoard.saveState();

	// Modify further
	mBoard.flipSide();
	mBoard.setEnPassantSquare(Square::None);
	mBoard.setCastlingRights(Castling::None);
	mBoard.setHalfMoveClock(0);

	// Restore state
	mBoard.restoreState(state);

	// Verify restoration
	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::d6) << "En passant should be restored to d6";
	EXPECT_EQ(mBoard.getCurrentCastlingRights(), Castling::WK | Castling::BQ) << "Castling rights should be restored";
	EXPECT_EQ(mBoard.getHalfMoveClock(), 15) << "Half move clock should be restored to 15";
}


TEST_F(ChessboardTest, ParseFENStartPosition)
{
	Chessboard board;
	board.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	// Verify pieces
	EXPECT_EQ(board.pieceAt(Square::e1), PieceType::WKing) << "e1 should have white king";
	EXPECT_EQ(board.pieceAt(Square::e8), PieceType::BKing) << "e8 should have black king";
	EXPECT_EQ(board.pieceAt(Square::e2), PieceType::WPawn) << "e2 should have white pawn";
	EXPECT_EQ(board.pieceAt(Square::e7), PieceType::BPawn) << "e7 should have black pawn";

	// Verify side
	EXPECT_EQ(board.getCurrentSide(), Side::White) << "White should move first";

	// Verify castling
	Castling rights = board.getCurrentCastlingRights();
	EXPECT_TRUE(static_cast<bool>(rights & Castling::WK)) << "White kingside castling should be available";
	EXPECT_TRUE(static_cast<bool>(rights & Castling::WQ)) << "White queenside castling should be available";
	EXPECT_TRUE(static_cast<bool>(rights & Castling::BK)) << "Black kingside castling should be available";
	EXPECT_TRUE(static_cast<bool>(rights & Castling::BQ)) << "Black queenside castling should be available";
}


TEST_F(ChessboardTest, ParseFENWithEnPassant)
{
	Chessboard board;
	board.parseFEN("rnbqkbnr/pppp1ppp/8/4pP2/8/8/PPPPP1PP/RNBQKBNR w KQkq e6 0 3");

	EXPECT_EQ(board.getCurrentEnPassantSqaure(), Square::e6) << "En passant square should be e6";
}


TEST_F(ChessboardTest, ParseFENWithLimitedCastling)
{
	Chessboard board;
	board.parseFEN("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w Kq - 0 1");

	Castling rights = board.getCurrentCastlingRights();
	EXPECT_TRUE(static_cast<bool>(rights & Castling::WK)) << "White kingside castling should be available";
	EXPECT_FALSE(static_cast<bool>(rights & Castling::WQ)) << "White queenside castling should not be available";
	EXPECT_FALSE(static_cast<bool>(rights & Castling::BK)) << "Black kingside castling should not be available";
	EXPECT_TRUE(static_cast<bool>(rights & Castling::BQ)) << "Black queenside castling should be available";
}


TEST_F(ChessboardTest, ParseFENBlackToMove)
{
	Chessboard board;
	board.parseFEN("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");

	EXPECT_EQ(board.getCurrentSide(), Side::Black) << "Black should move";
}


TEST_F(ChessboardTest, ZobristHashIsComputed)
{
	mBoard.computeHash();

	EXPECT_NE(mBoard.getHash(), 0ULL) << "Zobrist hash should not be zero for initial position";
}


TEST_F(ChessboardTest, ZobristHashChangesAfterMove)
{
	mBoard.computeHash();
	uint64_t initialHash = mBoard.getHash();

	// Make a move
	mBoard.movePiece(PieceType::WPawn, Square::e2, Square::e4);
	mBoard.updateOccupancies();
	mBoard.flipSide();
	mBoard.computeHash();

	EXPECT_NE(mBoard.getHash(), initialHash) << "Hash should change after a move";
}


TEST_F(ChessboardTest, OccupanciesAreCorrect)
{
	const auto &occ		 = mBoard.occ();

	// White occupancy should have 16 bits set (16 pieces)
	U64			whiteOcc = occ[static_cast<int>(Side::White)];
	EXPECT_EQ(BitUtils::popCount(whiteOcc), 16) << "White should have 16 pieces";

	// Black occupancy should have 16 bits set
	U64 blackOcc = occ[static_cast<int>(Side::Black)];
	EXPECT_EQ(BitUtils::popCount(blackOcc), 16) << "Black should have 16 pieces";

	// Both occupancy should have 32 bits set
	U64 bothOcc = occ[static_cast<int>(Side::Both)];
	EXPECT_EQ(BitUtils::popCount(bothOcc), 32) << "Total should be 32 pieces";
}


TEST_F(ChessboardTest, OccupanciesUpdateAfterRemoval)
{
	mBoard.removePiece(PieceType::WPawn, Square::e2);
	mBoard.updateOccupancies();

	const auto &occ		 = mBoard.occ();
	U64			whiteOcc = occ[static_cast<int>(Side::White)];

	EXPECT_EQ(BitUtils::popCount(whiteOcc), 15) << "White should have 15 pieces after removal";
}

} // namespace BoardTests
