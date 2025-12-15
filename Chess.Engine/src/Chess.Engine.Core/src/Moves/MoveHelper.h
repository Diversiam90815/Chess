/*
  ==============================================================================
	Module:         MoveHelper
	Description:    Helper class for generating general moves in chess
  ==============================================================================
*/

#pragma once

#include "BitboardHelper.h"

class MoveHelper
{
public:
	MoveHelper()  = default;
	~MoveHelper() = default;

	void	   initLeaperAttacks();

	static U64 maskPawnAttacks(int side, int square);
	static U64 maskKnightAttacks(int square);
	static U64 maskKingAttacks(int square);
	static U64 maskBishopAttacks(int square);
	static U64 maskRookAttacks(int square);

	static U64 generateRookAttacks(int square, U64 blocker);
	static U64 generateBishopAttacks(int square, U64 blocker);


	U64		   mPawnAttacks[2][64]; // pawn attack table [side][square]
	U64		   mKnightAttacks[64];	// knight attack table [square]
	U64		   mKingAttacks[64];	// Kings attack table [square]
};