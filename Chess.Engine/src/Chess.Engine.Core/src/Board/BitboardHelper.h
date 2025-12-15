/*
  ==============================================================================
	Module:         BitboardHeloer
	Description:    Helper for handling the bitboard
  ==============================================================================
*/

#pragma once

#include <cstdint>


// Define bitboard data type
#define U64 std::uint64_t

enum Side
{
	White,
	Black
};

/*
	Bitboard representation (0ULL)

	8	0  0  0  0  0  0  0  0
	7	0  0  0  0  0  0  0  0
	6	0  0  0  0  0  0  0  0
	5	0  0  0  0  0  0  0  0
	4	0  0  0  0  0  0  0  0
	3	0  0  0  0  0  0  0  0
	2	0  0  0  0  0  0  0  0
	1	0  0  0  0  0  0  0  0

		a  b  c  d  e  f  g  h
*/


// Offboard constants

/*
	Example:	not A file:

	8	0  1  1  1  1  1  1  1
	7	0  1  1  1  1  1  1  1
	6	0  1  1  1  1  1  1  1
	5	0  1  1  1  1  1  1  1
	4	0  1  1  1  1  1  1  1
	3	0  1  1  1  1  1  1  1
	2	0  1  1  1  1  1  1  1
	1	0  1  1  1  1  1  1  1

		a  b  c  d  e  f  g  h
*/

constexpr U64 not_A_file  = 18374403900871474942ULL;
constexpr U64 not_H_file  = 9187201950435737471ULL;
constexpr U64 not_AB_file = 18229723555195321596ULL;
constexpr U64 not_HG_file = 4557430888798830399ULL;

// clang-format off

// relevant occupancy bit count for every square on board
constexpr int bishop_relevant_bits[64] = 
{
	6, 5, 5, 5, 5, 5, 5, 6, 
	5, 5, 5, 5, 5, 5, 5, 5, 
	5, 5, 7, 7, 7, 7, 5, 5, 
	5, 5, 7, 9, 9, 7, 5, 5,
	5, 5, 7, 9, 9, 7, 5, 5, 
	5, 5, 7, 7, 7, 7, 5, 5, 
	5, 5, 5, 5, 5, 5, 5, 5, 
	6, 5, 5, 5, 5, 5, 5, 6,
};

constexpr int rook_relevant_bits[64] = 
{
	12, 11, 11, 11, 11, 11, 11, 12,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	11, 10, 10, 10, 10, 10, 10, 11,
	12, 11, 11, 11, 11, 11, 11, 12,
};


// board squares
enum
{
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1,
};


inline const char* square_to_coordinates[] =
{
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};
// clang-format on


// Bit manipulation

#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)


// Count bits within a bitboard
static inline int countBits(U64 bitboard)
{
	int count = 0;

	// consecutively reset least significant 1st bit
	while (bitboard)
	{
		count++;				  // Increment count

		bitboard &= bitboard - 1; // Reset least significant 1st bit
	}

	return count;
}


// Get least significant 1st bit index
static inline int getLeastSignificantFirstBitIndex(U64 bitboard)
{
	if (!bitboard)
		return -1; // return illegal index if bitboard is null

	return countBits((bitboard & ~bitboard + 1) - 1);
}


static inline U64 setOccupancy(int index, int bitsInMask, U64 attackMask)
{
	U64 occupancy = 0ULL; // Occupancy map

	for (int count = 0; count < bitsInMask; ++count)
	{
		// get least significant 1st bit of attack mask
		int square = getLeastSignificantFirstBitIndex(attackMask);

		// pop least significant 1st bit in attack map
		pop_bit(attackMask, square);

		// make sure occupancy is on board
		if (index & (1 << count))
			occupancy |= (1ULL << square); // populate occupancy map
	}

	return occupancy;
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

enum
{
	rook,
	bishop
};
