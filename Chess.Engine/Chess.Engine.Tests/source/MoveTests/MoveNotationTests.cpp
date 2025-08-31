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
	std::unique_ptr<MoveNotation>		mNotationHelper;

	void								SetUp() override { mNotationHelper = std::make_unique<MoveNotation>(); }

	void								TearDown() override { mNotationHelper.reset(); }

	// Helper to create a Move object with all necessary fields
	Move								CreateMove(Position	 start,
												   Position	 end,
												   PieceType movedPiece,
												   MoveType	 type			= MoveType::Normal,
												   PieceType capturedPiece	= PieceType::DefaultType,
												   PieceType promotionPiece = PieceType::DefaultType)
	{
		Move move(start, end, movedPiece, capturedPiece, type, promotionPiece);
		return move;
	}
};




// =============================================================================
// BASIC POSITION CONVERSION TESTS
// =============================================================================

TEST_F(MoveNotationTests, GetFileFromPosition)
{
	Position pos;

	// Test all files (a-h)
	pos = {0, 0}; // a-file
	EXPECT_EQ(mNotationHelper->getFileFromPosition(pos), 'a');

	pos = {1, 0}; // b-file
	EXPECT_EQ(mNotationHelper->getFileFromPosition(pos), 'b');

	pos = {2, 0}; // c-file
	EXPECT_EQ(mNotationHelper->getFileFromPosition(pos), 'c');

	pos = {3, 0}; // d-file
	EXPECT_EQ(mNotationHelper->getFileFromPosition(pos), 'd');

	pos = {4, 0}; // e-file
	EXPECT_EQ(mNotationHelper->getFileFromPosition(pos), 'e');

	pos = {5, 0}; // f-file
	EXPECT_EQ(mNotationHelper->getFileFromPosition(pos), 'f');

	pos = {6, 0}; // g-file
	EXPECT_EQ(mNotationHelper->getFileFromPosition(pos), 'g');

	pos = {7, 0}; // h-file
	EXPECT_EQ(mNotationHelper->getFileFromPosition(pos), 'h');
}


TEST_F(MoveNotationTests, GetRankFromPosition)
{
	Position pos;

	// Test all ranks (1-8)
	pos = {0, 7}; // rank 1
	EXPECT_EQ(mNotationHelper->getRankFromPosition(pos), '1');

	pos = {0, 6}; // rank 2
	EXPECT_EQ(mNotationHelper->getRankFromPosition(pos), '2');

	pos = {0, 5}; // rank 3
	EXPECT_EQ(mNotationHelper->getRankFromPosition(pos), '3');

	pos = {0, 4}; // rank 4
	EXPECT_EQ(mNotationHelper->getRankFromPosition(pos), '4');

	pos = {0, 3}; // rank 5
	EXPECT_EQ(mNotationHelper->getRankFromPosition(pos), '5');

	pos = {0, 2}; // rank 6
	EXPECT_EQ(mNotationHelper->getRankFromPosition(pos), '6');

	pos = {0, 1}; // rank 7
	EXPECT_EQ(mNotationHelper->getRankFromPosition(pos), '7');

	pos = {0, 0}; // rank 8
	EXPECT_EQ(mNotationHelper->getRankFromPosition(pos), '8');
}


TEST_F(MoveNotationTests, GetPositionString)
{
	Position pos;

	// Test various board positions
	pos = {0, 7}; // a1
	EXPECT_EQ(mNotationHelper->getPositionString(pos), "a1");

	pos = {7, 0}; // h8
	EXPECT_EQ(mNotationHelper->getPositionString(pos), "h8");

	pos = {4, 4}; // e4
	EXPECT_EQ(mNotationHelper->getPositionString(pos), "e4");

	pos = {3, 3}; // d5
	EXPECT_EQ(mNotationHelper->getPositionString(pos), "d5");

	pos = {2, 6}; // c2
	EXPECT_EQ(mNotationHelper->getPositionString(pos), "c2");
}


TEST_F(MoveNotationTests, GetPieceType)
{
	// Test piece type conversions
	EXPECT_EQ(mNotationHelper->getPieceType(PieceType::Pawn), 0); // Pawns have no letter
	EXPECT_EQ(mNotationHelper->getPieceType(PieceType::Knight), 'N');
	EXPECT_EQ(mNotationHelper->getPieceType(PieceType::Bishop), 'B');
	EXPECT_EQ(mNotationHelper->getPieceType(PieceType::Rook), 'R');
	EXPECT_EQ(mNotationHelper->getPieceType(PieceType::Queen), 'Q');
	EXPECT_EQ(mNotationHelper->getPieceType(PieceType::King), 'K');
	EXPECT_EQ(mNotationHelper->getPieceType(PieceType::DefaultType), 0);
}


// =============================================================================
// CASTLING NOTATION TESTS
// =============================================================================

TEST_F(MoveNotationTests, CastlingKingsideSAN)
{
	Move		kingsideCastling = CreateMove({4, 7}, {6, 7}, PieceType::King, MoveType::CastlingKingside);

	std::string notation		 = mNotationHelper->castlingToSAN(kingsideCastling);
	EXPECT_EQ(notation, "O-O") << "Kingside castling should be O-O";
}


TEST_F(MoveNotationTests, CastlingQueensideSAN)
{
	Move		queensideCastling = CreateMove({4, 7}, {2, 7}, PieceType::King, MoveType::CastlingQueenside);

	std::string notation		  = mNotationHelper->castlingToSAN(queensideCastling);
	EXPECT_EQ(notation, "O-O-O") << "Queenside castling should be O-O-O";
}


TEST_F(MoveNotationTests, CastlingInvalidMove)
{
	Move		regularMove = CreateMove({4, 7}, {4, 6}, PieceType::King, MoveType::Normal);

	std::string notation	= mNotationHelper->castlingToSAN(regularMove);
	EXPECT_EQ(notation, "") << "Non-castling move should return empty string";
}


TEST_F(MoveNotationTests, CastlingWithCheck)
{
	Move		castlingCheck = CreateMove({4, 7}, {6, 7}, PieceType::King, static_cast<MoveType>(MoveType::CastlingKingside | MoveType::Check));

	std::string notation	  = mNotationHelper->generateStandardAlgebraicNotation(castlingCheck);
	EXPECT_EQ(notation, "O-O+") << "Castling with check should include +";
}


// =============================================================================
// PAWN MOVE NOTATION TESTS
// =============================================================================

TEST_F(MoveNotationTests, SimplePawnMove)
{
	Move		pawnMove = CreateMove({4, 6}, {4, 4}, PieceType::Pawn, MoveType::Normal);

	std::string notation = mNotationHelper->generateStandardAlgebraicNotation(pawnMove);
	EXPECT_EQ(notation, "e4") << "Simple pawn move should be destination square only";
}


TEST_F(MoveNotationTests, PawnCapture)
{
	Move		pawnCapture = CreateMove({4, 4}, {3, 3}, PieceType::Pawn, MoveType::Capture, PieceType::Pawn);

	std::string notation	= mNotationHelper->generateStandardAlgebraicNotation(pawnCapture);
	EXPECT_EQ(notation, "exd5") << "Pawn capture should include starting file and 'x'";
}


TEST_F(MoveNotationTests, PawnCaptureFromDifferentFiles)
{
	// Test capture from different starting files
	Move		captureFromA = CreateMove({0, 4}, {1, 3}, PieceType::Pawn, MoveType::Capture, PieceType::Pawn);
	std::string notation	 = mNotationHelper->generateStandardAlgebraicNotation(captureFromA);
	EXPECT_EQ(notation, "axb5") << "Pawn capture from a-file should be axb5";

	Move captureFromH = CreateMove({7, 4}, {6, 3}, PieceType::Pawn, MoveType::Capture, PieceType::Pawn);
	notation		  = mNotationHelper->generateStandardAlgebraicNotation(captureFromH);
	EXPECT_EQ(notation, "hxg5") << "Pawn capture from h-file should be hxg5";
}


TEST_F(MoveNotationTests, PawnPromotion)
{
	Move		pawnPromotion = CreateMove({4, 1}, {4, 0}, PieceType::Pawn, MoveType::PawnPromotion, PieceType::DefaultType, PieceType::Queen);

	std::string notation	  = mNotationHelper->generateStandardAlgebraicNotation(pawnPromotion);
	EXPECT_EQ(notation, "e8=Q") << "Pawn promotion should include =Q";
}


TEST_F(MoveNotationTests, PawnPromotionWithCapture)
{
	Move pawnPromotionCapture = CreateMove({4, 1}, {3, 0}, PieceType::Pawn, static_cast<MoveType>(MoveType::PawnPromotion | MoveType::Capture), PieceType::Rook, PieceType::Knight);

	std::string notation	  = mNotationHelper->generateStandardAlgebraicNotation(pawnPromotionCapture);
	EXPECT_EQ(notation, "exd8=N") << "Pawn promotion with capture should include file, x, destination, and promotion";
}


TEST_F(MoveNotationTests, PawnPromotionToAllPieces)
{
	Position start = {4, 1}, end = {4, 0};

	Move	 promoteToQueen = CreateMove(start, end, PieceType::Pawn, MoveType::PawnPromotion, PieceType::DefaultType, PieceType::Queen);
	EXPECT_EQ(mNotationHelper->generateStandardAlgebraicNotation(promoteToQueen), "e8=Q");

	Move promoteToRook = CreateMove(start, end, PieceType::Pawn, MoveType::PawnPromotion, PieceType::DefaultType, PieceType::Rook);
	EXPECT_EQ(mNotationHelper->generateStandardAlgebraicNotation(promoteToRook), "e8=R");

	Move promoteToBishop = CreateMove(start, end, PieceType::Pawn, MoveType::PawnPromotion, PieceType::DefaultType, PieceType::Bishop);
	EXPECT_EQ(mNotationHelper->generateStandardAlgebraicNotation(promoteToBishop), "e8=B");

	Move promoteToKnight = CreateMove(start, end, PieceType::Pawn, MoveType::PawnPromotion, PieceType::DefaultType, PieceType::Knight);
	EXPECT_EQ(mNotationHelper->generateStandardAlgebraicNotation(promoteToKnight), "e8=N");
}


TEST_F(MoveNotationTests, EnPassantCapture)
{
	Move		enPassant = CreateMove({4, 3}, {3, 2}, PieceType::Pawn, MoveType::EnPassant, PieceType::Pawn);

	std::string notation  = mNotationHelper->generateStandardAlgebraicNotation(enPassant);
	EXPECT_EQ(notation, "exd6 e.p.") << "En passant should include e.p. notation";
}


// =============================================================================
// PIECE MOVE NOTATION TESTS
// =============================================================================

TEST_F(MoveNotationTests, KnightMove)
{
	Move		knightMove = CreateMove({1, 7}, {2, 5}, PieceType::Knight, MoveType::Normal);

	std::string notation   = mNotationHelper->generateStandardAlgebraicNotation(knightMove);
	EXPECT_EQ(notation, "Nc3") << "Knight move should include N and destination";
}


TEST_F(MoveNotationTests, BishopMove)
{
	Move		bishopMove = CreateMove({2, 7}, {5, 4}, PieceType::Bishop, MoveType::Normal);

	std::string notation   = mNotationHelper->generateStandardAlgebraicNotation(bishopMove);
	EXPECT_EQ(notation, "Bf4") << "Bishop move should include B and destination";
}


TEST_F(MoveNotationTests, RookMove)
{
	Move		rookMove = CreateMove({0, 7}, {0, 4}, PieceType::Rook, MoveType::Normal);

	std::string notation = mNotationHelper->generateStandardAlgebraicNotation(rookMove);
	EXPECT_EQ(notation, "Ra4") << "Rook move should include R and destination";
}


TEST_F(MoveNotationTests, QueenMove)
{
	Move		queenMove = CreateMove({3, 7}, {3, 4}, PieceType::Queen, MoveType::Normal);

	std::string notation  = mNotationHelper->generateStandardAlgebraicNotation(queenMove);
	EXPECT_EQ(notation, "Qd4") << "Queen move should include Q and destination";
}


TEST_F(MoveNotationTests, KingMove)
{
	Move		kingMove = CreateMove({4, 7}, {4, 6}, PieceType::King, MoveType::Normal);

	std::string notation = mNotationHelper->generateStandardAlgebraicNotation(kingMove);
	EXPECT_EQ(notation, "Ke2") << "King move should include K and destination";
}


// =============================================================================
// CAPTURE NOTATION TESTS
// =============================================================================

TEST_F(MoveNotationTests, KnightCapture)
{
	Move		knightCapture = CreateMove({2, 5}, {4, 4}, PieceType::Knight, MoveType::Capture, PieceType::Pawn);

	std::string notation	  = mNotationHelper->generateStandardAlgebraicNotation(knightCapture);
	EXPECT_EQ(notation, "Nxe4") << "Knight capture should include Nx and destination";
}


TEST_F(MoveNotationTests, BishopCapture)
{
	Move		bishopCapture = CreateMove({5, 4}, {2, 1}, PieceType::Bishop, MoveType::Capture, PieceType::Knight);

	std::string notation	  = mNotationHelper->generateStandardAlgebraicNotation(bishopCapture);
	EXPECT_EQ(notation, "Bxc7") << "Bishop capture should include Bx and destination";
}


TEST_F(MoveNotationTests, RookCapture)
{
	Move		rookCapture = CreateMove({0, 4}, {7, 4}, PieceType::Rook, MoveType::Capture, PieceType::Rook);

	std::string notation	= mNotationHelper->generateStandardAlgebraicNotation(rookCapture);
	EXPECT_EQ(notation, "Rxh4") << "Rook capture should include Rx and destination";
}


TEST_F(MoveNotationTests, QueenCapture)
{
	Move		queenCapture = CreateMove({3, 4}, {6, 1}, PieceType::Queen, MoveType::Capture, PieceType::Bishop);

	std::string notation	 = mNotationHelper->generateStandardAlgebraicNotation(queenCapture);
	EXPECT_EQ(notation, "Qxg7") << "Queen capture should include Qx and destination";
}


// =============================================================================
// CHECK AND CHECKMATE NOTATION TESTS
// =============================================================================

TEST_F(MoveNotationTests, MoveWithCheck)
{
	Move		checkMove = CreateMove({3, 4}, {3, 0}, PieceType::Queen, MoveType::Check);

	std::string notation  = mNotationHelper->generateStandardAlgebraicNotation(checkMove);
	EXPECT_EQ(notation, "Qd8+") << "Move giving check should end with +";
}


TEST_F(MoveNotationTests, MoveWithCheckmate)
{
	Move		checkmateMove = CreateMove({3, 4}, {5, 1}, PieceType::Queen, MoveType::Checkmate);

	std::string notation	  = mNotationHelper->generateStandardAlgebraicNotation(checkmateMove);
	EXPECT_EQ(notation, "Qf7#") << "Move giving checkmate should end with #";
}


TEST_F(MoveNotationTests, PawnMoveWithCheck)
{
	Move		pawnCheck = CreateMove({4, 2}, {4, 1}, PieceType::Pawn, MoveType::Check);

	std::string notation  = mNotationHelper->generateStandardAlgebraicNotation(pawnCheck);
	EXPECT_EQ(notation, "e7+") << "Pawn move giving check should end with +";
}


TEST_F(MoveNotationTests, CaptureWithCheckmate)
{
	Move		captureCheckmate = CreateMove({3, 4}, {5, 1}, PieceType::Queen, static_cast<MoveType>(MoveType::Capture | MoveType::Checkmate), PieceType::Bishop);

	std::string notation		 = mNotationHelper->generateStandardAlgebraicNotation(captureCheckmate);
	EXPECT_EQ(notation, "Qxf7#") << "Capture giving checkmate should have x and #";
}


// =============================================================================
// EDGE CASES AND COMPLEX MOVES
// =============================================================================

TEST_F(MoveNotationTests, DoublePawnPush)
{
	Move		doublePawnPush = CreateMove({4, 6}, {4, 4}, PieceType::Pawn, MoveType::DoublePawnPush);

	std::string notation	   = mNotationHelper->generateStandardAlgebraicNotation(doublePawnPush);
	EXPECT_EQ(notation, "e4") << "Double pawn push should be same as normal pawn move notation";
}


TEST_F(MoveNotationTests, ComplexPromotionScenario)
{
	// Pawn capture with promotion and check
	Move complexPromotion =
		CreateMove({6, 1}, {7, 0}, PieceType::Pawn, static_cast<MoveType>(MoveType::PawnPromotion | MoveType::Capture | MoveType::Check), PieceType::Knight, PieceType::Queen);

	std::string notation = mNotationHelper->generateStandardAlgebraicNotation(complexPromotion);
	EXPECT_EQ(notation, "gxh8=Q+") << "Complex promotion should include all elements";
}


TEST_F(MoveNotationTests, EnPassantWithCheck)
{
	Move		enPassantCheck = CreateMove({4, 3}, {3, 2}, PieceType::Pawn, static_cast<MoveType>(MoveType::EnPassant | MoveType::Check), PieceType::Pawn);

	std::string notation	   = mNotationHelper->generateStandardAlgebraicNotation(enPassantCheck);
	EXPECT_EQ(notation, "exd6 e.p.+") << "En passant with check should include both notations";
}


// =============================================================================
// BOUNDARY AND ERROR HANDLING TESTS
// =============================================================================

TEST_F(MoveNotationTests, CornerSquareNotations)
{
	// Test all corner squares
	Move a1Move = CreateMove({0, 0}, {0, 7}, PieceType::Rook, MoveType::Normal);
	EXPECT_EQ(mNotationHelper->generateStandardAlgebraicNotation(a1Move), "Ra1");

	Move a8Move = CreateMove({0, 7}, {0, 0}, PieceType::Rook, MoveType::Normal);
	EXPECT_EQ(mNotationHelper->generateStandardAlgebraicNotation(a8Move), "Ra8");

	Move h1Move = CreateMove({7, 0}, {7, 7}, PieceType::Rook, MoveType::Normal);
	EXPECT_EQ(mNotationHelper->generateStandardAlgebraicNotation(h1Move), "Rh1");

	Move h8Move = CreateMove({7, 7}, {7, 0}, PieceType::Rook, MoveType::Normal);
	EXPECT_EQ(mNotationHelper->generateStandardAlgebraicNotation(h8Move), "Rh8");
}


TEST_F(MoveNotationTests, InvalidPieceType)
{
	Move		invalidMove = CreateMove({4, 4}, {4, 5}, PieceType::DefaultType, MoveType::Normal);

	std::string notation	= mNotationHelper->generateStandardAlgebraicNotation(invalidMove);
	EXPECT_EQ(notation, "e3") << "Invalid piece type should result in destination only";
}


}


