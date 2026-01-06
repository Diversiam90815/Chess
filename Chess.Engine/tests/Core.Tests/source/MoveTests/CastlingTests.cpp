/*
  ==============================================================================
	Module:			Castling Tests
	Description:    Testing the castling special move
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Generation/MoveGeneration.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"


namespace MoveTests
{

class CastlingTests : public ::testing::Test
{
protected:
	Chessboard	   mBoard;
	MoveGeneration mGeneration{mBoard};
	MoveExecution  mExecution{mBoard};
	MoveValidation mValidation{mBoard, mGeneration, mExecution};

	void		   SetUp() override { mBoard.init(); }

	void		   SetupCastlingPosition(Side side)
	{
		mBoard.clear();

		if (side == Side::White)
		{
			mBoard.addPiece(PieceType::WKing, Square::e1);
			mBoard.addPiece(PieceType::WRook, Square::h1);
			mBoard.addPiece(PieceType::WRook, Square::a1);
			mBoard.addPiece(PieceType::BKing, Square::e8);
			mBoard.setSide(Side::White);
			mBoard.setCastlingRights(Castling::WK | Castling::WQ);
		}
		else
		{
			mBoard.addPiece(PieceType::BKing, Square::e8);
			mBoard.addPiece(PieceType::BRook, Square::h8);
			mBoard.addPiece(PieceType::BRook, Square::a8);
			mBoard.addPiece(PieceType::WKing, Square::e1);
			mBoard.setSide(Side::Black);
			mBoard.setCastlingRights(Castling::BK | Castling::BQ);
		}
		mBoard.updateOccupancies();
	}

	bool hasCastlingMove(const MoveList &moves, MoveFlag flag) const
	{
		for (size_t i = 0; i < moves.size(); ++i)
		{
			if (moves[i].flags() == flag)
				return true;
		}
		return false;
	}
};


TEST_F(CastlingTests, WhiteKingsideCastlingMovesKingAndRook)
{
	SetupCastlingPosition(Side::White);

	Move castleMove(Square::e1, Square::g1, MoveFlag::KingCastle);
	mExecution.makeMove(castleMove);

	EXPECT_EQ(mBoard.pieceAt(Square::g1), PieceType::WKing) << "King should be at g1";
	EXPECT_EQ(mBoard.pieceAt(Square::f1), PieceType::WRook) << "Rook should be at f1";
	EXPECT_EQ(mBoard.pieceAt(Square::e1), PieceType::None) << "e1 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::h1), PieceType::None) << "h1 should be empty";
}


TEST_F(CastlingTests, WhiteQueensideCastlingMovesKingAndRook)
{
	SetupCastlingPosition(Side::White);

	Move castleMove(Square::e1, Square::c1, MoveFlag::QueenCastle);
	mExecution.makeMove(castleMove);

	EXPECT_EQ(mBoard.pieceAt(Square::c1), PieceType::WKing) << "King should be at c1";
	EXPECT_EQ(mBoard.pieceAt(Square::d1), PieceType::WRook) << "Rook should be at d1";
	EXPECT_EQ(mBoard.pieceAt(Square::e1), PieceType::None) << "e1 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::a1), PieceType::None) << "a1 should be empty";
}


TEST_F(CastlingTests, BlackKingsideCastlingMovesKingAndRook)
{
	SetupCastlingPosition(Side::Black);

	Move castleMove(Square::e8, Square::g8, MoveFlag::KingCastle);
	mExecution.makeMove(castleMove);

	EXPECT_EQ(mBoard.pieceAt(Square::g8), PieceType::BKing) << "King should be at g8";
	EXPECT_EQ(mBoard.pieceAt(Square::f8), PieceType::BRook) << "Rook should be at f8";
	EXPECT_EQ(mBoard.pieceAt(Square::e8), PieceType::None) << "e8 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::h8), PieceType::None) << "h8 should be empty";
}


TEST_F(CastlingTests, BlackQueensideCastlingMovesKingAndRook)
{
	SetupCastlingPosition(Side::Black);

	Move castleMove(Square::e8, Square::c8, MoveFlag::QueenCastle);
	mExecution.makeMove(castleMove);

	EXPECT_EQ(mBoard.pieceAt(Square::c8), PieceType::BKing) << "King should be at c8";
	EXPECT_EQ(mBoard.pieceAt(Square::d8), PieceType::BRook) << "Rook should be at d8";
	EXPECT_EQ(mBoard.pieceAt(Square::e8), PieceType::None) << "e8 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::a8), PieceType::None) << "a8 should be empty";
}


TEST_F(CastlingTests, CastlingMovesAreGenerated)
{
	SetupCastlingPosition(Side::White);

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_TRUE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "Kingside castling should be generated";
	EXPECT_TRUE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "Queenside castling should be generated";
}


TEST_F(CastlingTests, KingsideCastlingBlockedByPiece)
{
	SetupCastlingPosition(Side::White);

	// Place a piece between king and kingside rook
	mBoard.addPiece(PieceType::WBishop, Square::f1);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "Kingside castling should be blocked";
	EXPECT_TRUE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "Queenside castling should still be available";
}


TEST_F(CastlingTests, QueensideCastlingBlockedByPiece)
{
	SetupCastlingPosition(Side::White);

	// Place a piece between king and queenside rook
	mBoard.addPiece(PieceType::WKnight, Square::b1);
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_TRUE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "Kingside castling should still be available";
	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "Queenside castling should be blocked";
}


TEST_F(CastlingTests, CastlingNotAvailableAfterKingMoves)
{
	SetupCastlingPosition(Side::White);

	// Move the king
	mExecution.makeMove(Move(Square::e1, Square::f1, MoveFlag::Quiet));

	// Move the king back
	mExecution.makeMove(Move(Square::e8, Square::d8, MoveFlag::Quiet)); // Black moves
	mExecution.makeMove(Move(Square::f1, Square::e1, MoveFlag::Quiet));

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "Kingside castling should not be available after king moved";
	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "Queenside castling should not be available after king moved";
}


TEST_F(CastlingTests, KingsideCastlingNotAvailableAfterRookMoves)
{
	SetupCastlingPosition(Side::White);

	// Move the kingside rook
	mExecution.makeMove(Move(Square::h1, Square::h2, MoveFlag::Quiet));

	// Make a black move and move rook back
	mExecution.makeMove(Move(Square::e8, Square::d8, MoveFlag::Quiet));
	mExecution.makeMove(Move(Square::h2, Square::h1, MoveFlag::Quiet));

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "Kingside castling should not be available after h-rook moved";
	EXPECT_TRUE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "Queenside castling should still be available";
}


TEST_F(CastlingTests, QueensideCastlingNotAvailableAfterRookMoves)
{
	SetupCastlingPosition(Side::White);

	// Move the queenside rook
	mExecution.makeMove(Move(Square::a1, Square::a2, MoveFlag::Quiet));

	// Make a black move and move rook back
	mExecution.makeMove(Move(Square::e8, Square::d8, MoveFlag::Quiet));
	mExecution.makeMove(Move(Square::a2, Square::a1, MoveFlag::Quiet));

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_TRUE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "Kingside castling should still be available";
	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "Queenside castling should not be available after a-rook moved";
}


TEST_F(CastlingTests, CastlingNotAllowedWhenKingInCheck)
{
	SetupCastlingPosition(Side::White);

	// Place black rook attacking the king
	mBoard.addPiece(PieceType::BRook, Square::e3);
	mBoard.updateOccupancies();

	MoveList moves;
	mValidation.generateLegalMoves(moves);

	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "Kingside castling should not be allowed when in check";
	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "Queenside castling should not be allowed when in check";
}


TEST_F(CastlingTests, KingsideCastlingNotAllowedThroughCheck)
{
	SetupCastlingPosition(Side::White);

	// Place black rook attacking f1 (square king passes through)
	mBoard.addPiece(PieceType::BRook, Square::f3);
	mBoard.updateOccupancies();

	MoveList moves;
	mValidation.generateLegalMoves(moves);

	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "Kingside castling should not be allowed through check";
	EXPECT_TRUE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "Queenside castling should still be allowed";
}


TEST_F(CastlingTests, QueensideCastlingNotAllowedThroughCheck)
{
	SetupCastlingPosition(Side::White);

	// Place black rook attacking d1 (square king passes through)
	mBoard.addPiece(PieceType::BRook, Square::d3);
	mBoard.updateOccupancies();

	MoveList moves;
	mValidation.generateLegalMoves(moves);

	EXPECT_TRUE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "Kingside castling should still be allowed";
	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "Queenside castling should not be allowed through check";
}


TEST_F(CastlingTests, CastlingNotAllowedIntoCheck)
{
	SetupCastlingPosition(Side::White);

	// Place black rook attacking g1 (king's destination for kingside)
	mBoard.addPiece(PieceType::BRook, Square::g3);
	mBoard.updateOccupancies();

	MoveList moves;
	mValidation.generateLegalMoves(moves);

	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "Kingside castling should not be allowed into check";
}


TEST_F(CastlingTests, UnmakeCastlingRestoresPosition)
{
	SetupCastlingPosition(Side::White);

	Move castleMove(Square::e1, Square::g1, MoveFlag::KingCastle);
	mExecution.makeMove(castleMove);
	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.pieceAt(Square::e1), PieceType::WKing) << "King should be back at e1";
	EXPECT_EQ(mBoard.pieceAt(Square::h1), PieceType::WRook) << "Rook should be back at h1";
	EXPECT_EQ(mBoard.pieceAt(Square::g1), PieceType::None) << "g1 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::f1), PieceType::None) << "f1 should be empty";
}


TEST_F(CastlingTests, UnmakeQueensideCastlingRestoresPosition)
{
	SetupCastlingPosition(Side::White);

	Move castleMove(Square::e1, Square::c1, MoveFlag::QueenCastle);
	mExecution.makeMove(castleMove);
	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.pieceAt(Square::e1), PieceType::WKing) << "King should be back at e1";
	EXPECT_EQ(mBoard.pieceAt(Square::a1), PieceType::WRook) << "Rook should be back at a1";
	EXPECT_EQ(mBoard.pieceAt(Square::c1), PieceType::None) << "c1 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::d1), PieceType::None) << "d1 should be empty";
}


TEST_F(CastlingTests, CastlingRightsRestoredOnUnmake)
{
	SetupCastlingPosition(Side::White);

	Castling originalRights = mBoard.getCurrentCastlingRights();

	Move	 castleMove(Square::e1, Square::g1, MoveFlag::KingCastle);
	mExecution.makeMove(castleMove);

	// After castling, rights should be lost
	EXPECT_FALSE(static_cast<bool>(mBoard.getCurrentCastlingRights() & Castling::WK));
	EXPECT_FALSE(static_cast<bool>(mBoard.getCurrentCastlingRights() & Castling::WQ));

	mExecution.unmakeMove();

	// Rights should be restored
	EXPECT_EQ(mBoard.getCurrentCastlingRights(), originalRights) << "Castling rights should be restored on unmake";
}


TEST_F(CastlingTests, CastlingRightsLostWhenRookCaptured)
{
	SetupCastlingPosition(Side::White);

	// Place black queen to capture white's h1 rook
	mBoard.addPiece(PieceType::BQueen, Square::h3);
	mBoard.updateOccupancies();

	// Simulate black capturing the rook (switch to black's turn first)
	mBoard.flipSide();
	Move captureMove(Square::h3, Square::h1, MoveFlag::Capture);
	mExecution.makeMove(captureMove);

	Castling rights = mBoard.getCurrentCastlingRights();
	EXPECT_FALSE(static_cast<bool>(rights & Castling::WK)) << "White kingside castling should be lost when h1 rook captured";
	EXPECT_TRUE(static_cast<bool>(rights & Castling::WQ)) << "White queenside castling should still be available";
}


TEST_F(CastlingTests, QueensideCastlingAllowedWithB1Attacked)
{
	SetupCastlingPosition(Side::White);

	// Place black piece attacking b1 (rook passes through, but king doesn't)
	mBoard.addPiece(PieceType::BRook, Square::b3);
	mBoard.updateOccupancies();

	MoveList moves;
	mValidation.generateLegalMoves(moves);

	// b1 being attacked should NOT prevent queenside castling
	// because the king doesn't pass through b1
	EXPECT_TRUE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "Queenside castling should be allowed even with b1 attacked";
}


TEST_F(CastlingTests, CastlingIsLegalMove)
{
	SetupCastlingPosition(Side::White);

	Move kingsideCastle(Square::e1, Square::g1, MoveFlag::KingCastle);
	Move queensideCastle(Square::e1, Square::c1, MoveFlag::QueenCastle);

	EXPECT_TRUE(mValidation.isMoveLegal(kingsideCastle)) << "Kingside castling should be legal";
	EXPECT_TRUE(mValidation.isMoveLegal(queensideCastle)) << "Queenside castling should be legal";
}


TEST_F(CastlingTests, NoCastlingRightsNoCastlingMoves)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::WRook, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::None); // No castling rights
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::KingCastle)) << "No kingside castling without rights";
	EXPECT_FALSE(hasCastlingMove(moves, MoveFlag::QueenCastle)) << "No queenside castling without rights";
}

} // namespace MoveTests
