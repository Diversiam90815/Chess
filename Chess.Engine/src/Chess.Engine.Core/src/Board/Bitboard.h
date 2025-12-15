/*
  ==============================================================================
	Module:         Bitboard
	Description:    A bitboard representing a chess board
  ==============================================================================
*/

#pragma once

#include "BitboardHelper.h"
#include "MoveHelper.h"


class Bitboard
{
public:
	Bitboard()	= default;
	~Bitboard() = default;

	void initMagicNumbers();

private:
	U64		   findMagicNumber(int square, int relavantBits, int bishop);


	MoveHelper mMoveHelper;
};
