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
	// parse FEN starting position
	parseFEN(mStartPosition);
}


void Bitboard::clear()
{
	mBitBoards.fill(0);
	mOccupancyBitboards.fill(0);
	side	  = Side::None;
	enPassant = no_square;
	castle	  = 0;
}


void Bitboard::parseFEN(std::string_view fen)
{
	clear();

	size_t i		  = 0;

	auto   skipSpaces = [&]()
	{
		while (i < fen.size() && fen[i] == ' ')
			++i;
	};

	// 1 Piece placement
	int rank = 0;
	int file = 0;

	while (i < fen.size() && fen[i] != ' ')
	{
		char c = fen[i++];

		if (c == '/')
		{
			++rank;
			file = 0;
			continue;
		}

		if (c >= '1' && c <= '8')
		{
			file += (c - '0');
			continue;
		}

		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			int piece  = GetPieceTypeFromChar(c);
			int square = rank * 8 + file;
			BitUtils::setBit(mBitBoards[piece], square);
			++file;
		}
	}

	skipSpaces();

	// 2 Side to move
	if (i <= fen.size())
	{
		side = (fen[i] == 'w') ? Side::White : Side::Black;
		++i;
	}

	skipSpaces();

	// 3 Castling
	while (i < fen.size() && fen[i] != ' ')
	{
		switch (fen[i])
		{
		case 'K': castle |= wk; break;
		case 'Q': castle |= wq; break;
		case 'k': castle |= bk; break;
		case 'q': castle |= bq; break;
		case '-': break;
		}
		++i;
	}

	skipSpaces();

	// 4 En Passant
	if (i < fen.size() && fen[i] != '-')
	{
		int epFile = fen[i + 0] - 'a';
		int epRank = 8 - (fen[i + 1] - '0');
		enPassant  = epRank * 8 + epFile;
	}
	else
	{
		enPassant = no_square;
	}

	// 5 occupancies
	mOccupancyBitboards[Side::White] = mBitBoards[WKing] | mBitBoards[WQueen] | mBitBoards[WPawn] | mBitBoards[WKnight] | mBitBoards[WBishop] | mBitBoards[WRook];
	mOccupancyBitboards[Side::Black] = mBitBoards[BKing] | mBitBoards[BQueen] | mBitBoards[BPawn] | mBitBoards[BKnight] | mBitBoards[BBishop] | mBitBoards[BRook];

	mOccupancyBitboards[Side::Both] |= mOccupancyBitboards[Side::White];
	mOccupancyBitboards[Side::Both] |= mOccupancyBitboards[Side::Black];
}



bool Bitboard::isSquareAttacked(int square, Side side)
{
	// attacked by white pawns
	if ((side == Side::White) && (mAttackTables.mPawnAttacks[Side::Black][square] & mBitBoards[WPawn]))
		return true;

	// attacked by black pawns
	if ((side == Side::Black) && (mAttackTables.mPawnAttacks[Side::White][square] & mBitBoards[BPawn]))
		return true;

	// attacked by knights
	if (mAttackTables.mKnightAttacks[square] & ((side == Side::White) ? mBitBoards[WKnight] : mBitBoards[BKnight]))
		return true;

	// attacked by kings
	if (mAttackTables.mKingAttacks[square] & ((side == Side::White) ? mBitBoards[WKing] : mBitBoards[BKing]))
		return true;

	// attacked by rooks
	if (mAttackTables.getRookAttacks(square, mOccupancyBitboards[Side::Both]) & ((side == Side::White) ? mBitBoards[WRook] : mBitBoards[BRook]))
		return true;

	// attacked by bishops
	if (mAttackTables.getBishopAttacks(square, mOccupancyBitboards[Side::Both]) & ((side == Side::White) ? mBitBoards[WBishop] : mBitBoards[BBishop]))
		return true;

	// attacked by queens
	if (mAttackTables.getQueenAttacks(square, mOccupancyBitboards[Side::Both]) & ((side == Side::White) ? mBitBoards[WQueen] : mBitBoards[BQueen]))
		return true;

	return false;
}
