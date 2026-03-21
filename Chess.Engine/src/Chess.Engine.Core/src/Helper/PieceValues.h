/*
  ==============================================================================
	Module:         PieceValues
	Description:    Shared piece material values used by evaluation and search
  ==============================================================================
*/

#pragma once

#include "BitboardTypes.h"


namespace PieceValues
{

// Material values in centipawns
constexpr int PAWN		  = 100;
constexpr int KNIGHT	  = 320;
constexpr int BISHOP	  = 330;
constexpr int ROOK		  = 500;
constexpr int QUEEN		  = 900;
constexpr int KING		  = 20000;


// Indexed by PieceType enum for fast lookup
// Order: WKing, WQueen, WPawn, WKnight, WBishop, WRook, BKing, BQueen, BPawn, BKnight, BBishop, BRook
constexpr int BY_TYPE[12] = {KING, QUEEN, PAWN, KNIGHT, BISHOP, ROOK, KING, QUEEN, PAWN, KNIGHT, BISHOP, ROOK};

} // namespace PieceValues
