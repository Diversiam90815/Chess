/*
  ==============================================================================
	Module:			Position Tests
	Description:    Testing the Position structures
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Moves/Move.h"
#include <unordered_set>
#include <unordered_map>


namespace MoveTests
{

class PositionTests : public ::testing::Test
{
};


TEST_F(PositionTests, DefaultConstructor)
{
	Position pos;

	EXPECT_EQ(pos.x, 0) << "Default x coordinate should be 0";
	EXPECT_EQ(pos.y, 0) << "Default y coordinate should be 0";
}


TEST_F(PositionTests, ParameterizedConstructor)
{
	Position pos{4, 6};

	EXPECT_EQ(pos.x, 4) << "X coordinate should be set correctly";
	EXPECT_EQ(pos.y, 6) << "Y coordinate should be set correctly";
}


TEST_F(PositionTests, IsValid)
{
	// valid positions
	Position a8			  = {0, 0};
	Position h1			  = {7, 7};
	Position d4			  = {3, 4};

	// invalid positions
	Position negativeX	  = {-1, 0};
	Position negativeY	  = {0, -1};
	Position boardLimitX  = {0, BOARD_SIZE};
	Position boardLimitY  = {BOARD_SIZE, 0};
	Position negativeXY	  = {-5, -2};
	Position outOfBoundXY = {10, 10};

	EXPECT_TRUE(a8.isValid()) << "a8 should be valid";
	EXPECT_TRUE(h1.isValid()) << "h1 should be valid";
	EXPECT_TRUE(d4.isValid()) << "d4 should be valid";

	EXPECT_FALSE(negativeX.isValid()) << "Negative x should be invalid";
	EXPECT_FALSE(negativeY.isValid()) << "Negative y should be invalid";
	EXPECT_FALSE(boardLimitX.isValid()) << "x >= BOARD_SIZE should be invalid";
	EXPECT_FALSE(boardLimitY.isValid()) << "y >= BOARD_SIZE should be invalid";
	EXPECT_FALSE(negativeXY.isValid()) << "Both negative coordinates should be invalid";
	EXPECT_FALSE(outOfBoundXY.isValid()) << "Both coordinates out of bounds should be invalid";
}


TEST_F(PositionTests, EqualityOperator)
{
	Position pos1{4, 6};
	Position pos2{4, 6};
	Position pos3{3, 6};
	Position pos4{4, 5};

	EXPECT_TRUE(pos1 == pos2) << "Same positions should be equal";
	EXPECT_FALSE(pos1 == pos3) << "Different x coordinates should not be equal";
	EXPECT_FALSE(pos1 == pos4) << "Different y coordinates should not be equal";
}


TEST_F(PositionTests, HashFunction)
{
	Position			pos1{4, 6};
	Position			pos2{4, 6};
	Position			pos3{6, 4};

	std::hash<Position> hasher;

	// Test hash consistency
	EXPECT_EQ(hasher(pos1), hasher(pos2)) << "Same positions should have same hash";

	// Test hash distribution (different positions should likely have different hashes)
	EXPECT_NE(hasher(pos1), hasher(pos3)) << "Different positions should have different hashes";
}


TEST_F(PositionTests, HashInContainers)
{
	// Test Position can be used in hash-based containers
	std::unordered_set<Position>	  posSet;
	std::unordered_map<Position, int> posMap;

	Position						  pos1{4, 6};
	Position						  pos2{3, 5};

	posSet.insert(pos1);
	posSet.insert(pos2);
	EXPECT_EQ(posSet.size(), 2) << "Set should contain 2 unique positions";

	posMap[pos1] = 10;
	posMap[pos2] = 20;
	EXPECT_EQ(posMap.size(), 2) << "Map should contain 2 entries";
	EXPECT_EQ(posMap[pos1], 10) << "Map should store values correctly";
}


} // namespace MoveTests
