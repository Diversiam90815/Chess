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


U64 MoveHelper::generateBishopAttacks(int square, U64 blocker)
{
	// result attacks bitboard
	U64 attacks = 0ULL;

	// init rank, files
	int r		= 0;
	int f		= 0;

	// init target rank & files
	int tr		= square / 8;
	int tf		= square % 8;

	// mask relevant bishop occupancy bits
	for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blocker)
			break;
	}

	for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blocker)
			break;
	}

	for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blocker)
			break;
	}

	for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
	{
		attacks |= (1ULL << (r * 8 + f));
		if ((1ULL << (r * 8 + f)) & blocker)
			break;
	}

	return attacks;
}


U64 MoveHelper::maskRookAttacks(int square)
{
	// result attacks bitboard
	U64 attacks = 0ULL;

	// init rank, files
	int r		= 0;
	int f		= 0;

	// init target rank & files
	int tr		= square / 8;
	int tf		= square % 8;

	// mask relevant rook occupancy bits
	for (r = tr + 1; r <= 6; r++)
		attacks |= (1ULL << (r * 8 + tf));

	for (r = tr - 1; r >= 6; r--)
		attacks |= (1ULL << (r * 8 + tf));

	for (f = tf + 1; f <= 6; f++)
		attacks |= (1ULL << (tr * 8 + f));

	for (f = tf - 1; f >= 1; f--)
		attacks |= (1ULL >> (tr * 8 + f));


	return attacks;
}
