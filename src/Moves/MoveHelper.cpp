/*
  ==============================================================================

	Class:          MoveHelper

	Description:    Helper class for generating general moves in chess

  ==============================================================================
*/


#include "MoveHelper.h"


MoveHelper::MoveHelper()
{
}


MoveHelper::~MoveHelper()
{
}


bool MoveHelper::checkAvailableMoves(PieceType piece)
{
	return false;
}


std::vector<Move> MoveHelper::getAvailableMoves()
{
	return std::vector<Move>();
}


bool MoveHelper::checkDiagonalMoves()
{
	return false;
}


bool MoveHelper::checkAdjacentMoves()
{
	return false;
}


bool MoveHelper::checkLinearForwardMove()
{
	return false;
}


bool MoveHelper::checkPawnCaptureMoves()
{
	return false;
}


bool MoveHelper::checkLShapedMoves()
{
	return false;
}
