/*
  ==============================================================================
	Module:			En Passant Tests
	Description:    Testing the en passant special move
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Generation/MoveGeneration.h"
#include "Validation/MoveValidation.h"
#include "Execution/MoveExecution.h"


namespace MoveTests
{

class EnPassantTests : public ::testing::Test
{
protected:
	Chessboard	   mBoard;
	MoveGeneration mGeneration{mBoard};
	MoveExecution  mExecution{mBoard};
	MoveValidation mValidation{mBoard, mGeneration, mExecution};

	void		   SetUp() override { mBoard.init(); }

	bool		   hasEnPassantMove(const MoveList &moves) const
	{
		for (size_t i = 0; i < moves.size(); ++i)
		{
			if (moves[i].isEnPassant())
				return true;
		}
		return false;
	}

	bool hasEnPassantTo(const MoveList &moves, Square target) const
	{
		for (size_t i = 0; i < moves.size(); ++i)
		{
			if (moves[i].isEnPassant() && moves[i].to() == target)
				return true;
		}
		return false;
	}
};


TEST_F(EnPassantTests, EnPassantSquareSetAfterDoublePawnPush)
{
	Move doublePush(Square::e2, Square::e4, MoveFlag::DoublePawnPush);
	mExecution.makeMove(doublePush);

	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::e3) << "En passant square should be e3 after e2-e4";
}


TEST_F(EnPassantTests, EnPassantSquareResetAfterOtherMove)
{
	// Make double pawn push
	mExecution.makeMove(Move(Square::e2, Square::e4, MoveFlag::DoublePawnPush));
	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::e3);

	// Make another move
	mExecution.makeMove(Move(Square::b8, Square::c6, MoveFlag::Quiet));

	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::None) << "En passant square should reset after non-double-push move";
}


TEST_F(EnPassantTests, EnPassantMoveGeneratedAfterDoublePush)
{
	mBoard.clear();

	// White pawn on e5, black pawn about to double push d7-d5
	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::Black);
	mBoard.updateOccupancies();

	// Black plays d7-d5
	mExecution.makeMove(Move(Square::d7, Square::d5, MoveFlag::DoublePawnPush));

	// Now white should have en passant available
	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_TRUE(hasEnPassantTo(moves, Square::d6)) << "En passant to d6 should be available";
}


TEST_F(EnPassantTests, EnPassantCaptureRemovesPawn)
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

	// Execute en passant
	Move enPassant(Square::e5, Square::d6, MoveFlag::EnPassant);
	mExecution.makeMove(enPassant);

	EXPECT_EQ(mBoard.pieceAt(Square::d6), PieceType::WPawn) << "White pawn should be at d6";
	EXPECT_EQ(mBoard.pieceAt(Square::e5), PieceType::None) << "e5 should be empty";
	EXPECT_EQ(mBoard.pieceAt(Square::d5), PieceType::None) << "Captured pawn at d5 should be removed";
}


TEST_F(EnPassantTests, EnPassantUnmakeRestoresPawn)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setEnPassantSquare(Square::d6);
	mBoard.updateOccupancies();

	Move enPassant(Square::e5, Square::d6, MoveFlag::EnPassant);
	mExecution.makeMove(enPassant);
	mExecution.unmakeMove();

	EXPECT_EQ(mBoard.pieceAt(Square::e5), PieceType::WPawn) << "White pawn should be back at e5";
	EXPECT_EQ(mBoard.pieceAt(Square::d5), PieceType::BPawn) << "Black pawn should be restored at d5";
	EXPECT_EQ(mBoard.pieceAt(Square::d6), PieceType::None) << "d6 should be empty";
	EXPECT_EQ(mBoard.getCurrentEnPassantSqaure(), Square::d6) << "En passant square should be restored";
}


TEST_F(EnPassantTests, EnPassantOnlyAvailableImmediately)
{
	mBoard.clear();

	// White pawns on e5 and h5, black pawn at d7
	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::WPawn, Square::h5);
	mBoard.addPiece(PieceType::BPawn, Square::d7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::Black);
	mBoard.updateOccupancies();

	// Black plays d7-d5
	mExecution.makeMove(Move(Square::d7, Square::d5, MoveFlag::DoublePawnPush));

	// White plays h5-h6 (not taking en passant)
	mExecution.makeMove(Move(Square::h5, Square::h6, MoveFlag::Quiet));

	// Black makes a random move
	mExecution.makeMove(Move(Square::e8, Square::d7, MoveFlag::Quiet));

	// Now white should NOT have en passant available
	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_FALSE(hasEnPassantMove(moves)) << "En passant should not be available after intervening moves";
}


TEST_F(EnPassantTests, EnPassantNotAvailableWithoutDoublePush)
{
	mBoard.clear();

	// White pawn on e5, black pawn directly placed on d5 (no double push)
	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	// No en passant square set
	mBoard.updateOccupancies();

	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_FALSE(hasEnPassantMove(moves)) << "En passant should not be available without prior double pawn push";
}


TEST_F(EnPassantTests, EnPassantFromBothSides)
{
	mBoard.clear();

	// White pawns on c5 and e5, black pawn double pushes d7-d5
	mBoard.addPiece(PieceType::WPawn, Square::c5);
	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::Black);
	mBoard.updateOccupancies();

	// Black plays d7-d5
	mExecution.makeMove(Move(Square::d7, Square::d5, MoveFlag::DoublePawnPush));

	// Both white pawns should be able to capture en passant
	MoveList moves;
	mGeneration.generateAllMoves(moves);

	int enPassantCount = 0;
	for (size_t i = 0; i < moves.size(); ++i)
	{
		if (moves[i].isEnPassant() && moves[i].to() == Square::d6)
			++enPassantCount;
	}

	EXPECT_EQ(enPassantCount, 2) << "Both white pawns should be able to capture en passant";
}


TEST_F(EnPassantTests, BlackEnPassantCapture)
{
	mBoard.clear();

	// Black pawn on e4, white pawn double pushes d2-d4
	mBoard.addPiece(PieceType::BPawn, Square::e4);
	mBoard.addPiece(PieceType::WPawn, Square::d2);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	// White plays d2-d4
	mExecution.makeMove(Move(Square::d2, Square::d4, MoveFlag::DoublePawnPush));

	// Black should have en passant available
	MoveList moves;
	mGeneration.generateAllMoves(moves);

	EXPECT_TRUE(hasEnPassantTo(moves, Square::d3)) << "Black en passant to d3 should be available";
}


TEST_F(EnPassantTests, EnPassantIsLegalMove)
{
	mBoard.clear();

	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setEnPassantSquare(Square::d6);
	mBoard.updateOccupancies();

	Move enPassant(Square::e5, Square::d6, MoveFlag::EnPassant);

	EXPECT_TRUE(mValidation.isMoveLegal(enPassant)) << "En passant should be a legal move";
}


TEST_F(EnPassantTests, EnPassantBlockedByPin)
{
	mBoard.clear();

	// White king on a5, white pawn on e5, black pawn on d5 (just double pushed)
	// Black rook on h5 pins the white pawn horizontally
	mBoard.addPiece(PieceType::WKing, Square::a5);
	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::BRook, Square::h5);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setEnPassantSquare(Square::d6);
	mBoard.updateOccupancies();

	Move enPassant(Square::e5, Square::d6, MoveFlag::EnPassant);

	// En passant would leave king in check (both pawns removed from 5th rank)
	EXPECT_FALSE(mValidation.isMoveLegal(enPassant)) << "En passant should be illegal when it exposes king to check";
}


} // namespace MoveTests
