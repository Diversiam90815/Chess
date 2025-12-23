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


void Bitboard::parseFEN(const char *fen)
{
	// Reset boards
	memset(mBitBoards, 0ULL, sizeof(mBitBoards));
	memset(mOccupancyBitboards, 0ULL, sizeof(mOccupancyBitboards));

	// reset game state variables
	side	  = Side::None;
	enPassant = no_square;
	castle	  = 0;

	for (int rank = 0; rank < 8; ++rank)
	{
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;

			// match ascii pieces within FEN string
			if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z'))
			{
				int piece = GetPieceTypeFromChar(*fen);

				BitUtils::setBit(mBitBoards[piece], square);

				*fen++; // increment pointer to fen string
			}

			// match empty square numbers within FEN string
			if (*fen >= '0' && *fen <= '9')
			{
				int offset = *fen - '0';

				// define piece variable
				int piece  = -1;

				// loop over all piece bitboards
				for (int bbPiece = 0; bbPiece < 12; bbPiece++)
				{
					// if there is a piece on the current square
					if (BitUtils::getBit(mBitBoards[bbPiece], square))
						piece = bbPiece; // get piece code
				}

				// on empty current square
				if (piece == -1)
					file--;		// decrement the file

				file += offset; // adjust file counter

				*fen++;			// increment pointer to fen string
			}

			// match rank separator
			if (*fen == '/')
			{
				*fen++; // increment pointer to FEN string
			}
		}
	}

	// go to parsing side to move (increment pointer to FEN to skip whitespace)
	*fen++;

	// parse side to move
	*fen == 'w' ? side = Side::White : side = Side::Black;

	// go to parsing castling rights (increment pointer twice)
	fen += 2;

	// parse castling rights
	while (*fen != ' ')
	{
		switch (*fen)
		{
		case 'K': castle |= wk; break;
		case 'Q': castle |= wq; break;
		case 'k': castle |= bk; break;
		case 'q': castle |= bq; break;
		case '-': break;
		};

		*fen++;
	}

	// go to parsing enpassant square(increment pointer to FEN to skip whitespace)
	*fen++;

	// parsing en passant square
	if (*fen != '-')
	{
		int file  = fen[0] - 'a';
		int rank  = 8 - (fen[1] - 'a');

		enPassant = rank * 8 + file;
	}
	else
	{
		// no en passant square
		enPassant = no_square;
	}

	// init occupancies

	for (int piece = WKing; piece <= WRook; piece++)
		mOccupancyBitboards[Side::White] |= mBitBoards[piece];

	for (int piece = BKing; piece <= BRook; piece++)
		mOccupancyBitboards[Side::Black] |= mBitBoards[piece];

	mOccupancyBitboards[Side::Both] |= mOccupancyBitboards[Side::White];
	mOccupancyBitboards[Side::Both] |= mOccupancyBitboards[Side::Black];
}
