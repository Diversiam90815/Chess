/*
  ==============================================================================
	Module:			Move Notation Tests
	Description:    Testing the MoveNotation class for Standard Algebraic Notation generation
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Notation/MoveNotation.h"

namespace GameMechanicTests
{


class MoveNotationHelperTests : public ::testing::Test
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

TEST_F(MoveNotationHelperTests, GetFileFromPosition)
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


TEST_F(MoveNotationHelperTests, GetRankFromPosition)
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


TEST_F(MoveNotationHelperTests, GetPositionString)
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


TEST_F(MoveNotationHelperTests, GetPieceType)
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


















}


