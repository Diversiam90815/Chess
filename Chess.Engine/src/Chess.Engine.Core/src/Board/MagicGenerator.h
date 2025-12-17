/*
  ==============================================================================
	Module:         MagicGenerator
	Description:    Utility for finding magic numbers (development tool only)
  ==============================================================================
*/

#pragma once

#include <iostream>

#include "BitboardTypes.h"
#include "AttackTables.h"

#define CHESS_DEV_TOOLS


#ifdef CHESS_DEV_TOOLS

namespace MagicGenerator
{

static inline U64 findMagicNumber(int square, int relevantBits, int bishop)
{
	AttackTables attackTables;

	// init occupancies
	U64			 occupancies[4096];

	// init attack tables
	U64			 attacks[4096];

	// init used attacks
	U64			 usedAttacks[4096];

	// init attack mask for a current piece
	U64			 attackMask		   = bishop ? attackTables.maskBishopAttacks(square) : attackTables.maskRookAttacks(square);

	// init occupancy indicies
	int			 occupancyIndicies = 1 << relevantBits;

	// loop over occupancy indicies
	for (int index = 0; index < occupancyIndicies; index++)
	{
		// init occupancies
		occupancies[index] = attackTables.setOccupancy(index, relevantBits, attackMask);

		// init attacks
		attacks[index]	   = bishop ? attackTables.generateBishopAttacks(square, occupancies[index]) : attackTables.generateRookAttacks(square, occupancies[index]);
	}

	// test magic numbers loop
	for (int randomCount = 0; randomCount < 100000000; randomCount++)
	{
		// generate magic number candidate
		U64 magicNumber = generateMagicNumbers();

		// skip inappropriate magic numbers
		if (BitUtils::countBits((attackMask * magicNumber) & 0xFF00000000000000) < 6)
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


static inline unsigned int random_state = 1804289383;

// generate 32bit pseudo legal numbers
static inline int		   getRandomU32Number()
{
	unsigned int number = random_state;

	// XOR shift algorithm
	number ^= number << 13;
	number ^= number >> 17;
	number ^= number << 5;

	// update random number state
	random_state = number;

	return number;
}


static inline U64 getRandomU64Number()
{
	// define 4 random numbers
	U64 n1, n2, n3, n4;

	// init random numbers slicing 16 bits from MS1b
	n1 = (U64)(getRandomU32Number()) & 0xFFFF;
	n2 = (U64)(getRandomU32Number()) & 0xFFFF;
	n3 = (U64)(getRandomU32Number()) & 0xFFFF;
	n4 = (U64)(getRandomU32Number()) & 0xFFFF;

	// return random number
	return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}


static inline U64 generateMagicNumbers()
{
	return getRandomU64Number() & getRandomU64Number() & getRandomU64Number();
}


} // namespace MagicGenerator

#endif
