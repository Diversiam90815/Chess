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
		mRookMagicNumbers[square]	= findMagicNumber(square, rook_relevant_bits[square], rook);
		mBishopMagicNumbers[square] = findMagicNumber(square, bishop_relevant_bits[square], bishop);
	}
}


U64 Bitboard::findMagicNumber(int square, int relevantBits, int bishop)
{
	// init occupancies
	U64 occupancies[4096];

	// init attack tables
	U64 attacks[4096];

	// init used attacks
	U64 usedAttacks[4096];

	// init attack mask for a current piece
	U64 attackMask		   = bishop ? MoveHelper::maskBishopAttacks(square) : MoveHelper::maskRookAttacks(square);

	// init occupancy indicies
	int occupancyIndicies = 1 << relevantBits;

	// loop over occupancy indicies
	for (int index = 0; index < occupancyIndicies; index++)
	{
		// init occupancies
		occupancies[index] = setOccupancy(index, relevantBits, attackMask);

		// init attacks
		attacks[index]	   = bishop ? MoveHelper::generateBishopAttacks(square, occupancies[index]) : MoveHelper::generateRookAttacks(square, occupancies[index]);
	}

	// test magic numbers loop
	for (int randomCount = 0; randomCount < 100000000; randomCount++)
	{
		// generate magic number candidate
		U64 magicNumber = generateMagicNumbers();

		// skip inappropriate magic numbers
		if (countBits((attackMask * magicNumber) & 0xFF00000000000000) < 6)
			continue;

		// init used attacks
		memset(usedAttacks, 0ULL, sizeof(usedAttacks));

		// init index & fail flag
		int index, fail;

		// test magic index loop
		for (index = 0, fail = 0; !fail && index < occupancyIndicies; index++)
		{
			// init magic index
			int magicIndex = (int)((occupancies[index] * magicNumber) >> (64 - relevantBits));

			// if magic index works
			if (usedAttacks[magicIndex] == 0ULL)
				// init used attacks
				usedAttacks[magicIndex] = attacks[index];

			// otherwise
			else if (usedAttacks[magicIndex] != attacks[index])
				// magic index doesn't work
				fail = 1;
		}

		// if magic number works
		if (!fail)
			// return it
			return magicNumber;
	}

	// if magic number doesn't work
	printf("  Magic number fails!\n");
	return 0ULL;
}
