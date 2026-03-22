/*
  ==============================================================================
	Module:			Move Validation Tests
	Description:    Testing the general move validation module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Validation/MoveValidation.h"


namespace MoveTests
{


class MoveValidationTest : public ::testing::Test
{
protected:
	Chessboard	   mBoard;
	MoveGeneration mGeneration{mBoard};
	MoveExecution  mExecution{mBoard};
	MoveValidation mValidation{mBoard, mGeneration, mExecution};

	void		   SetUp() override { mBoard.init(); }
};


TEST_F(MoveValidationTest, InitialSetupNotInCheck)
{
	EXPECT_FALSE(mValidation.isInCheck()) << "White King should not be in check on initial board";
}


TEST_F(MoveValidationTest, InitialSetupNotCheckmate)
{
	EXPECT_FALSE(mValidation.isCheckmate()) << "No checkmate on initial board";
}


TEST_F(MoveValidationTest, InitialSetupNotStalemate)
{
	EXPECT_FALSE(mValidation.isStalemate()) << "No stalemate on initial board";
}


TEST_F(MoveValidationTest, InitialSetupNotDraw)
{
	EXPECT_FALSE(mValidation.isDraw()) << "No draw on initial board";
}


TEST_F(MoveValidationTest, ValidMoveIsLegal)
{
	// e2-e4 is a legal move
	Move move(Square::e2, Square::e4, MoveFlag::DoublePawnPush);

	EXPECT_TRUE(mValidation.isMoveLegal(move)) << "e2-e4 should be a legal move";
}


TEST_F(MoveValidationTest, DetectsKingInCheckFromQueen)
{
	mBoard.clear();

	// White king at e1, black queen at e8 (in check along e-file)
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BQueen, Square::e8);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isInCheck()) << "King should be in check from queen on same file";
}


TEST_F(MoveValidationTest, DetectsKingInCheckFromKnight)
{
	mBoard.clear();

	// White king at e1, black knight at c2 (checking the king)
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKnight, Square::c2);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isInCheck()) << "King should be in check from knight's L-shaped attack";
}


TEST_F(MoveValidationTest, DetectsKingInCheckFromPawn)
{
	mBoard.clear();

	// White king at e1, black pawn at d2 (checking the king diagonally)
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BPawn, Square::d2);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isInCheck()) << "King should be in check from pawn's diagonal attack";
}


TEST_F(MoveValidationTest, DetectsKingInCheckFromBishop)
{
	mBoard.clear();

	// White king at e1, black bishop at h4 (checking diagonally)
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BBishop, Square::h4);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isInCheck()) << "King should be in check from bishop on diagonal";
}


TEST_F(MoveValidationTest, DetectsKingInCheckFromRook)
{
	mBoard.clear();

	// White king at e1, black rook at e8 (checking along file)
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BRook, Square::e8);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isInCheck()) << "King should be in check from rook on same file";
}


TEST_F(MoveValidationTest, DetectsScholarsMateCheckmate)
{
	mBoard.clear();

	// Scholar's mate position
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.addPiece(PieceType::BQueen, Square::d8);
	mBoard.addPiece(PieceType::BBishop, Square::f8);
	mBoard.addPiece(PieceType::BPawn, Square::d7);
	mBoard.addPiece(PieceType::BPawn, Square::e7);
	mBoard.addPiece(PieceType::BPawn, Square::g7);

	// White queen delivers checkmate at f7
	mBoard.addPiece(PieceType::WQueen, Square::f7);
	mBoard.addPiece(PieceType::WBishop, Square::c4); // Supports queen
	mBoard.addPiece(PieceType::WKing, Square::e1);

	mBoard.setSide(Side::Black);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isCheckmate()) << "Scholar's mate should be detected as checkmate";
}


TEST_F(MoveValidationTest, DetectsSimpleBackRankCheckmate)
{
	mBoard.clear();

	// Black king at h8, white rook at h1, blocking pawns
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::BPawn, Square::g8);
	mBoard.addPiece(PieceType::BPawn, Square::g7);
	mBoard.addPiece(PieceType::WKing, Square::a1);

	mBoard.setSide(Side::Black);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isCheckmate()) << "Back rank mate should be detected as checkmate";
}


TEST_F(MoveValidationTest, DetectsStalemate)
{
	mBoard.clear();

	// Classic stalemate: black king at h8, white queen at g6
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.addPiece(PieceType::WQueen, Square::g6);
	mBoard.addPiece(PieceType::WKing, Square::f6);

	mBoard.setSide(Side::Black);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isStalemate()) << "Position should be detected as stalemate";
}


TEST_F(MoveValidationTest, NotStalemateWhenInCheck)
{
	mBoard.clear();

	// Black king at h8, white queen at h6 (in check, not stalemate)
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.addPiece(PieceType::WQueen, Square::h6);
	mBoard.addPiece(PieceType::WKing, Square::f6);

	mBoard.setSide(Side::Black);
	mBoard.updateOccupancies();

	EXPECT_FALSE(mValidation.isStalemate()) << "Should not be stalemate when in check";
	EXPECT_TRUE(mValidation.isInCheck()) << "Should be in check";
}


TEST_F(MoveValidationTest, PinnedPieceCannotMoveOffPinLine)
{
	mBoard.clear();

	// White king at e1, white bishop at d2, black rook at c3
	// Bishop is pinned to the king
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WBishop, Square::d2);
	mBoard.addPiece(PieceType::BRook, Square::c3);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	// Illegal move that moves bishop off the pin line
	Move illegalMove(Square::d2, Square::b4, MoveFlag::Quiet);

	EXPECT_FALSE(mValidation.isMoveLegal(illegalMove)) << "Pinned bishop should not be allowed to move off the pin line";
}


TEST_F(MoveValidationTest, PinnedPieceCanMoveAlongPinLine)
{
	mBoard.clear();

	// White king at e1, white rook at e4, black rook at e8
	// White rook is pinned but can move along the file
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::e4);
	mBoard.addPiece(PieceType::BRook, Square::e8);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	// Legal move along the pin line
	Move legalMove(Square::e4, Square::e5, MoveFlag::Quiet);

	EXPECT_TRUE(mValidation.isMoveLegal(legalMove)) << "Pinned rook should be allowed to move along the pin line";
}


TEST_F(MoveValidationTest, KingCannotMoveIntoCheck)
{
	mBoard.clear();

	// White king at e1, black rook at e8
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BRook, Square::e8);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	// Illegal: king moves to e2, still in check
	Move illegalMove(Square::e1, Square::e2, MoveFlag::Quiet);

	// Legal: king moves to d1, escapes check
	Move legalMove(Square::e1, Square::d1, MoveFlag::Quiet);

	EXPECT_FALSE(mValidation.isMoveLegal(illegalMove)) << "King should not be allowed to move into check";
	EXPECT_TRUE(mValidation.isMoveLegal(legalMove)) << "King should be allowed to escape check";
}


TEST_F(MoveValidationTest, MustBlockOrCaptureWhenInCheck)
{
	mBoard.clear();

	// White king at e1 in check from black rook at e8
	// White bishop at d3 can block on e4
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WBishop, Square::d3);
	mBoard.addPiece(PieceType::BRook, Square::e8);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	// Legal: bishop blocks at e4
	Move blockMove(Square::d3, Square::e4, MoveFlag::Quiet);

	// Illegal: bishop moves somewhere that doesn't address the check
	Move illegalMove(Square::d3, Square::c2, MoveFlag::Quiet);

	EXPECT_TRUE(mValidation.isMoveLegal(blockMove)) << "Bishop should be able to block the check";
	EXPECT_FALSE(mValidation.isMoveLegal(illegalMove)) << "Must address check, cannot make unrelated move";
}


TEST_F(MoveValidationTest, CanCaptureCheckingPiece)
{
	mBoard.clear();

	// White king at e1, white knight at d3, black rook at e3 checking
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WKnight, Square::d5);
	mBoard.addPiece(PieceType::BRook, Square::e3);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	// Knight can capture the checking rook
	Move captureMove(Square::d5, Square::e3, MoveFlag::Capture);

	EXPECT_TRUE(mValidation.isMoveLegal(captureMove)) << "Knight should be able to capture the checking rook";
}


TEST_F(MoveValidationTest, GenerateLegalMovesFiltersIllegal)
{
	mBoard.clear();

	// Position where king is in check
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WPawn, Square::a2);
	mBoard.addPiece(PieceType::BRook, Square::e8);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	MoveList legalMoves;
	mValidation.generateLegalMoves(legalMoves);

	// All moves should be legal (escape check)
	for (size_t i = 0; i < legalMoves.size(); ++i)
	{
		EXPECT_TRUE(mValidation.isMoveLegal(legalMoves[i])) << "All generated moves should be legal";
	}

	// Pawn move should not be in the list (doesn't address check)
	bool hasPawnMove = false;
	for (size_t i = 0; i < legalMoves.size(); ++i)
	{
		if (legalMoves[i].from() == Square::a2)
		{
			hasPawnMove = true;
			break;
		}
	}

	EXPECT_FALSE(hasPawnMove) << "Pawn move should not be legal when king is in check";
}


TEST_F(MoveValidationTest, CountLegalMovesReturnsCorrectCount)
{
	size_t count = mValidation.countLegalMoves();

	// Initial position has 20 legal moves
	EXPECT_EQ(count, 20) << "Initial position should have 20 legal moves";
}


TEST_F(MoveValidationTest, BlockingPreventsCheckmate)
{
	mBoard.clear();

	// Black king at h8, black rook at h7, white queen at g7
	// Not checkmate because rook can block
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.addPiece(PieceType::BRook, Square::f7);
	mBoard.addPiece(PieceType::WQueen, Square::g7);
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.setSide(Side::Black);
	mBoard.updateOccupancies();

	EXPECT_FALSE(mValidation.isCheckmate()) << "Not checkmate when rook can block";
}


TEST_F(MoveValidationTest, InsufficientMaterialDraw)
{
	mBoard.clear();

	// King vs King is a draw
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isDraw()) << "King vs King should be a draw";
}


TEST_F(MoveValidationTest, KingAndBishopVsKingIsDraw)
{
	mBoard.clear();

	// King + Bishop vs King is a draw
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WBishop, Square::c1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isDraw()) << "King + Bishop vs King should be a draw";
}


TEST_F(MoveValidationTest, KingAndKnightVsKingIsDraw)
{
	mBoard.clear();

	// King + Knight vs King is a draw
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WKnight, Square::b1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	EXPECT_TRUE(mValidation.isDraw()) << "King + Knight vs King should be a draw";
}


} // namespace MoveTests
