/*
  ==============================================================================
	Module:         BitboardUtils
	Description:    Bit manipulation utility functions
  ==============================================================================
*/

#pragma once

#include <bit>
#include "BitboardTypes.h"


namespace BitUtils
{

inline int popCount(U64 bb)
{
	return std::popcount(bb);
}

inline int lsb(U64 bb)
{
	if (!bb)
		return -1;
	return std::countr_zero(bb);
}

inline constexpr bool getBit(U64 bb, int sq)
{
	return bb & (1ULL << sq);
}

inline constexpr void setBit(U64 &bb, int sq)
{
	bb |= (1ULL << sq);
}

inline constexpr void popBit(U64 &bb, int sq)
{
	bb &= ~(1ULL << sq);
}

inline constexpr int countBits(U64 bitboard)
{
	int count = 0;

	while (bitboard)
	{
		count++;
		bitboard &= bitboard - 1;
	}

	return count;
}


} // namespace BitUtils
