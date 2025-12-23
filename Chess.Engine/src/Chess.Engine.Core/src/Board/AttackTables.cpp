/*
  ==============================================================================
	Module:         AttackTables
	Description:    Precomputed attack tables for all pieces
  ==============================================================================
*/

#include "AttackTables.h"


void AttackTables::initLeaperAttacks()
{
	for (int square = 0; square < 64; ++square)
	{
		mPawnAttacks[to_index(Side::White)][square] = maskPawnAttacks(Side::White, square);
		mPawnAttacks[to_index(Side::Black)][square] = maskPawnAttacks(Side::Black, square);

		mKnightAttacks[square]						= maskKnightAttacks(square);

		mKingAttacks[square]						= maskKingAttacks(square);
	}
}


U64 AttackTables::maskPawnAttacks(Side side, int square)
{
	// result attacks bitboard
	U64 attacks	 = 0ULL;

	// piece bitboard
	U64 bitboard = 0ULL;

	// Set bit on board
	BitUtils::setBit(bitboard, square);

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


U64 AttackTables::maskKnightAttacks(int square)
{
	// result attacks bitboard
	U64 attacks	 = 0ULL;

	// piece bitboard
	U64 bitboard = 0ULL;

	// Set bit on board
	BitUtils::setBit(bitboard, square);

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


U64 AttackTables::maskKingAttacks(int square)
{
	// result attacks bitboard
	U64 attacks	 = 0ULL;

	// piece bitboard
	U64 bitboard = 0ULL;

	// Set bit on board
	BitUtils::setBit(bitboard, square);

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


U64 AttackTables::generateBishopAttacks(int square, U64 blocker)
{
	// result attacks bitboard
	U64 attacks = 0ULL;

	// init rank, files
	int r		= 0;
	int f		= 0;

	// init target rank & files
	int tr		= square / 8;
	int tf		= square % 8;

	// generate bishop attacks
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


U64 AttackTables::generateRookAttacks(int square, U64 blocker)
{
	// result attacks bitboard
	U64 attacks = 0ULL;

	// init rank, files
	int r		= 0;
	int f		= 0;

	// init target rank & files
	int tr		= square / 8;
	int tf		= square % 8;

	// generate rook attacks
	for (r = tr + 1; r <= 7; r++)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if ((1ULL << (r * 8 + tf)) & blocker)
			break;
	}

	for (r = tr - 1; r >= 0; r--)
	{
		attacks |= (1ULL << (r * 8 + tf));
		if ((1ULL << (r * 8 + tf)) & blocker)
			break;
	}

	for (f = tf + 1; f <= 7; f++)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if ((1ULL << (tr * 8 + f)) & blocker)
			break;
	}

	for (f = tf - 1; f >= 0; f--)
	{
		attacks |= (1ULL << (tr * 8 + f));
		if ((1ULL << (tr * 8 + f)) & blocker)
			break;
	}

	return attacks;
}


U64 AttackTables::maskBishopAttacks(int square)
{
	// result attacks bitboard
	U64 attacks = 0ULL;

	// init ranks & files
	int r, f;

	// init target rank & files
	int tr = square / 8;
	int tf = square % 8;

	// mask relevant bishop occupancy bits
	for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
		attacks |= (1ULL << (r * 8 + f));
	for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
		attacks |= (1ULL << (r * 8 + f));
	for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
		attacks |= (1ULL << (r * 8 + f));
	for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
		attacks |= (1ULL << (r * 8 + f));

	return attacks;
}


U64 AttackTables::maskRookAttacks(int square)
{
	// result attacks bitboard
	U64 attacks = 0ULL;

	// init ranks & files
	int r, f;

	// init target rank & files
	int tr = square / 8;
	int tf = square % 8;

	// mask relevant rook occupancy bits
	for (r = tr + 1; r <= 6; r++)
		attacks |= (1ULL << (r * 8 + tf));
	for (r = tr - 1; r >= 1; r--)
		attacks |= (1ULL << (r * 8 + tf));
	for (f = tf + 1; f <= 6; f++)
		attacks |= (1ULL << (tr * 8 + f));
	for (f = tf - 1; f >= 1; f--)
		attacks |= (1ULL << (tr * 8 + f));

	return attacks;
}


U64 AttackTables::setOccupancy(int index, int bitsInMask, U64 attackMask)
{
	U64 occupancy = 0ULL; // Occupancy map

	for (int count = 0; count < bitsInMask; ++count)
	{
		// get least significant 1st bit of attack mask
		int square = BitUtils::lsb(attackMask);

		// pop least significant 1st bit in attack map
		BitUtils::popBit(attackMask, square);

		// make sure occupancy is on board
		if (index & (1 << count))
			occupancy |= (1ULL << square); // populate occupancy map
	}

	return occupancy;
}


void AttackTables::initSliderAttacks(int bishop)
{
	// loop over all squares
	for (int square = 0; square < 64; ++square)
	{
		// init bishop & rook masks
		mBishopMasks[square]  = maskBishopAttacks(square);
		mRookMasks[square]	  = maskRookAttacks(square);

		// ini current maks
		U64 attackMask		  = bishop ? mBishopMasks[square] : mRookMasks[square];

		// init relevant occupancy bit count
		int relevantBitsCount = BitUtils::countBits(attackMask);

		// init occupancy indicies
		int occupancyIndicies = (1 << relevantBitsCount);

		// loop over occupancy indicies
		for (int index = 0; index < occupancyIndicies; ++index)
		{
			// bishop
			if (bishop)
			{
				// init current occupancy variation
				U64 occupancy					   = setOccupancy(index, relevantBitsCount, attackMask);

				// init magic index
				int magicIndex					   = (occupancy * mBishopMagicNumbers[square]) >> (64 - bishop_relevant_bits[square]);

				// init bishop attacks
				mBishopAttacks[square][magicIndex] = generateBishopAttacks(square, occupancy);
			}
			// rook
			else
			{
				// init current occupancy variation
				U64 occupancy					 = setOccupancy(index, relevantBitsCount, attackMask);

				// init magic index
				int magicIndex					 = (occupancy * mRookMagicNumbers[square]) >> (64 - rook_relevant_bits[square]);

				// init rook attacks
				mRookAttacks[square][magicIndex] = generateRookAttacks(square, occupancy);
			}
		}
	}
}


U64 AttackTables::getBishopAttacks(int square, U64 occupancy)
{
	// get bishop attacks assuming current board occupancy
	occupancy &= mBishopMasks[square];
	occupancy *= mBishopMagicNumbers[square];
	occupancy >>= 64 - bishop_relevant_bits[square];

	return mBishopAttacks[square][occupancy];
};


U64 AttackTables::getRookAttacks(int square, U64 occupancy)
{
	// get rook attacks assuming current board occupancy
	occupancy &= mRookMasks[square];
	occupancy *= mRookMagicNumbers[square];
	occupancy >>= 64 - rook_relevant_bits[square];

	return mRookAttacks[square][occupancy];
}


U64 AttackTables::getQueenAttacks(int square, U64 occupancy)
{
	// init result attacks
	U64 queenAttacks	= 0ULL;

	// init bishop & rook occupancies
	U64 bishopOccupancy = occupancy;
	U64 rookOccupancy	= occupancy;

	// get bishop attacks assuming current board occupancy
	bishopOccupancy &= mBishopMasks[square];
	bishopOccupancy *= mBishopMagicNumbers[square];
	bishopOccupancy >>= 64 - bishop_relevant_bits[square];

	queenAttacks = mBishopAttacks[square][bishopOccupancy];

	// get rook attacks assuming current board occupancy
	rookOccupancy &= mRookMasks[square];
	rookOccupancy *= mRookMagicNumbers[square];
	rookOccupancy >>= 64 - rook_relevant_bits[square];

	queenAttacks |= mRookAttacks[square][rookOccupancy];

	return queenAttacks;
}
