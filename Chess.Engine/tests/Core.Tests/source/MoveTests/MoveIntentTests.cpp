/*
  ==============================================================================
	Module:			MoveIntent Tests
	Description:    Testing the MoveIntent structures
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Moves/Move.h"


namespace MoveTests
{

class MoveIntentTests : public ::testing::Test
{
};


TEST_F(MoveIntentTests, DefaultConstruction)
{
	MoveIntent intent;

	EXPECT_EQ(intent.fromSquare, Square::None) << "Default from should be None";
	EXPECT_EQ(intent.toSquare, Square::None) << "Default to should be None";
	EXPECT_EQ(intent.promotion, PieceType::None) << "Default promotion should be None";
	EXPECT_FALSE(intent.fromRemote) << "Default fromRemote should be false";
	EXPECT_FALSE(intent.fromCPU) << "Default fromCPU should be false";
	EXPECT_TRUE(intent.legalMoves.empty()) << "Default legal moves should be empty";
}


TEST_F(MoveIntentTests, HasSource)
{
	MoveIntent intent;

	EXPECT_FALSE(intent.hasSource()) << "Should not have source initially";

	intent.fromSquare = Square::e2;
	EXPECT_TRUE(intent.hasSource()) << "Should have source after setting";
}


TEST_F(MoveIntentTests, HasTarget)
{
	MoveIntent intent;

	EXPECT_FALSE(intent.hasTarget()) << "Should not have target initially";

	intent.toSquare = Square::e4;
	EXPECT_TRUE(intent.hasTarget()) << "Should have target after setting";
}


TEST_F(MoveIntentTests, IsComplete)
{
	MoveIntent intent;

	EXPECT_FALSE(intent.isComplete()) << "Should not be complete initially";

	intent.fromSquare = Square::e2;
	EXPECT_FALSE(intent.isComplete()) << "Should not be complete with only source";

	intent.toSquare = Square::e4;
	EXPECT_TRUE(intent.isComplete()) << "Should be complete with source and target";
}


TEST_F(MoveIntentTests, Clear)
{
	MoveIntent intent;
	intent.fromSquare = Square::e2;
	intent.toSquare	  = Square::e4;
	intent.promotion  = PieceType::WQueen;
	intent.fromRemote = true;
	intent.fromCPU	  = true;
	intent.legalMoves.push(Move(Square::e2, Square::e4));

	intent.clear();

	EXPECT_EQ(intent.fromSquare, Square::None) << "From should be None after clear";
	EXPECT_EQ(intent.toSquare, Square::None) << "To should be None after clear";
	EXPECT_EQ(intent.promotion, PieceType::None) << "Promotion should be None after clear";
	EXPECT_FALSE(intent.fromRemote) << "fromRemote should be false after clear";
	EXPECT_FALSE(intent.fromCPU) << "fromCPU should be false after clear";
	EXPECT_TRUE(intent.legalMoves.empty()) << "Legal moves should be empty after clear";
}

} // namespace MoveTests
