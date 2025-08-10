/*
  ==============================================================================
	Module:			Possible Move Tests
	Description:    Testing the Possible Move structures
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Moves/Move.h"


namespace MoveTests
{


class PossibleMoveTests : public ::testing::Test
{
};


TEST_F(PossibleMoveTests, DefaultConstructor)
{
	PossibleMove move;

	EXPECT_EQ(move.start, Position({-1, -1})) << "Default start should be invalid position";
	EXPECT_EQ(move.end, Position({-1, -1})) << "Default end should be invalid position";
	EXPECT_EQ(move.type, MoveType::Normal) << "Default type should be Normal";
	EXPECT_EQ(move.promotionPiece, PieceType::DefaultType) << "Default promotion should be DefaultType";
}


TEST_F(PossibleMoveTests, ParameterizedInitialization)
{
	Position	 start{4, 6};
	Position	 end{4, 4};

	PossibleMove move{start, end, MoveType::DoublePawnPush, PieceType::Queen};

	EXPECT_EQ(move.start, start) << "Start position should be set correctly";
	EXPECT_EQ(move.end, end) << "End position should be set correctly";
	EXPECT_EQ(move.type, MoveType::DoublePawnPush) << "Move type should be set correctly";
	EXPECT_EQ(move.promotionPiece, PieceType::Queen) << "Promotion piece should be set correctly";
}


TEST_F(PossibleMoveTests, EqualityOperator)
{
	Position	 start{4, 6};
	Position	 end{4, 4};

	PossibleMove move1{start, end, MoveType::Normal};
	PossibleMove move2{start, end, MoveType::Capture}; // Different type
	PossibleMove move3{start, end, MoveType::Normal};

	EXPECT_TRUE(move1 == move3) << "Moves with same start and end should be equal";
	EXPECT_TRUE(move1 == move2) << "Move equality should only check start and end positions";

	PossibleMove move4{{3, 6}, end, MoveType::Normal};
	PossibleMove move5{start, {4, 5}, MoveType::Normal};

	EXPECT_FALSE(move1 == move4) << "Moves with different start should not be equal";
	EXPECT_FALSE(move1 == move5) << "Moves with different end should not be equal";
}


TEST_F(PossibleMoveTests, IsEmpty)
{
	PossibleMove emptyMove;
	PossibleMove validMove{{4, 6}, {4, 4}};
	PossibleMove partialMove{{4, 6}, {-1, -1}};

	EXPECT_TRUE(emptyMove.isEmpty()) << "Default constructed move should be empty";
	EXPECT_FALSE(validMove.isEmpty()) << "Move with valid positions should not be empty";
	EXPECT_FALSE(partialMove.isEmpty()) << "Move with one valid position should not be empty";
}


} // namespace MoveTests
