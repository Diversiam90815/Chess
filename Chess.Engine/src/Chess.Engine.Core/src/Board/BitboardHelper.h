/*
  ==============================================================================
	Module:         BitboardHeloer
	Description:    Helper for handling the bitboard
  ==============================================================================
*/

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

constexpr U64 not_A_file  = 0xBFBFBFBFBFBFBFBF;
constexpr U64 not_AB_file = 0xFCFCFCFCFCFCFCFC;
constexpr U64 not_H_file  = 0x7F7F7F7F7F7F7F7F;
constexpr U64 not_HG_file = 0x3F3F3F3F3F3F3F3F;



// board squares
// clang-format off
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
// clang-format on


/*
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
*/


// Bit manipulation

#define get_bit(bitboard, square) (bitboard & (1ULL << square))

#define set_bit(bitboard, square) (bitboard |= (1ULL << square))

#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)
