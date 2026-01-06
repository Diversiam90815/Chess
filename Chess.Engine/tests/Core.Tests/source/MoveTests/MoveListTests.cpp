/*
  ==============================================================================
	Module:			MoveList Tests
	Description:    Testing the MoveList structures
  ==============================================================================
*/


#include <gtest/gtest.h>

#include "Moves/Move.h"


namespace MoveTests
{

class MoveListTests : public ::testing::Test
{
};


TEST_F(MoveListTests, DefaultConstructorCreatesEmptyList)
{
	MoveList list;

	EXPECT_EQ(list.size(), 0) << "Default list should be empty";
	EXPECT_TRUE(list.empty()) << "Default list should report empty";
}


TEST_F(MoveListTests, PushAddsMove)
{
	MoveList list;
	Move	 move(Square::e2, Square::e4, MoveFlag::DoublePawnPush);

	list.push(move);

	EXPECT_EQ(list.size(), 1) << "List should have one element after push";
	EXPECT_FALSE(list.empty()) << "List should not be empty after push";
}


TEST_F(MoveListTests, ClearRemovesAllMoves)
{
	MoveList list;
	list.push(Move(Square::e2, Square::e4));
	list.push(Move(Square::d2, Square::d4));
	list.push(Move(Square::c2, Square::c4));

	list.clear();

	EXPECT_EQ(list.size(), 0) << "List should be empty after clear";
	EXPECT_TRUE(list.empty()) << "List should report empty after clear";
}


TEST_F(MoveListTests, IndexOperator)
{
	MoveList list;
	Move	 move1(Square::e2, Square::e4);
	Move	 move2(Square::d2, Square::d4);

	list.push(move1);
	list.push(move2);

	EXPECT_EQ(list[0], move1) << "First element should be move1";
	EXPECT_EQ(list[1], move2) << "Second element should be move2";
}


TEST_F(MoveListTests, ConstIndexOperator)
{
	MoveList list;
	list.push(Move(Square::e2, Square::e4));
	list.push(Move(Square::d2, Square::d4));

	const MoveList &constList = list;

	EXPECT_EQ(constList[0].from(), Square::e2) << "Const access should work";
	EXPECT_EQ(constList[1].from(), Square::d2) << "Const access should work";
}


TEST_F(MoveListTests, IteratorSupport)
{
	MoveList list;
	list.push(Move(Square::e2, Square::e4));
	list.push(Move(Square::d2, Square::d4));
	list.push(Move(Square::c2, Square::c4));

	int count = 0;
	for (const auto &move : list)
	{
		EXPECT_TRUE(move.isValid()) << "Each move in iteration should be valid";
		++count;
	}

	EXPECT_EQ(count, 3) << "Should iterate over 3 moves";
}


TEST_F(MoveListTests, BeginEndPointers)
{
	MoveList list;
	list.push(Move(Square::e2, Square::e4));
	list.push(Move(Square::d2, Square::d4));

	EXPECT_EQ(list.end() - list.begin(), 2) << "Distance between begin and end should be size";
}


TEST_F(MoveListTests, MaxCapacity)
{
	// MoveList should handle up to MAX_MOVES (256)
	MoveList list;

	// Push many moves (not all 256, just verify it handles a reasonable number)
	for (int i = 0; i < 100; ++i)
	{
		list.push(Move(Square::e2, Square::e4));
	}

	EXPECT_EQ(list.size(), 100) << "List should hold 100 moves";
}


} // namespace MoveTests
