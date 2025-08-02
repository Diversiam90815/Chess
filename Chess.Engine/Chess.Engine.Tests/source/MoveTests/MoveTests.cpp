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


TEST_F(MoveTests, DefaultConstructor)
{
	Move move;

	EXPECT_EQ(move.startingPosition, Position{}) << "Default starting position should be {0,0}";
	EXPECT_EQ(move.endingPosition, Position{}) << "Default ending position should be {0,0}";
	EXPECT_EQ(move.movedPiece, PieceType::DefaultType) << "Default moved piece should be DefaultType";
	EXPECT_EQ(move.capturedPiece, PieceType::DefaultType) << "Default captured piece should be DefaultType";
	EXPECT_EQ(move.promotionType, PieceType::DefaultType) << "Default promotion should be DefaultType";
	EXPECT_EQ(move.player, PlayerColor::NoColor) << "Default player should be NoColor";
	EXPECT_EQ(move.type, MoveType::Normal) << "Default type should be Normal";
	EXPECT_EQ(move.notation, "") << "Default notation should be empty";
	EXPECT_EQ(move.number, 0) << "Default number should be 0";
	EXPECT_EQ(move.halfMoveClock, 0) << "Default half-move clock should be 0";
}


TEST_F(MoveTests, PossibleMoveConstructor)
{
	PossibleMove possibleMove{{4, 6}, {4, 4}, MoveType::DoublePawnPush, PieceType::Queen};
	Move		 move(possibleMove);

	EXPECT_EQ(move.startingPosition, possibleMove.start) << "Starting position should match PossibleMove";
	EXPECT_EQ(move.endingPosition, possibleMove.end) << "Ending position should match PossibleMove";
	EXPECT_EQ(move.type, possibleMove.type) << "Move type should match PossibleMove";
}


TEST_F(MoveTests, ParameterizedConstructor)
{
	Position  start{4, 6};
	Position  end{4, 4};
	PieceType moved		= PieceType::Pawn;
	PieceType captured	= PieceType::Rook;
	MoveType  type		= MoveType::Capture;
	PieceType promotion = PieceType::Queen;

	Move	  move(start, end, moved, captured, type, promotion);

	EXPECT_EQ(move.startingPosition, start) << "Starting position should be set correctly";
	EXPECT_EQ(move.endingPosition, end) << "Ending position should be set correctly";
	EXPECT_EQ(move.movedPiece, moved) << "Moved piece should be set correctly";
	EXPECT_EQ(move.capturedPiece, captured) << "Captured piece should be set correctly";
	EXPECT_EQ(move.type, type) << "Move type should be set correctly";
	EXPECT_EQ(move.promotionType, promotion) << "Promotion type should be set correctly";
}


TEST_F(MoveTests, DefaultParametersInConstructor)
{
	Position  start{4, 6};
	Position  end{4, 4};
	PieceType moved = PieceType::Pawn;

	Move	  move(start, end, moved);

	EXPECT_EQ(move.capturedPiece, PieceType::DefaultType) << "Default captured piece should be DefaultType";
	EXPECT_EQ(move.type, MoveType::Normal) << "Default move type should be Normal";
	EXPECT_EQ(move.promotionType, PieceType::DefaultType) << "Default promotion should be DefaultType";
}


TEST_F(MoveTests, ComparisonOperators)
{
	Move move1;
	move1.number = 5;

	Move move2;
	move2.number = 10;

	Move move3;
	move3.number = 5;

	// Test less than operator
	EXPECT_TRUE(move1 < move2) << "Move with smaller number should be less than move with larger number";
	EXPECT_FALSE(move2 < move1) << "Move with larger number should not be less than move with smaller number";
	EXPECT_FALSE(move1 < move3) << "Moves with same number should not be less than each other";

	// Test equality operator
	EXPECT_TRUE(move1 == move3) << "Moves with same number should be equal";
	EXPECT_FALSE(move1 == move2) << "Moves with different numbers should not be equal";
}


TEST_F(MoveTests, MoveInContainers)
{
	// Test that Move can be used in sorted containers (requires operator<)
	std::set<Move> moveSet;

	Move		   move1;
	move1.number = 1;

	Move move2;
	move2.number = 3;

	Move move3;
	move3.number = 2;

	moveSet.insert(move1);
	moveSet.insert(move2);
	moveSet.insert(move3);

	EXPECT_EQ(moveSet.size(), 3) << "Set should contain 3 moves";

	// Verify order
	auto it = moveSet.begin();
	EXPECT_EQ(it->number, 1) << "First move should have number 1";
	++it;
	EXPECT_EQ(it->number, 2) << "Second move should have number 2";
	++it;
	EXPECT_EQ(it->number, 3) << "Third move should have number 3";
}


TEST_F(MoveTests, PossibleMoveToMoveConversion)
{
	PossibleMove possibleMove{{1, 6}, {1, 4}, MoveType::DoublePawnPush};
	Move		 move(possibleMove);

	EXPECT_EQ(move.startingPosition, possibleMove.start) << "Start position should be converted correctly";
	EXPECT_EQ(move.endingPosition, possibleMove.end) << "End position should be converted correctly";
	EXPECT_EQ(move.type, possibleMove.type) << "Move type should be converted correctly";
}


TEST_F(MoveTests, MoveTypeFlags)
{
	// Test individual flags
	EXPECT_EQ(static_cast<int>(MoveType::Normal), 1) << "Normal move should be flag 1";
	EXPECT_EQ(static_cast<int>(MoveType::DoublePawnPush), 2) << "DoublePawnPush should be flag 2";
	EXPECT_EQ(static_cast<int>(MoveType::PawnPromotion), 4) << "PawnPromotion should be flag 4";
	EXPECT_EQ(static_cast<int>(MoveType::Capture), 8) << "Capture should be flag 8";
	EXPECT_EQ(static_cast<int>(MoveType::EnPassant), 16) << "EnPassant should be flag 16";
	EXPECT_EQ(static_cast<int>(MoveType::CastlingKingside), 32) << "CastlingKingside should be flag 32";
	EXPECT_EQ(static_cast<int>(MoveType::CastlingQueenside), 64) << "CastlingQueenside should be flag 64";
	EXPECT_EQ(static_cast<int>(MoveType::Check), 128) << "Check should be flag 128";
	EXPECT_EQ(static_cast<int>(MoveType::Checkmate), 256) << "Checkmate should be flag 256";
}


TEST_F(MoveTests, CombinedMoveTypes)
{
	// Test combining move types
	MoveType capturePromotion = static_cast<MoveType>(static_cast<int>(MoveType::Capture) | static_cast<int>(MoveType::PawnPromotion));
	MoveType checkCapture	  = static_cast<MoveType>(static_cast<int>(MoveType::Check) | static_cast<int>(MoveType::Capture));

	EXPECT_EQ(static_cast<int>(capturePromotion), 12) << "Capture + Promotion should be 8 + 4 = 12";
	EXPECT_EQ(static_cast<int>(checkCapture), 136) << "Check + Capture should be 128 + 8 = 136";
}


TEST_F(MoveTests, TestingMoveTypeFlags)
{
	MoveType combinedType = (MoveType::Capture | MoveType::Check);

	// Test if specific flags are set
	bool	 hasCapture	  = ((combinedType) & (MoveType::Capture)) == MoveType::Capture;
	bool	 hasCheck	  = ((combinedType) & (MoveType::Check)) == MoveType::Check;
	bool	 hasPromotion = ((combinedType) & (MoveType::PawnPromotion)) == MoveType::PawnPromotion;

	EXPECT_TRUE(hasCapture) << "Combined type should have Capture flag";
	EXPECT_TRUE(hasCheck) << "Combined type should have Check flag";
	EXPECT_FALSE(hasPromotion) << "Combined type should not have Promotion flag";
}


} // namespace MoveTests
