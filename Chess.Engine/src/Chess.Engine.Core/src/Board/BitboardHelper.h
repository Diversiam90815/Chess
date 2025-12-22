/*
  ==============================================================================
	Module:         BitboardHeloer
	Description:    Helper for handling the bitboard
  ==============================================================================
*/

#pragma once

#include "MagicGenerator.h"
#include "AttackTables.h"
#include "BitboardTypes.h"
#include "BitboardUtils.h"


// Defining the different chess pieces
enum PieceTypes
{
	WKing	= 1,
	WQueen	= 2,
	WPawn	= 3,
	WKnight = 4,
	WBishop = 5,
	WRook	= 6,
	BKing	= 7,
	BQueen	= 8,
	BPawn	= 9,
	BKnight = 10,
	BBishop = 11,
	BRook	= 12
};

// ASCII pieces with the enum values as indecies
// (capital letters for white, small letters for black)
const char	asciiPieces[13]	  = "KQPNBRkqpnbr";

const char *unicodePieces[12] = {"♚", "♛", "♟︎", "♞", "♝", "♜", "♔", "♕", "♙", "♘", "♗", "♖"};

// convert ASCII character pieces to encoded constants
inline int	GetPieceTypeFromChar(char c)
{
	switch (c)
	{
	case 'K': return PieceTypes::WKing;
	case 'Q': return PieceTypes::WQueen;
	case 'P': return PieceTypes::WPawn;
	case 'N': return PieceTypes::WKnight;
	case 'B': return PieceTypes::WBishop;
	case 'R': return PieceTypes::WRook;
	case 'k': return PieceTypes::BKing;
	case 'q': return PieceTypes::BQueen;
	case 'p': return PieceTypes::BPawn;
	case 'n': return PieceTypes::BKnight;
	case 'b': return PieceTypes::BBishop;
	case 'r': return PieceTypes::BRook;
	default: return 0;
	}
}
