/*
  ==============================================================================
	Module:         BitboardTypes
	Description:    Core types, constants, and enums for bitboard operations
  ==============================================================================
*/

#pragma once

#include <cstdint>

/*
	Example: Bitboard representation (0ULL)

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
using U64				  = std::uint64_t;


/*
=========================================
		OFFBOARD CONSTANTS
=========================================
*/

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


/*
=========================================
		BOARD SQUARES
=========================================
*/

enum class Square
{
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1,
	None
};

constexpr int to_index(Square s) noexcept
{
	return static_cast<int>(s);
}


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



/*
=========================================
		PIECE TYPES
=========================================
*/

enum PieceType
{
	None	= -1,
	WKing	= 0,
	WQueen	= 1,
	WPawn	= 2,
	WKnight = 3,
	WBishop = 4,
	WRook	= 5,
	BKing	= 6,
	BQueen	= 7,
	BPawn	= 8,
	BKnight = 9,
	BBishop = 10,
	BRook	= 11
};

// ASCII pieces with the enum values as indecies
// (capital letters for white, small letters for black)
constexpr char		  asciiPieces[13]	= "KQPNBRkqpnbr";

constexpr const char *unicodePieces[12] = {"♚", "♛", "♟︎", "♞", "♝", "♜", "♔", "♕", "♙", "♘", "♗", "♖"};

// convert ASCII character pieces to encoded constants
inline int			  GetPieceTypeFromChar(char c)
{
	switch (c)
	{
	case 'K': return PieceType::WKing;
	case 'Q': return PieceType::WQueen;
	case 'P': return PieceType::WPawn;
	case 'N': return PieceType::WKnight;
	case 'B': return PieceType::WBishop;
	case 'R': return PieceType::WRook;
	case 'k': return PieceType::BKing;
	case 'q': return PieceType::BQueen;
	case 'p': return PieceType::BPawn;
	case 'n': return PieceType::BKnight;
	case 'b': return PieceType::BBishop;
	case 'r': return PieceType::BRook;
	default: return 0;
	}
}

/*
=========================================
		SIDE
=========================================
*/

enum class Side : int8_t
{
	None  = -1,
	White = 0,
	Black = 1,
	Both  = 2 // For all occupancies
};

constexpr int to_index(Side s) noexcept
{
	return static_cast<int>(s);
}


/*
=========================================
		CASTLING
=========================================
*/

/*
	bin  dec

   0001    1  white king can castle to the king side
   0010    2  white king can castle to the queen side
   0100    4  black king can castle to the king side
   1000    8  black king can castle to the queen side

   examples

   1111       both sides an castle both directions
   1001       black king => queen side
			  white king => king side
*/
enum class Castling : uint8_t
{
	None = 0,
	WK	 = 1 << 0,
	WQ	 = 1 << 1,
	BK	 = 1 << 2,
	BQ	 = 1 << 3
};

constexpr Castling operator|(Castling a, Castling b) noexcept
{
	return static_cast<Castling>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr Castling operator&(Castling a, Castling b) noexcept
{
	return static_cast<Castling>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

constexpr Castling &operator|=(Castling &a, Castling b) noexcept
{
	a = a | b;
	return a;
}

constexpr bool has(Castling value, Castling flag) noexcept
{
	return (static_cast<uint8_t>(value & flag) != 0);
}
