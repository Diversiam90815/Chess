/*
  ==============================================================================
	Module:         MoveNotation
	Description:    Transforming the Move class into a Standart Algebraic Notation string
  ==============================================================================
*/

#pragma once

#include <string>

#include "Move.h"
#include "BitboardTypes.h"
#include "Chessboard.h"

/**
 * @brief	Utility for generating and formatting move notation strings.
 */
class MoveNotation
{
public:
	/**
	 * @brief	Produce SAN (Standard Algebraic Notation) for a move.
	 */
	static std::string toSAN(Move &move, const Chessboard &board, bool isCheck, bool isCheckmate);

	/**
	 * @brief Generate UCI notation (e.g., "e2e4", "e7e8q").
	 */
	static std::string toUCI(Move move);


private:
	static inline std::string squareToString(Square sq) noexcept
	{
		int idx = to_index(sq);

		if (idx >= 0 && idx < 64)
			return square_to_coordinates[idx];

		return "--";
	}

	static inline char getFile(Square sq) noexcept
	{
		int idx = to_index(sq);
		return 'a' + (idx % 8);
	}

	static inline char getRank(Square sq) noexcept
	{
		int idx = to_index(sq);
		return '8' - (idx / 8);
	}

	static inline char pieceToSANChar(int pieceType) noexcept
	{
		switch (pieceType)
		{
		case WKing:
		case BKing: return 'K';
		case WQueen:
		case BQueen: return 'Q';
		case WRook:
		case BRook: return 'R';
		case WBishop:
		case BBishop: return 'B';
		case WKnight:
		case BKnight: return 'N';
		default: return '\0'; // Pawns have no letter in SAN
		}
	}

	static inline char pieceToUCIChar(int pieceType) noexcept
	{
		switch (pieceType)
		{
		case WKnight:
		case BKnight: return 'n';
		case WBishop:
		case BBishop: return 'b';
		case WRook:
		case BRook: return 'r';
		case WQueen:
		case BQueen: return 'q';
		default: return '\0';
		}
	}
};
