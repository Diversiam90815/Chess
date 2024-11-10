/*
  ==============================================================================

	Class:          MoveType

	Description:    Defining Bitmask enum MoveType to store multiple moves

  ==============================================================================
*/ 

#pragma once

enum class MoveType : int
{
	None			  = 0,
	Normal			  = 1 << 0, // 1
	DoublePawnPush	  = 1 << 1, // 2
	PawnPromotion	  = 1 << 2, // 4
	Capture			  = 1 << 3, // 8
	EnPassant		  = 1 << 4, // 16
	CastlingKingside  = 1 << 5, // 32
	CastlingQueenside = 1 << 6, // 64
	Check			  = 1 << 7, // 128
	Checkmate		  = 1 << 8, // 256
};


inline MoveType operator|(MoveType lhs, MoveType rhs)
{
	return static_cast<MoveType>(static_cast<int>(lhs) | static_cast<int>(rhs));
}


inline MoveType &operator|=(MoveType &lhs, MoveType rhs)
{
	lhs = lhs | rhs;
	return lhs;
}


inline MoveType operator&(MoveType lhs, MoveType rhs)
{
	return static_cast<MoveType>(static_cast<int>(lhs) & static_cast<int>(rhs));
}


inline MoveType operator~(MoveType rhs)
{
	return static_cast<MoveType>(~static_cast<int>(rhs));
}
