/*
  ==============================================================================
	Module:			Move Generation Tests
	Description:    Testing the move generation module from the chess engine
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "MoveGeneration.h"
#include "MoveValidation.h"
#include "MoveExecution.h"

#include "TestHelper.h"


TEST(MoveGenerationTest, Construction)
{
	auto board		= CreateDefaultBoard();
	auto validation = std::make_shared<MoveValidation>(board);
	auto execution	= std::make_shared<MoveExecution>(board, validation);

	EXPECT_NO_THROW({ MoveGeneration generation(board, validation, execution); });
}


TEST(MoveGenerationTest, GetMovesForPositionReturnsLegalMoves)
{
	auto		   board	  = CreateDefaultBoard();
	auto		   validation = std::make_shared<MoveValidation>(board);
	auto		   execution  = std::make_shared<MoveExecution>(board, validation);

	MoveGeneration generation(board, validation, execution);

	// e2 pawn
	Position	   pawnPos = {4, 1};
	auto		   moves   = generation.getMovesForPosition(pawnPos);

	// Should have at least one move (e2-e3, e2-e4)
	EXPECT_FALSE(moves.empty());
}


TEST(MoveGenerationTest, CalculateAllLegalBasicMovesReturnsTrue)
{
	auto		   board	  = CreateDefaultBoard();
	auto		   validation = std::make_shared<MoveValidation>(board);
	auto		   execution  = std::make_shared<MoveExecution>(board, validation);

	MoveGeneration generation(board, validation, execution);

	EXPECT_TRUE(generation.calculateAllLegalBasicMoves(PlayerColor::White));
}