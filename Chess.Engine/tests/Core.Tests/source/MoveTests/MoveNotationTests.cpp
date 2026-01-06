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
	MoveNotation mNotation{mBoard};

	void		 SetUp() override { mBoard.init(); }
};




} // namespace MoveTests
