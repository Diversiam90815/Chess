/*
  ==============================================================================
	Module:         Bitboard
	Description:    A bitboard representing a chess board
  ==============================================================================
*/

#include "Bitboard.h"
#include <stdio.h>
#include <string.h>


void Bitboard::init()
{
	// initialize all piece bitboards with the starting position
	BitUtils::setBit(mBitBoards[PieceTypes::WPawn], a2);
	BitUtils::setBit(mBitBoards[PieceTypes::WPawn], b2);
	BitUtils::setBit(mBitBoards[PieceTypes::WPawn], c2);
	BitUtils::setBit(mBitBoards[PieceTypes::WPawn], d2);
	BitUtils::setBit(mBitBoards[PieceTypes::WPawn], e2);
	BitUtils::setBit(mBitBoards[PieceTypes::WPawn], f2);
	BitUtils::setBit(mBitBoards[PieceTypes::WPawn], g2);
	BitUtils::setBit(mBitBoards[PieceTypes::WPawn], h2);

	BitUtils::setBit(mBitBoards[PieceTypes::WRook], a1);
	BitUtils::setBit(mBitBoards[PieceTypes::WRook], h1);

	BitUtils::setBit(mBitBoards[PieceTypes::WKnight], b1);
	BitUtils::setBit(mBitBoards[PieceTypes::WKnight], g1);

	BitUtils::setBit(mBitBoards[PieceTypes::WBishop], c1);
	BitUtils::setBit(mBitBoards[PieceTypes::WBishop], f1);

	BitUtils::setBit(mBitBoards[PieceTypes::WKing], e1);
	BitUtils::setBit(mBitBoards[PieceTypes::WQueen], d1);

	BitUtils::setBit(mBitBoards[PieceTypes::BPawn], a7);
	BitUtils::setBit(mBitBoards[PieceTypes::BPawn], b7);
	BitUtils::setBit(mBitBoards[PieceTypes::BPawn], c7);
	BitUtils::setBit(mBitBoards[PieceTypes::BPawn], d7);
	BitUtils::setBit(mBitBoards[PieceTypes::BPawn], e7);
	BitUtils::setBit(mBitBoards[PieceTypes::BPawn], f7);
	BitUtils::setBit(mBitBoards[PieceTypes::BPawn], g7);
	BitUtils::setBit(mBitBoards[PieceTypes::BPawn], h7);

	BitUtils::setBit(mBitBoards[PieceTypes::BRook], a8);
	BitUtils::setBit(mBitBoards[PieceTypes::BRook], h8);

	BitUtils::setBit(mBitBoards[PieceTypes::BKnight], b8);
	BitUtils::setBit(mBitBoards[PieceTypes::BKnight], g8);

	BitUtils::setBit(mBitBoards[PieceTypes::BBishop], c8);
	BitUtils::setBit(mBitBoards[PieceTypes::BBishop], f8);

	BitUtils::setBit(mBitBoards[PieceTypes::BKing], e8);
	BitUtils::setBit(mBitBoards[PieceTypes::BQueen], d8);

	side = Side::White;
}
