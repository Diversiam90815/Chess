/*
  ==============================================================================

	Class:          MoveHelper

	Description:    Helper class for generating general moves in chess

  ==============================================================================
*/

#pragma once

#include <vector>
#include <utility>
#include <array>

#include "Move.h"


enum MoveType
{
	Diagonal = 1,  // Bishop, Queen
	LinearForward, // Pawns moving
	Adjacent,	   // King moving one step in any direction
	Vertical,	   // Rook, Queen moving along vertical lines (files)
	LShaped		   // Knight's jump
};


class MoveHelper
{
public:
	MoveHelper();

	~MoveHelper();


	bool checkAvailableMoves(PieceType piece);

	std::vector<Move> getAvailableMoves();


private:
	bool							   checkDiagonalMoves();

	bool							   checkAdjacentMoves();

	bool							   checkLinearForwardMove();

	bool							   checkPawnCaptureMoves();

	bool							   checkLShapedMoves();


	std::array<std::pair<int, int>, 8> mAdjacentPositions  = {{{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}}};

	std::array<std::pair<int, int>, 4> mDiagonalDirections = {{{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}};

	std::array<std::pair<int, int>, 8> mLShapedDirections  = {{{1, 2}, {2, 1}, {-1, 2}, {-2, 1}, {1, -2}, {2, -1}, {-1, -2}, {-2, -1}}};


	std::vector<Move>				   mPossibleMovesAndCaptures;
};
