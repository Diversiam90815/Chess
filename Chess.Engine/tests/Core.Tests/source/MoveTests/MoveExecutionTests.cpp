/*
  ==============================================================================
	Module:			Move Execution Tests
	Description:    Testing the general move execution module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Execution/MoveExecution.h"
#include "Validation/MoveValidation.h"


namespace MoveTests
{

class MoveExecutionTest : public ::testing::Test
{
protected:
	Chessboard	  mBoard;
	MoveExecution mExecution{mBoard};

	void		  SetUp() override { mBoard.init(); }
};


TEST_F(MoveExecutionTest, ExecuteMoveUpdatesBoard)
{
	// Move white pawn e2 to e4
	Move move(Square::e2, Square::e4, MoveFlag::DoublePawnPush);

	bool result = mExecution.makeMove(move);

	EXPECT_TRUE(result) << "Move should succeed";
	EXPECT_EQ(mBoard.pieceAt(Square::e2), PieceType::None) << "e2 should be empty after move";
	EXPECT_EQ(mBoard.pieceAt(Square::e4), PieceType::WPawn) << "e4 should have white pawn after move";
}


TEST_F(MoveExecutionTest, ExecuteMoveSwitchesSide)
{
	EXPECT_EQ(mBoard.getCurrentSide(), Side::White) << "Initial side should be white";

	Move move(Square::e2, Square::e4, MoveFlag::DoublePawnPush);
	mExecution.makeMove(move);

	EXPECT_EQ(mBoard.getCurrentSide(), Side::Black) << "Side should be black after white's move";
}


TEST_F(MoveExecutionTest, ExecuteCaptureMove)
{
	mBoard.clear();

	// Place white knight at e4, black pawn at f5
	mBoard.addPiece(PieceType::WKnight, Square::e4);
	mBoard.addPiece(PieceType::BPawn, Square::f5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	// Execute capture move
	Move captureMove(Square::e4, Square::f5, MoveFlag::Capture);
	bool result = mExecution.makeMove(captureMove);

	EXPECT_TRUE(result) << "Capture move should succeed";
	EXPECT_EQ(mBoard.pieceAt(Square::f5), PieceType::WKnight) << "f5 should have white knight after capture";
	EXPECT_EQ(mBoard.pieceAt(Square::e4), PieceType::None) << "e4 should be empty after move";
}


TEST_F(MoveExecutionTest, MoveHistoryIsRecorded)
{
	Move move(Square::e2, Square::e4, MoveFlag::DoublePawnPush);
	mExecution.makeMove(move);

	EXPECT_EQ(mExecution.historySize(), 1) << "History should have one entry";

	const MoveHistoryEntry *lastMove = mExecution.getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Last move should not be null";
	EXPECT_EQ(lastMove->move.from(), Square::e2) << "Last move should be from e2";
	EXPECT_EQ(lastMove->move.to(), Square::e4) << "Last move should be to e4";
}


TEST_F(MoveExecutionTest, UnmakeMoveRestoresPosition)
{
	// Make a move
	Move move(Square::e2, Square::e4, MoveFlag::DoublePawnPush);
	mExecution.makeMove(move);

	// Unmake the move
	bool result = mExecution.unmakeMove();

	EXPECT_TRUE(result) << "Unmake should succeed";
	EXPECT_EQ(mBoard.pieceAt(Square::e2), PieceType::WPawn) << "e2 should have pawn after unmake";
	EXPECT_EQ(mBoard.pieceAt(Square::e4), PieceType::None) << "e4 should be empty after unmake";
	EXPECT_EQ(mBoard.getCurrentSide(), Side::White) << "Side should be white after unmake";
}


TEST_F(MoveExecutionTest, UnmakeCaptureRestoresCapturedPiece)
{
	mBoard.clear();

	// Place white knight at e4, black pawn at f5
	mBoard.addPiece(PieceType::WKnight, Square::e4);
	mBoard.addPiece(PieceType::BPawn, Square::f5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	// Execute and unmake capture
	Move captureMove(Square::e4, Square::f5, MoveFlag::Capture);
	mExecution.makeMove(captureMove);
	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.pieceAt(Square::e4), PieceType::WKnight) << "Knight should be back at e4";
	EXPECT_EQ(mBoard.pieceAt(Square::f5), PieceType::BPawn) << "Black pawn should be restored at f5";
}


TEST_F(MoveExecutionTest, UnmakeWithEmptyHistoryReturnsFalse)
{
	bool result = mExecution.unmakeMove();

	EXPECT_FALSE(result) << "Unmake with empty history should return false";
}


TEST_F(MoveExecutionTest, ClearHistoryRemovesAllEntries)
{
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::e7, Square::e5, MoveFlag::DoublePawnPush));

	mExecution.clearHistory();

	EXPECT_EQ(mExecution.historySize(), 0) << "History should be empty after clear";
	EXPECT_EQ(mExecution.getLastMove(), nullptr) << "Last move should be null after clear";
}


TEST_F(MoveExecutionTest, DoublePawnPushSetsEnPassantSquare)
{
	Move move(Square::e2, Square::e4, MoveFlag::DoublePawnPush);
	mExecution.makeMove(move);

	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::e3) << "En passant square should be e3 after e2-e4";
}


TEST_F(MoveExecutionTest, QuietMoveResetsEnPassantSquare)
{
	// First, make a double pawn push to set en passant
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));

	// Make a quiet move
	mExecution.makeMove(Move(Square::b8, Square::c6, MoveFlag::Quiet));

	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::None) << "En passant should be reset after quiet move";
}


TEST_F(MoveExecutionTest, KingsideCastlingMovesKingAndRook)
{
	mBoard.clear();

	// Set up castling position
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::WK);
	mBoard.updateOccupancies();

	Move castleMove(Square::e1, Square::g1, MoveFlag::KingCastle);
	mExecution.makeMove(castleMove);

	EXPECT_EQ(mBoard.pieceAt(Square::g1), PieceType::WKing) << "King should be at g1";
	EXPECT_EQ(mBoard.pieceAt(Square::f1), PieceType::WRook) << "Rook should be at f1";
	EXPECT_EQ(mBoard.pieceAt(Square::e1), PieceType::None) << "e1 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::h1), PieceType::None) << "h1 should be empty";
}


TEST_F(MoveExecutionTest, QueensideCastlingMovesKingAndRook)
{
	mBoard.clear();

	// Set up castling position
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::WQ);
	mBoard.updateOccupancies();

	Move castleMove(Square::e1, Square::c1, MoveFlag::QueenCastle);
	mExecution.makeMove(castleMove);

	EXPECT_EQ(mBoard.pieceAt(Square::c1), PieceType::WKing) << "King should be at c1";
	EXPECT_EQ(mBoard.pieceAt(Square::d1), PieceType::WRook) << "Rook should be at d1";
	EXPECT_EQ(mBoard.pieceAt(Square::e1), PieceType::None) << "e1 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::a1), PieceType::None) << "a1 should be empty";
}


TEST_F(MoveExecutionTest, UnmakeCastlingRestoresPosition)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::WK);
	mBoard.updateOccupancies();

	Move castleMove(Square::e1, Square::g1, MoveFlag::KingCastle);
	mExecution.makeMove(castleMove);
	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.pieceAt(Square::e1), PieceType::WKing) << "King should be back at e1";
	EXPECT_EQ(mBoard.pieceAt(Square::h1), PieceType::WRook) << "Rook should be back at h1";
	EXPECT_EQ(mBoard.pieceAt(Square::g1), PieceType::None) << "g1 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::f1), PieceType::None) << "f1 should be empty";
}


TEST_F(MoveExecutionTest, EnPassantCapturesCorrectPawn)
{
	mBoard.clear();

	// Set up en passant position
	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setEnPassantSquare(Square::d6);
	mBoard.updateOccupancies();

	Move enPassantMove(Square::e5, Square::d6, MoveFlag::EnPassant);
	mExecution.makeMove(enPassantMove);

	EXPECT_EQ(mBoard.pieceAt(Square::d6), PieceType::WPawn) << "White pawn should be at d6";
	EXPECT_EQ(mBoard.pieceAt(Square::e5), PieceType::None) << "e5 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::d5), PieceType::None) << "Captured pawn at d5 should be removed";
}


TEST_F(MoveExecutionTest, UnmakeEnPassantRestoresCapturedPawn)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setEnPassantSquare(Square::d6);
	mBoard.updateOccupancies();

	Move enPassantMove(Square::e5, Square::d6, MoveFlag::EnPassant);
	mExecution.makeMove(enPassantMove);
	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.pieceAt(Square::e5), PieceType::WPawn) << "White pawn should be back at e5";
	EXPECT_EQ(mBoard.pieceAt(Square::d5), PieceType::BPawn) << "Black pawn should be restored at d5";
	EXPECT_EQ(mBoard.pieceAt(Square::d6), PieceType::None) << "d6 should be empty";
}


TEST_F(MoveExecutionTest, PawnPromotionToQueen)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move promoMove(Square::e7, Square::e8, MoveFlag::QueenPromotion);
	mExecution.makeMove(promoMove);

	EXPECT_EQ(mBoard.pieceAt(Square::e8), PieceType::WQueen) << "e8 should have white queen after promotion";
	EXPECT_EQ(mBoard.pieceAt(Square::e7), PieceType::None) << "e7 should be empty";
}


TEST_F(MoveExecutionTest, PawnPromotionToKnight)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move promoMove(Square::e7, Square::e8, MoveFlag::KnightPromotion);
	mExecution.makeMove(promoMove);

	EXPECT_EQ(mBoard.pieceAt(Square::e8), PieceType::WKnight) << "e8 should have white knight after promotion";
}


TEST_F(MoveExecutionTest, PromotionCaptureWorks)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::BRook, Square::d8);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move promoCapture(Square::e7, Square::d8, MoveFlag::QueenPromoCapture);
	mExecution.makeMove(promoCapture);

	EXPECT_EQ(mBoard.pieceAt(Square::d8), PieceType::WQueen) << "d8 should have white queen";
	EXPECT_EQ(mBoard.pieceAt(Square::e7), PieceType::None) << "e7 should be empty";
}


TEST_F(MoveExecutionTest, UnmakePromotionRestoresPawn)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move promoMove(Square::e7, Square::e8, MoveFlag::QueenPromotion);
	mExecution.makeMove(promoMove);
	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.pieceAt(Square::e7), PieceType::WPawn) << "Pawn should be back at e7";
	EXPECT_EQ(mBoard.pieceAt(Square::e8), PieceType::None) << "e8 should be empty";
}


TEST_F(MoveExecutionTest, CastlingRightsLostAfterKingMove)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::WRook, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::WK | Castling::WQ);
	mBoard.updateOccupancies();

	// Move king
	mExecution.makeMove(Move(Square::e1, Square::f1, MoveFlag::Quiet));

	Castling rights = mBoard.getCurrentCastlingRights();
	EXPECT_FALSE(static_cast<bool>(rights & Castling::WK)) << "White kingside castling should be lost";
	EXPECT_FALSE(static_cast<bool>(rights & Castling::WQ)) << "White queenside castling should be lost";
}


TEST_F(MoveExecutionTest, CastlingRightsLostAfterRookMove)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::WRook, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::WK | Castling::WQ);
	mBoard.updateOccupancies();

	// Move kingside rook
	mExecution.makeMove(Move(Square::h1, Square::h2, MoveFlag::Quiet));

	Castling rights = mBoard.getCurrentCastlingRights();
	EXPECT_FALSE(static_cast<bool>(rights & Castling::WK)) << "White kingside castling should be lost";
	EXPECT_TRUE(static_cast<bool>(rights & Castling::WQ)) << "White queenside castling should still be available";
}


TEST_F(MoveExecutionTest, CastlingRightsRestoredOnUnmake)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::WK);
	mBoard.updateOccupancies();

	mExecution.makeMove(Move(Square::e1, Square::f1, MoveFlag::Quiet));
	mExecution.unmakeMove();

	Castling rights = mBoard.getCurrentCastlingRights();
	EXPECT_TRUE(static_cast<bool>(rights & Castling::WK)) << "White kingside castling should be restored";
}


TEST_F(MoveExecutionTest, HalfMoveClockIncrementsOnQuietMove)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WKnight, Square::b1);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setHalfMoveClock(0);
	mBoard.updateOccupancies();

	mExecution.makeMove(Move(Square::b1, Square::c3, MoveFlag::Quiet));

	EXPECT_EQ(mBoard.getHalfMoveClock(), 1) << "Half move clock should increment on quiet move";
}


TEST_F(MoveExecutionTest, HalfMoveClockResetsOnPawnMove)
{
	mBoard.setHalfMoveClock(10);

	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));

	EXPECT_EQ(mBoard.getHalfMoveClock(), 0) << "Half move clock should reset on pawn move";
}


TEST_F(MoveExecutionTest, HalfMoveClockResetsOnCapture)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WKnight, Square::e4);
	mBoard.addPiece(PieceType::BPawn, Square::f5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setHalfMoveClock(15);
	mBoard.updateOccupancies();

	mExecution.makeMove(Move(Square::e4, Square::f5, MoveFlag::Capture));

	EXPECT_EQ(mBoard.getHalfMoveClock(), 0) << "Half move clock should reset on capture";
}


TEST_F(MoveExecutionTest, MultipleMovesAndUnmakes)
{
	// Play several moves and unmake them all
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::e7, Square::e5, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::g1, Square::f3, MoveFlag::Quiet));

	EXPECT_EQ(mExecution.historySize(), 3) << "Should have 3 moves in history";

	mExecution.unmakeMove();
	mExecution.unmakeMove();
	mExecution.unmakeMove();

	EXPECT_EQ(mExecution.historySize(), 0) << "History should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::e2), PieceType::WPawn) << "e2 should have white pawn";
	EXPECT_EQ(mBoard.pieceAt(Square::e7), PieceType::BPawn) << "e7 should have black pawn";
	EXPECT_EQ(mBoard.pieceAt(Square::g1), PieceType::WKnight) << "g1 should have white knight";
	EXPECT_EQ(mBoard.getCurrentSide(), Side::White) << "Side should be white";
}


TEST_F(MoveExecutionTest, GetHistoryReturnsAllMoves)
{
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::e7, Square::e5, MoveFlag::DoublePawnPush));

	const auto &history = mExecution.getHistory();

	EXPECT_EQ(history.size(), 2) << "History should have 2 entries";
	EXPECT_EQ(history[0].move.from(), Square::e2) << "First move should be from e2";
	EXPECT_EQ(history[1].move.from(), Square::e7) << "Second move should be from e7";
}

} // namespace MoveTests
