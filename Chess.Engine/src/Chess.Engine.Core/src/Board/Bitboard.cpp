/*
  ==============================================================================
	Module:         Bitboard
	Description:    A bitboard representing a chess board
  ==============================================================================
*/

#include "Bitboard.h"
#include <stdio.h>
#include <string.h>


void Bitboard::initMagicNumbers()
{
	// loop over 64 board squares
	for (int square = 0; square < 64; ++square)
	{
		// init rook magic numbers
		printf(" 0x%llxULL\n", findMagicNumber(square, rook_relevant_bits[square], rook));
	}
}


U64 Bitboard::findMagicNumber(int square, int relevantBits, int bishop)
{
	// init occupancies
	U64 occupancies[4096];

	// init attack tables
	U64 attacks[4096];

	// init used attacks
	U64 used_attacks[4096];

	// init attack mask for a current piece
	U64 attack_mask		   = bishop ? MoveHelper::maskBishopAttacks(square) : MoveHelper::maskRookAttacks(square);

	// init occupancy indicies
	int occupancy_indicies = 1 << relevantBits;

	// loop over occupancy indicies
	for (int index = 0; index < occupancy_indicies; index++)
	{
		// init occupancies
		occupancies[index] = setOccupancy(index, relevantBits, attack_mask);

		// init attacks
		attacks[index]	   = bishop ? MoveHelper::generateBishopAttacks(square, occupancies[index]) : MoveHelper::generateRookAttacks(square, occupancies[index]);
	}

	// test magic numbers loop
	for (int random_count = 0; random_count < 100000000; random_count++)
	{
		// generate magic number candidate
		U64 magic_number = generateMagicNumbers();

		// skip inappropriate magic numbers
		if (countBits((attack_mask * magic_number) & 0xFF00000000000000) < 6)
			continue;

		// init used attacks
		memset(used_attacks, 0ULL, sizeof(used_attacks));

		// init index & fail flag
		int index, fail;

		// test magic index loop
		for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++)
		{
			// init magic index
			int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevantBits));

			// if magic index works
			if (used_attacks[magic_index] == 0ULL)
				// init used attacks
				used_attacks[magic_index] = attacks[index];

			// otherwise
			else if (used_attacks[magic_index] != attacks[index])
				// magic index doesn't work
				fail = 1;
		}

		// if magic number works
		if (!fail)
			// return it
			return magic_number;
	}

	// if magic number doesn't work
	printf("  Magic number fails!\n");
	return 0ULL;
}
