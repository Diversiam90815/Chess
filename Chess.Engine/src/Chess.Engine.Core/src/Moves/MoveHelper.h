/*
  ==============================================================================
	Module:         MoveHelper
	Description:    Helper class for generating general moves in chess
  ==============================================================================
*/

#pragma once

#include "Bitboard.h"

class MoveHelper
{
public:
	MoveHelper()  = default;
	~MoveHelper() = default;

	void initLeaperAttacks();

	U64	 maskPawnAttacks(int side, int square);
	U64	 maskKnightAttacks(int square);
	U64	 maskKingAttacks(int square);
	U64	 maskBishopAttacks(int square);
	U64	 maskRookAttacks(int square);

	U64	 mPawnAttacks[2][64]; // pawn attack table [side][square]
	U64	 mKnightAttacks[64];  // knight attack table [square]
	U64	 mKingAttacks[64];	  // Kings attack table [square]
};