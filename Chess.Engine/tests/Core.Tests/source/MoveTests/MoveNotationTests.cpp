/*
  ==============================================================================
	Module:			Move Notation Tests
	Description:    Testing the MoveNotation class for Standard Algebraic Notation generation
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Notation/MoveNotation.h"

namespace MoveTests
{

class MoveNotationTests : public ::testing::Test
{
protected:
	Chessboard	 mBoard;
	MoveNotation mNotation;

	void		 SetUp() override { mBoard.init(); }
};


//=============================================================================
// PAWN MOVE NOTATION TESTS
//=============================================================================

TEST_F(MoveNotationTests, SimplePawnDoublePush)
{
	Move		pawnMove(Square::e2, Square::e4, MoveFlag::DoublePawnPush);

	std::string notation = mNotation.toSAN(pawnMove, mBoard, false, false);
	EXPECT_EQ(notation, "e4") << "Simple pawn move should be destination only";
}


TEST_F(MoveNotationTests, PawnSinglePush)
{
	Move		pawnMove(Square::e2, Square::e3, MoveFlag::Quiet);

	std::string notation = mNotation.toSAN(pawnMove, mBoard, false, false);
	EXPECT_EQ(notation, "e3") << "Pawn single push should be destination only";
}


TEST_F(MoveNotationTests, PawnCapture)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e4);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		captureMove(Square::e4, Square::d5, MoveFlag::Capture);

	std::string notation = mNotation.toSAN(captureMove, mBoard, false, false);
	EXPECT_EQ(notation, "exd5") << "Pawn capture should include file and x";
}


TEST_F(MoveNotationTests, PawnCaptureFromDifferentFiles)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::a4);
	mBoard.addPiece(PieceType::BPawn, Square::b5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		captureMove(Square::a4, Square::b5, MoveFlag::Capture);

	std::string notation = mNotation.toSAN(captureMove, mBoard, false, false);
	EXPECT_EQ(notation, "axb5") << "Pawn capture from a-file should include 'a'";
}


TEST_F(MoveNotationTests, PawnPromotionToQueen)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		promoMove(Square::e7, Square::e8, MoveFlag::QueenPromotion);

	std::string notation = mNotation.toSAN(promoMove, mBoard, false, false);
	EXPECT_EQ(notation, "e8=Q") << "Promotion should include =Q";
}


TEST_F(MoveNotationTests, PawnPromotionToKnight)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		promoMove(Square::e7, Square::e8, MoveFlag::KnightPromotion);

	std::string notation = mNotation.toSAN(promoMove, mBoard, false, false);
	EXPECT_EQ(notation, "e8=N") << "Knight promotion should include =N";
}


TEST_F(MoveNotationTests, PawnPromotionToRook)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		promoMove(Square::e7, Square::e8, MoveFlag::RookPromotion);

	std::string notation = mNotation.toSAN(promoMove, mBoard, false, false);
	EXPECT_EQ(notation, "e8=R") << "Rook promotion should include =R";
}


TEST_F(MoveNotationTests, PawnPromotionToBishop)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		promoMove(Square::e7, Square::e8, MoveFlag::BishopPromotion);

	std::string notation = mNotation.toSAN(promoMove, mBoard, false, false);
	EXPECT_EQ(notation, "e8=B") << "Bishop promotion should include =B";
}


TEST_F(MoveNotationTests, PawnPromotionWithCapture)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::BRook, Square::d8);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::a8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		promoCapture(Square::e7, Square::d8, MoveFlag::QueenPromoCapture);

	std::string notation = mNotation.toSAN(promoCapture, mBoard, false, false);
	EXPECT_EQ(notation, "exd8=Q") << "Promotion capture should include file, x, dest, =Q";
}


TEST_F(MoveNotationTests, EnPassantCapture)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e5);
	mBoard.addPiece(PieceType::BPawn, Square::d5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.setEnPassantSquare(Square::d6);
	mBoard.updateOccupancies();

	Move		enPassant(Square::e5, Square::d6, MoveFlag::EnPassant);

	std::string notation = mNotation.toSAN(enPassant, mBoard, false, false);
	EXPECT_EQ(notation, "exd6") << "En passant should be formatted as capture";
}


//=============================================================================
// PIECE MOVE NOTATION TESTS
//=============================================================================

TEST_F(MoveNotationTests, KnightMove)
{
	Move		knightMove(Square::g1, Square::f3, MoveFlag::Quiet);

	std::string notation = mNotation.toSAN(knightMove, mBoard, false, false);
	EXPECT_EQ(notation, "Nf3") << "Knight move should include N";
}


TEST_F(MoveNotationTests, BishopMove)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WBishop, Square::c1);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		bishopMove(Square::c1, Square::f4, MoveFlag::Quiet);

	std::string notation = mNotation.toSAN(bishopMove, mBoard, false, false);
	EXPECT_EQ(notation, "Bf4") << "Bishop move should include B";
}


TEST_F(MoveNotationTests, RookMove)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WRook, Square::a1);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		rookMove(Square::a1, Square::a4, MoveFlag::Quiet);

	std::string notation = mNotation.toSAN(rookMove, mBoard, false, false);
	EXPECT_EQ(notation, "Ra4") << "Rook move should include R";
}


TEST_F(MoveNotationTests, QueenMove)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WQueen, Square::d1);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		queenMove(Square::d1, Square::d4, MoveFlag::Quiet);

	std::string notation = mNotation.toSAN(queenMove, mBoard, false, false);
	EXPECT_EQ(notation, "Qd4") << "Queen move should include Q";
}


TEST_F(MoveNotationTests, KingMove)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		kingMove(Square::e1, Square::e2, MoveFlag::Quiet);

	std::string notation = mNotation.toSAN(kingMove, mBoard, false, false);
	EXPECT_EQ(notation, "Ke2") << "King move should include K";
}


//=============================================================================
// CAPTURE NOTATION TESTS
//=============================================================================

TEST_F(MoveNotationTests, KnightCapture)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WKnight, Square::f3);
	mBoard.addPiece(PieceType::BPawn, Square::e5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		captureMove(Square::f3, Square::e5, MoveFlag::Capture);

	std::string notation = mNotation.toSAN(captureMove, mBoard, false, false);
	EXPECT_EQ(notation, "Nxe5") << "Knight capture should include Nx";
}


TEST_F(MoveNotationTests, BishopCapture)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WBishop, Square::c4);
	mBoard.addPiece(PieceType::BPawn, Square::f7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		captureMove(Square::c4, Square::f7, MoveFlag::Capture);

	std::string notation = mNotation.toSAN(captureMove, mBoard, false, false);
	EXPECT_EQ(notation, "Bxf7") << "Bishop capture should include Bx";
}


TEST_F(MoveNotationTests, RookCapture)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WRook, Square::a1);
	mBoard.addPiece(PieceType::BRook, Square::a8);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		captureMove(Square::a1, Square::a8, MoveFlag::Capture);

	std::string notation = mNotation.toSAN(captureMove, mBoard, false, false);
	EXPECT_EQ(notation, "Rxa8") << "Rook capture should include Rx";
}


TEST_F(MoveNotationTests, QueenCapture)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WQueen, Square::d1);
	mBoard.addPiece(PieceType::BPawn, Square::d7);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		captureMove(Square::d1, Square::d7, MoveFlag::Capture);

	std::string notation = mNotation.toSAN(captureMove, mBoard, false, false);
	EXPECT_EQ(notation, "Qxd7") << "Queen capture should include Qx";
}


TEST_F(MoveNotationTests, KingCapture)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WKing, Square::e4);
	mBoard.addPiece(PieceType::BPawn, Square::e5);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		captureMove(Square::e4, Square::e5, MoveFlag::Capture);

	std::string notation = mNotation.toSAN(captureMove, mBoard, false, false);
	EXPECT_EQ(notation, "Kxe5") << "King capture should include Kx";
}


//=============================================================================
// CASTLING NOTATION TESTS
//=============================================================================

TEST_F(MoveNotationTests, KingsideCastling)
{
	Move		castleMove(Square::e1, Square::g1, MoveFlag::KingCastle);

	std::string notation = mNotation.toSAN(castleMove, mBoard, false, false);
	EXPECT_EQ(notation, "O-O") << "Kingside castling should be O-O";
}


TEST_F(MoveNotationTests, QueensideCastling)
{
	Move		castleMove(Square::e1, Square::c1, MoveFlag::QueenCastle);

	std::string notation = mNotation.toSAN(castleMove, mBoard, false, false);
	EXPECT_EQ(notation, "O-O-O") << "Queenside castling should be O-O-O";
}


TEST_F(MoveNotationTests, BlackKingsideCastling)
{
	mBoard.flipSide();
	Move		castleMove(Square::e8, Square::g8, MoveFlag::KingCastle);

	std::string notation = mNotation.toSAN(castleMove, mBoard, false, false);
	EXPECT_EQ(notation, "O-O") << "Black kingside castling should be O-O";
}


TEST_F(MoveNotationTests, BlackQueensideCastling)
{
	mBoard.flipSide();
	Move		castleMove(Square::e8, Square::c8, MoveFlag::QueenCastle);

	std::string notation = mNotation.toSAN(castleMove, mBoard, false, false);
	EXPECT_EQ(notation, "O-O-O") << "Black queenside castling should be O-O-O";
}


//=============================================================================
// CHECK AND CHECKMATE NOTATION TESTS
//=============================================================================

TEST_F(MoveNotationTests, MoveWithCheck)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WQueen, Square::d1);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::d8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		checkMove(Square::d1, Square::d7, MoveFlag::Quiet);

	std::string notation = mNotation.toSAN(checkMove, mBoard, true, false);
	EXPECT_EQ(notation, "Qd7+") << "Move giving check should end with +";
}


TEST_F(MoveNotationTests, MoveWithCheckmate)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WQueen, Square::h5);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.addPiece(PieceType::BPawn, Square::f7);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		checkmateMove(Square::h5, Square::f7, MoveFlag::Capture);

	std::string notation = mNotation.toSAN(checkmateMove, mBoard, false, true);
	EXPECT_EQ(notation, "Qxf7#") << "Move giving checkmate should end with #";
}


TEST_F(MoveNotationTests, PawnMoveWithCheck)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::d6);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		pawnCheck(Square::d6, Square::d7, MoveFlag::Quiet);

	std::string notation = mNotation.toSAN(pawnCheck, mBoard, true, false);
	EXPECT_EQ(notation, "d7+") << "Pawn move giving check should end with +";
}


TEST_F(MoveNotationTests, CaptureWithCheckmate)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WQueen, Square::g5);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.addPiece(PieceType::BPawn, Square::g7);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		captureCheckmate(Square::g5, Square::g7, MoveFlag::Capture);

	std::string notation = mNotation.toSAN(captureCheckmate, mBoard, false, true);
	EXPECT_EQ(notation, "Qxg7#") << "Capture giving checkmate should have x and #";
}


TEST_F(MoveNotationTests, CastlingWithCheck)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::BKing, Square::f8); // King on f-file, castling gives check
	mBoard.setSide(Side::White);
	mBoard.setCastlingRights(Castling::WK);
	mBoard.updateOccupancies();

	Move		castleCheck(Square::e1, Square::g1, MoveFlag::KingCastle);

	std::string notation = mNotation.toSAN(castleCheck, mBoard, true, false);
	EXPECT_EQ(notation, "O-O+") << "Castling with check should be O-O+";
}


TEST_F(MoveNotationTests, PromotionWithCheck)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::e6);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		promoCheck(Square::e7, Square::e8, MoveFlag::QueenPromotion);

	std::string notation = mNotation.toSAN(promoCheck, mBoard, true, false);
	EXPECT_EQ(notation, "e8=Q+") << "Promotion with check should include + at end";
}


TEST_F(MoveNotationTests, PromotionWithCheckmate)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WPawn, Square::e7);
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::h8);
	mBoard.addPiece(PieceType::WRook, Square::g1);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		promoMate(Square::e7, Square::e8, MoveFlag::QueenPromotion);

	std::string notation = mNotation.toSAN(promoMate, mBoard, false, true);
	EXPECT_EQ(notation, "e8=Q#") << "Promotion with checkmate should include # at end";
}


//=============================================================================
// UCI NOTATION TESTS
//=============================================================================

TEST_F(MoveNotationTests, UCISimpleMove)
{
	Move		move(Square::e2, Square::e4, MoveFlag::DoublePawnPush);

	std::string uci = mNotation.toUCI(move);
	EXPECT_EQ(uci, "e2e4") << "UCI notation should be source + destination";
}


TEST_F(MoveNotationTests, UCICapture)
{
	Move		move(Square::e4, Square::d5, MoveFlag::Capture);

	std::string uci = mNotation.toUCI(move);
	EXPECT_EQ(uci, "e4d5") << "UCI capture notation should be source + destination";
}


TEST_F(MoveNotationTests, UCIKingsideCastling)
{
	Move		move(Square::e1, Square::g1, MoveFlag::KingCastle);

	std::string uci = mNotation.toUCI(move);
	EXPECT_EQ(uci, "e1g1") << "UCI castling should be king from-to";
}


TEST_F(MoveNotationTests, UCIQueensideCastling)
{
	Move		move(Square::e1, Square::c1, MoveFlag::QueenCastle);

	std::string uci = mNotation.toUCI(move);
	EXPECT_EQ(uci, "e1c1") << "UCI queenside castling should be king from-to";
}


TEST_F(MoveNotationTests, UCIPromotionToQueen)
{
	Move		move(Square::e7, Square::e8, MoveFlag::QueenPromotion);

	std::string uci = mNotation.toUCI(move);
	EXPECT_EQ(uci, "e7e8q") << "UCI queen promotion should end with 'q'";
}


TEST_F(MoveNotationTests, UCIPromotionToRook)
{
	Move		move(Square::e7, Square::e8, MoveFlag::RookPromotion);

	std::string uci = mNotation.toUCI(move);
	EXPECT_EQ(uci, "e7e8r") << "UCI rook promotion should end with 'r'";
}


TEST_F(MoveNotationTests, UCIPromotionToBishop)
{
	Move		move(Square::e7, Square::e8, MoveFlag::BishopPromotion);

	std::string uci = mNotation.toUCI(move);
	EXPECT_EQ(uci, "e7e8b") << "UCI bishop promotion should end with 'b'";
}


TEST_F(MoveNotationTests, UCIPromotionToKnight)
{
	Move		move(Square::e7, Square::e8, MoveFlag::KnightPromotion);

	std::string uci = mNotation.toUCI(move);
	EXPECT_EQ(uci, "e7e8n") << "UCI knight promotion should end with 'n'";
}


TEST_F(MoveNotationTests, UCIPromotionCapture)
{
	Move		move(Square::e7, Square::d8, MoveFlag::QueenPromoCapture);

	std::string uci = mNotation.toUCI(move);
	EXPECT_EQ(uci, "e7d8q") << "UCI promotion capture should include 'q'";
}


TEST_F(MoveNotationTests, UCIEnPassant)
{
	Move		move(Square::e5, Square::d6, MoveFlag::EnPassant);

	std::string uci = mNotation.toUCI(move);
	EXPECT_EQ(uci, "e5d6") << "UCI en passant should be source + destination";
}


//=============================================================================
// CORNER CASES
//=============================================================================

TEST_F(MoveNotationTests, CornerSquaresA1)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WRook, Square::a8);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		rookMove(Square::a8, Square::a1, MoveFlag::Quiet);

	std::string notation = mNotation.toSAN(rookMove, mBoard, false, false);
	EXPECT_EQ(notation, "Ra1");
}


TEST_F(MoveNotationTests, CornerSquaresH8)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::WRook, Square::h1);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::White);
	mBoard.updateOccupancies();

	Move		rookMove(Square::h1, Square::h8, MoveFlag::Quiet);

	std::string notation = mNotation.toSAN(rookMove, mBoard, false, false);
	EXPECT_EQ(notation, "Rh8");
}


TEST_F(MoveNotationTests, BlackPawnCapture)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::BPawn, Square::e5);
	mBoard.addPiece(PieceType::WPawn, Square::d4);
	mBoard.addPiece(PieceType::WKing, Square::e1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::Black);
	mBoard.updateOccupancies();

	Move		captureMove(Square::e5, Square::d4, MoveFlag::Capture);

	std::string notation = mNotation.toSAN(captureMove, mBoard, false, false);
	EXPECT_EQ(notation, "exd4") << "Black pawn capture should use same format";
}


TEST_F(MoveNotationTests, BlackPawnPromotion)
{
	mBoard.clear();
	mBoard.addPiece(PieceType::BPawn, Square::e2);
	mBoard.addPiece(PieceType::WKing, Square::a1);
	mBoard.addPiece(PieceType::BKing, Square::e8);
	mBoard.setSide(Side::Black);
	mBoard.updateOccupancies();

	Move		promoMove(Square::e2, Square::e1, MoveFlag::QueenPromotion);

	std::string notation = mNotation.toSAN(promoMove, mBoard, false, false);
	EXPECT_EQ(notation, "e1=Q") << "Black pawn promotion should use same format";
}

} // namespace MoveTests
