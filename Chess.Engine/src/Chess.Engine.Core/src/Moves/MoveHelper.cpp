/*
  ==============================================================================
	Module:         MoveHelper
	Description:    Helper class for generating general moves in chess
  ==============================================================================
*/


#include "MoveHelper.h"


void MoveHelper::initLeaperAttacks()
{
	for (int square = 0; square < 64; ++square)
	{
		mPawnAttacks[Side::White][square] = maskPawnAttacks(Side::White, square);
		mPawnAttacks[Side::Black][square] = maskPawnAttacks(Side::Black, square);

		mKnightAttacks[square]			  = maskKnightAttacks(square);

		mKingAttacks[square]			  = maskKingAttacks(square);
	}
}


U64 MoveHelper::maskPawnAttacks(int side, int square)
{
	// result attacks bitboard
	U64 attacks	 = 0ULL;

	// piece bitboard
	U64 bitboard = 0ULL;

	// Set bit on board
	set_bit(bitboard, square);

	// Generate attacks for
	// white pawns
	if (!(side == Side::White))
	{
		if ((bitboard >> 7) & not_A_file)
			attacks |= (bitboard >> 7);

		if ((bitboard >> 9) & not_H_file)
			attacks |= (bitboard >> 9);
	}
	// black pawns
	else
	{
		if ((bitboard << 7) & not_A_file)
			attacks |= (bitboard << 7);

		if ((bitboard << 9) & not_H_file)
			attacks |= (bitboard << 9);
	}

	return attacks;
}


U64 MoveHelper::maskKnightAttacks(int square)
{
	// result attacks bitboard
	U64 attacks	 = 0ULL;

	// piece bitboard
	U64 bitboard = 0ULL;

	// Set bit on board
	set_bit(bitboard, square);

	// generate knight attacks (Bitboard offsets 6, 10, 15, 17)
	if ((bitboard >> 17) & not_H_file)
		attacks |= (bitboard >> 17);
	if ((bitboard >> 15) & not_A_file)
		attacks |= (bitboard >> 15);
	if ((bitboard >> 10) & not_HG_file)
		attacks |= (bitboard >> 10);
	if ((bitboard >> 6) & not_AB_file)
		attacks |= (bitboard >> 6);

	if ((bitboard << 17) & not_A_file)
		attacks |= (bitboard << 17);
	if ((bitboard << 15) & not_H_file)
		attacks |= (bitboard << 15);
	if ((bitboard << 10) & not_AB_file)
		attacks |= (bitboard << 10);
	if ((bitboard << 6) & not_HG_file)
		attacks |= (bitboard << 6);

	return attacks;
}


U64 MoveHelper::maskKingAttacks(int square)
{
	// result attacks bitboard
	U64 attacks	 = 0ULL;

	// piece bitboard
	U64 bitboard = 0ULL;

	// Set bit on board
	set_bit(bitboard, square);

	// generate king attacks
	if (bitboard >> 8)
		attacks |= (bitboard >> 8);
	if ((bitboard >> 9) & not_H_file)
		attacks |= (bitboard >> 9);
	if ((bitboard >> 7) & not_A_file)
		attacks |= (bitboard >> 7);
	if ((bitboard >> 1) & not_H_file)
		attacks |= (bitboard >> 1);

	if (bitboard << 8)
		attacks |= (bitboard << 8);
	if ((bitboard << 9) & not_A_file)
		attacks |= (bitboard << 9);
	if ((bitboard << 7) & not_H_file)
		attacks |= (bitboard << 7);
	if ((bitboard << 1) & not_A_file)
		attacks |= (bitboard << 1);

	return attacks;
}
