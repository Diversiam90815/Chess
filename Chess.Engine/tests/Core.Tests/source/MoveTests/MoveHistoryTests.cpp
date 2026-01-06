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
	Chessboard	  mBoard;
	MoveExecution mExecution{mBoard};

	void		  SetUp() override { mBoard.init(); }

	void		  TearDown() override { mExecution.clearHistory(); }
};


TEST_F(MoveHistoryTests, InitiallyEmptyHistory)
{
	const MoveHistoryEntry *lastMove = mExecution.getLastMove();
	EXPECT_EQ(lastMove, nullptr) << "Move history should be empty initially";
	EXPECT_EQ(mExecution.historySize(), 0) << "History size should be 0";
}


TEST_F(MoveHistoryTests, MakeMoveAddsToHistory)
{
	Move move(Square::e2, Square::e4, MoveFlag::DoublePawnPush);
	mExecution.makeMove(move);

	EXPECT_EQ(mExecution.historySize(), 1) << "History should have one entry";

	const MoveHistoryEntry *lastMove = mExecution.getLastMove();
	ASSERT_NE(lastMove, nullptr) << "Last move should not be null";
	EXPECT_EQ(lastMove->move.from(), Square::e2) << "Move from should be e2";
	EXPECT_EQ(lastMove->move.to(), Square::e4) << "Move to should be e4";
}


TEST_F(MoveHistoryTests, MultipleMovesInHistory)
{
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::e7, Square::e5, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::g1, Square::f3, MoveFlag::Quiet));

	EXPECT_EQ(mExecution.historySize(), 3) << "History should have 3 entries";

	const MoveHistoryEntry *lastMove = mExecution.getLastMove();
	ASSERT_NE(lastMove, nullptr);
	EXPECT_EQ(lastMove->move.from(), Square::g1) << "Last move should be Nf3";
	EXPECT_EQ(lastMove->move.to(), Square::f3);
}


TEST_F(MoveHistoryTests, UnmakeMoveRemovesFromHistory)
{
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::e7, Square::e5, MoveFlag::DoublePawnPush));

	EXPECT_EQ(mExecution.historySize(), 2);

	mExecution.unmakeMove();

	EXPECT_EQ(mExecution.historySize(), 1) << "History should have 1 entry after unmake";

	const MoveHistoryEntry *lastMove = mExecution.getLastMove();
	ASSERT_NE(lastMove, nullptr);
	EXPECT_EQ(lastMove->move.from(), Square::e2) << "Last move should now be e2-e4";
}


TEST_F(MoveHistoryTests, UnmakeAllMoves)
{
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::e7, Square::e5, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::g1, Square::f3, MoveFlag::Quiet));

	mExecution.unmakeMove();
	mExecution.unmakeMove();
	mExecution.unmakeMove();

	EXPECT_EQ(mExecution.historySize(), 0) << "History should be empty";
	EXPECT_EQ(mExecution.getLastMove(), nullptr) << "No last move";
}


TEST_F(MoveHistoryTests, UnmakeFromEmptyHistoryReturnsFalse)
{
	bool result = mExecution.unmakeMove();
	EXPECT_FALSE(result) << "Unmake from empty history should return false";
}


TEST_F(MoveHistoryTests, ClearHistoryRemovesAllEntries)
{
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::e7, Square::e5, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::g1, Square::f3, MoveFlag::Quiet));

	mExecution.clearHistory();

	EXPECT_EQ(mExecution.historySize(), 0) << "History should be empty after clear";
	EXPECT_EQ(mExecution.getLastMove(), nullptr) << "No last move after clear";
}


TEST_F(MoveHistoryTests, HistoryPreservesBoardState)
{
	// Make a move that changes board state
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));

	const MoveHistoryEntry *entry = mExecution.getLastMove();
	ASSERT_NE(entry, nullptr);

	// Previous state should have no en passant square
	EXPECT_EQ(entry->previousState.enPassant, Square::None) << "Previous state should have no en passant";
}


TEST_F(MoveHistoryTests, UnmakeRestoresCastlingRights)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::WK);
	mBoard.updateOccupancies();

	Castling originalRights = mBoard.getCurrentCastlingRights();

	// Move king (loses castling rights)
	mExecution.makeMove(Move(Square::e1, Square::f1, MoveFlag::Quiet));

	EXPECT_NE(mBoard.getCurrentCastlingRights(), originalRights) << "Castling rights should change";

	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.getCurrentCastlingRights(), originalRights) << "Castling rights should be restored";
}


TEST_F(MoveHistoryTests, UnmakeRestoresEnPassantSquare)
{
	// After double pawn push, en passant square is set
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));

	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::e3) << "En passant should be e3";

	// Make another move (resets en passant)
	mExecution.makeMove(Move(Square::b8, Square::c6, MoveFlag::Quiet));

	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::None) << "En passant should be None";

	// Unmake should restore en passant
	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::e3) << "En passant should be restored to e3";
}


TEST_F(MoveHistoryTests, UnmakeRestoresCapturedPiece)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WKnight, Square::f3);
	mBoard.addPiece(PieceType::BPawn, Square::e5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	// Capture the pawn
	mExecution.makeMove(Move(Square::f3, Square::e5, MoveFlag::Capture));

	EXPECT_EQ(mBoard.pieceAt(Square::e5), PieceType::WKnight);
	EXPECT_EQ(mBoard.pieceAt(Square::f3), PieceType::None);

	// Unmake
	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.pieceAt(Square::f3), PieceType::WKnight) << "Knight should be back";
	EXPECT_EQ(mBoard.pieceAt(Square::e5), PieceType::BPawn) << "Captured pawn should be restored";
}


TEST_F(MoveHistoryTests, UnmakeRestoresHalfMoveClock)
{
	mBoard.setHalfMoveClock(10);

	// Pawn move resets half move clock
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));

	EXPECT_EQ(mBoard.getHalfMoveClock(), 0) << "Half move clock should reset on pawn move";

	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.getHalfMoveClock(), 10) << "Half move clock should be restored";
}


TEST_F(MoveHistoryTests, GetHistoryReturnsAllMoves)
{
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::e7, Square::e5, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::g1, Square::f3, MoveFlag::Quiet));

	const auto &history = mExecution.getHistory();

	EXPECT_EQ(history.size(), 3) << "History should have 3 entries";
	EXPECT_EQ(history[0].move.from(), Square::e2);
	EXPECT_EQ(history[1].move.from(), Square::e7);
	EXPECT_EQ(history[2].move.from(), Square::g1);
}


TEST_F(MoveHistoryTests, CaptureRecordsCapturedPieceInState)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WKnight, Square::f3);
	mBoard.addPiece(PieceType::BPawn, Square::e5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	mExecution.makeMove(Move(Square::f3, Square::e5, MoveFlag::Capture));

	const MoveHistoryEntry *entry = mExecution.getLastMove();
	ASSERT_NE(entry, nullptr);
	EXPECT_EQ(entry->previousState.capturedPiece, PieceType::BPawn) << "Should record captured pawn";
}


TEST_F(MoveHistoryTests, EnPassantCaptureRecordedCorrectly)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setEnPassantSquare(Square::d6);
	mBoard.updateOccupancies();

	mExecution.makeMove(Move(Square::e5, Square::d6, MoveFlag::EnPassant));

	// En passant captures a pawn
	const MoveHistoryEntry *entry = mExecution.getLastMove();
	ASSERT_NE(entry, nullptr);
	EXPECT_EQ(entry->previousState.capturedPiece, PieceType::BPawn) << "En passant should record captured pawn";
}


TEST_F(MoveHistoryTests, PromotionRecordedInHistory)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	mExecution.makeMove(Move(Square::e7, Square::e8, MoveFlag::QueenPromotion));

	const MoveHistoryEntry *entry = mExecution.getLastMove();
	ASSERT_NE(entry, nullptr);
	EXPECT_TRUE(entry->move.isPromotion()) << "Move should be marked as promotion";
}


TEST_F(MoveHistoryTests, UnmakePromotionRestoresPawn)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	mExecution.makeMove(Move(Square::e7, Square::e8, MoveFlag::QueenPromotion));

	EXPECT_EQ(mBoard.pieceAt(Square::e8), PieceType::WQueen) << "Should be queen after promotion";

	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.pieceAt(Square::e7), PieceType::WPawn) << "Pawn should be restored";
	EXPECT_EQ(mBoard.pieceAt(Square::e8), PieceType::None) << "e8 should be empty";
}


TEST_F(MoveHistoryTests, CastlingRecordedInHistory)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::WK);
	mBoard.updateOccupancies();

	mExecution.makeMove(Move(Square::e1, Square::g1, MoveFlag::KingCastle));

	const MoveHistoryEntry *entry = mExecution.getLastMove();
	ASSERT_NE(entry, nullptr);
	EXPECT_TRUE(entry->move.isCastle()) << "Move should be marked as castle";
}


TEST_F(MoveHistoryTests, ZobristHashRestoredOnUnmake)
{
	mBoard.computeHash();
	uint64_t originalHash = mBoard.getHash();

	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));

	EXPECT_NE(mBoard.getHash(), originalHash) << "Hash should change after move";

	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.getHash(), originalHash) << "Hash should be restored after unmake";
}


TEST_F(MoveHistoryTests, MultipleMakesAndUnmakesPreserveState)
{
	// Save initial state
	mBoard.computeHash();
	uint64_t initialHash	 = mBoard.getHash();
	Castling initialCastling = mBoard.getCurrentCastlingRights();

	// Play several moves
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::e7, Square::e5, MoveFlag::DoublePawnPush));
	mExecution.makeMove(Move(Square::g1, Square::f3, MoveFlag::Quiet));
	mExecution.makeMove(Move(Square::b8, Square::c6, MoveFlag::Quiet));
	mExecution.makeMove(Move(Square::f1, Square::c4, MoveFlag::Quiet));

	// Unmake all
	while (mExecution.historySize() > 0)
	{
		mExecution.unmakeMove();
	}

	EXPECT_EQ(mBoard.getHash(), initialHash) << "Hash should match initial state";
	EXPECT_EQ(mBoard.getCurrentCastlingRights(), initialCastling) << "Castling rights should match";
	EXPECT_EQ(mBoard.pieceAt(Square::e2), PieceType::WPawn) << "Pieces should be in initial positions";
	EXPECT_EQ(mBoard.pieceAt(Square::g1), PieceType::WKnight);
}


} // namespace MoveTests
