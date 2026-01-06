/*
  ==============================================================================
	Module:			Move Tests
	Description:    Testing the Move structures
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Moves/Move.h"


namespace MoveTests
{

class MoveTests : public ::testing::Test
{
};


TEST_F(MoveTests, DefaultConstructorCreatesInvalidMove)
{
	Move move;

	EXPECT_FALSE(move.isValid()) << "Default constructed move should be invalid";
	EXPECT_EQ(move.raw(), 0) << "Default move should have raw value of 0";
}


TEST_F(MoveTests, ConstructorWithSquaresAndFlags)
{
	Move move(Square::e2, Square::e4, MoveFlag::DoublePawnPush);

	EXPECT_EQ(move.from(), Square::e2) << "From square should be e2";
	EXPECT_EQ(move.to(), Square::e4) << "To square should be e4";
	EXPECT_EQ(move.flags(), MoveFlag::DoublePawnPush) << "Flags should be DoublePawnPush";
	EXPECT_TRUE(move.isValid()) << "Move should be valid";
}


TEST_F(MoveTests, ConstructorWithDefaultFlags)
{
	Move move(Square::e2, Square::e3);

	EXPECT_EQ(move.from(), Square::e2) << "From square should be e2";
	EXPECT_EQ(move.to(), Square::e3) << "To square should be e3";
	EXPECT_EQ(move.flags(), MoveFlag::Quiet) << "Default flags should be Quiet";
	EXPECT_TRUE(move.isQuiet()) << "Move should be quiet";
}


TEST_F(MoveTests, ConstructorFromRawData)
{
	// Create a move and get its raw data
	Move	 original(Square::d2, Square::d4, MoveFlag::DoublePawnPush);
	uint16_t rawData = original.raw();

	// Construct from raw data
	Move	 reconstructed(rawData);

	EXPECT_EQ(reconstructed.from(), Square::d2) << "Reconstructed from should match";
	EXPECT_EQ(reconstructed.to(), Square::d4) << "Reconstructed to should match";
	EXPECT_EQ(reconstructed.flags(), MoveFlag::DoublePawnPush) << "Reconstructed flags should match";
}


TEST_F(MoveTests, IsCapture)
{
	Move quietMove(Square::e2, Square::e3, MoveFlag::Quiet);
	Move captureMove(Square::e4, Square::d5, MoveFlag::Capture);
	Move enPassantMove(Square::e5, Square::d6, MoveFlag::EnPassant);
	Move promoCapture(Square::e7, Square::d8, MoveFlag::QueenPromoCapture);

	EXPECT_FALSE(quietMove.isCapture()) << "Quiet move should not be capture";
	EXPECT_TRUE(captureMove.isCapture()) << "Capture move should be capture";
	EXPECT_TRUE(enPassantMove.isCapture()) << "En passant should be capture";
	EXPECT_TRUE(promoCapture.isCapture()) << "Promotion capture should be capture";
}


TEST_F(MoveTests, IsPromotion)
{
	Move quietMove(Square::e2, Square::e3, MoveFlag::Quiet);
	Move knightPromo(Square::e7, Square::e8, MoveFlag::KnightPromotion);
	Move bishopPromo(Square::e7, Square::e8, MoveFlag::BishopPromotion);
	Move rookPromo(Square::e7, Square::e8, MoveFlag::RookPromotion);
	Move queenPromo(Square::e7, Square::e8, MoveFlag::QueenPromotion);
	Move promoCapture(Square::e7, Square::d8, MoveFlag::QueenPromoCapture);

	EXPECT_FALSE(quietMove.isPromotion()) << "Quiet move should not be promotion";
	EXPECT_TRUE(knightPromo.isPromotion()) << "Knight promotion should be promotion";
	EXPECT_TRUE(bishopPromo.isPromotion()) << "Bishop promotion should be promotion";
	EXPECT_TRUE(rookPromo.isPromotion()) << "Rook promotion should be promotion";
	EXPECT_TRUE(queenPromo.isPromotion()) << "Queen promotion should be promotion";
	EXPECT_TRUE(promoCapture.isPromotion()) << "Promotion capture should be promotion";
}


TEST_F(MoveTests, IsCastle)
{
	Move quietMove(Square::e2, Square::e3, MoveFlag::Quiet);
	Move kingCastle(Square::e1, Square::g1, MoveFlag::KingCastle);
	Move queenCastle(Square::e1, Square::c1, MoveFlag::QueenCastle);

	EXPECT_FALSE(quietMove.isCastle()) << "Quiet move should not be castle";
	EXPECT_TRUE(kingCastle.isCastle()) << "King castle should be castle";
	EXPECT_TRUE(queenCastle.isCastle()) << "Queen castle should be castle";
}


TEST_F(MoveTests, IsEnPassant)
{
	Move quietMove(Square::e2, Square::e3, MoveFlag::Quiet);
	Move captureMove(Square::e4, Square::d5, MoveFlag::Capture);
	Move enPassantMove(Square::e5, Square::d6, MoveFlag::EnPassant);

	EXPECT_FALSE(quietMove.isEnPassant()) << "Quiet move should not be en passant";
	EXPECT_FALSE(captureMove.isEnPassant()) << "Regular capture should not be en passant";
	EXPECT_TRUE(enPassantMove.isEnPassant()) << "En passant move should be en passant";
}


TEST_F(MoveTests, IsDoublePush)
{
	Move quietMove(Square::e2, Square::e3, MoveFlag::Quiet);
	Move doublePush(Square::e2, Square::e4, MoveFlag::DoublePawnPush);

	EXPECT_FALSE(quietMove.isDoublePush()) << "Quiet move should not be double push";
	EXPECT_TRUE(doublePush.isDoublePush()) << "Double pawn push should be double push";
}


TEST_F(MoveTests, PromotionPieceOffset)
{
	Move knightPromo(Square::e7, Square::e8, MoveFlag::KnightPromotion);
	Move bishopPromo(Square::e7, Square::e8, MoveFlag::BishopPromotion);
	Move rookPromo(Square::e7, Square::e8, MoveFlag::RookPromotion);
	Move queenPromo(Square::e7, Square::e8, MoveFlag::QueenPromotion);

	// Offset: 0=Knight, 1=Bishop, 2=Rook, 3=Queen
	EXPECT_EQ(knightPromo.promotionPieceOffset(), 0) << "Knight promotion offset should be 0";
	EXPECT_EQ(bishopPromo.promotionPieceOffset(), 1) << "Bishop promotion offset should be 1";
	EXPECT_EQ(rookPromo.promotionPieceOffset(), 2) << "Rook promotion offset should be 2";
	EXPECT_EQ(queenPromo.promotionPieceOffset(), 3) << "Queen promotion offset should be 3";
}


TEST_F(MoveTests, PromotionCaptureOffset)
{
	Move knightPromoCapture(Square::e7, Square::d8, MoveFlag::KnightPromoCapture);
	Move bishopPromoCapture(Square::e7, Square::d8, MoveFlag::BishopPromoCapture);
	Move rookPromoCapture(Square::e7, Square::d8, MoveFlag::RookPromoCapture);
	Move queenPromoCapture(Square::e7, Square::d8, MoveFlag::QueenPromoCapture);

	EXPECT_EQ(knightPromoCapture.promotionPieceOffset(), 0) << "Knight promo capture offset should be 0";
	EXPECT_EQ(bishopPromoCapture.promotionPieceOffset(), 1) << "Bishop promo capture offset should be 1";
	EXPECT_EQ(rookPromoCapture.promotionPieceOffset(), 2) << "Rook promo capture offset should be 2";
	EXPECT_EQ(queenPromoCapture.promotionPieceOffset(), 3) << "Queen promo capture offset should be 3";
}


TEST_F(MoveTests, EqualityOperator)
{
	Move move1(Square::e2, Square::e4, MoveFlag::DoublePawnPush);
	Move move2(Square::e2, Square::e4, MoveFlag::DoublePawnPush);
	Move move3(Square::d2, Square::d4, MoveFlag::DoublePawnPush);
	Move move4(Square::e2, Square::e4, MoveFlag::Quiet);

	EXPECT_TRUE(move1 == move2) << "Identical moves should be equal";
	EXPECT_FALSE(move1 == move3) << "Moves with different squares should not be equal";
	EXPECT_FALSE(move1 == move4) << "Moves with different flags should not be equal";
}


TEST_F(MoveTests, InequalityOperator)
{
	Move move1(Square::e2, Square::e4, MoveFlag::DoublePawnPush);
	Move move2(Square::e2, Square::e4, MoveFlag::DoublePawnPush);
	Move move3(Square::d2, Square::d4, MoveFlag::DoublePawnPush);

	EXPECT_FALSE(move1 != move2) << "Identical moves should not be unequal";
	EXPECT_TRUE(move1 != move3) << "Different moves should be unequal";
}


TEST_F(MoveTests, NoneStaticMethod)
{
	Move nullMove = Move::none();

	EXPECT_FALSE(nullMove.isValid()) << "None move should be invalid";
	EXPECT_EQ(nullMove.raw(), 0) << "None move should have raw value of 0";
}


TEST_F(MoveTests, AllSquaresCanBeEncoded)
{
	// Test that all 64 squares can be properly encoded and decoded
	for (int from = 0; from < 64; ++from)
	{
		for (int to = 0; to < 64; ++to)
		{
			if (from == to)
				continue; // Skip same square moves

			Square fromSq = static_cast<Square>(from);
			Square toSq	  = static_cast<Square>(to);
			Move   move(fromSq, toSq, MoveFlag::Quiet);

			EXPECT_EQ(move.from(), fromSq) << "From square encoding failed for " << from;
			EXPECT_EQ(move.to(), toSq) << "To square encoding failed for " << to;
		}
	}
}


} // namespace MoveTests
