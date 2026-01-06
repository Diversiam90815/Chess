/*
  ==============================================================================
	Module:         ZobristHash
	Description:    Zobrist hashing for board position identification
  ==============================================================================
*/

#pragma once

#include <array>
#include <random>
#include <cstdint>

#include "BitboardTypes.h"


class ZobristHash
{
public:
	/**
	 * @brief	Initialize random numbers (call once at program start).
	 */
	static void		initialize();

	/**
	 * @brief	Check if initialized.
	 */
	static bool		isInitialized() { return mInitialized; }

	//=========================================================================
	// Hash Components
	//=========================================================================

	/**
	 * @brief	Get hash for a piece at a square.
	 */
	static uint64_t piece(PieceType piece, Square sq) { return mPieceKeys[static_cast<int>(piece)][static_cast<int>(sq)]; }

	/**
	 * @brief	Get hash for side to move (XOR when black to move).
	 */
	static uint64_t sideToMove() { return mSideKey; }

	/**
	 * @brief	Get hash for castling rights.
	 */
	static uint64_t castling(Castling rights) { return mCastlingKeys[static_cast<int>(rights) & 0xF]; }

	/**
	 * @brief	Get hash for en passant file (0-7, or 8 for none).
	 */
	static uint64_t enPassant(Square sq)
	{
		if (sq == Square::None)
			return 0;

		int file = static_cast<int>(sq) % 8;
		return mEnPassantKeys[file];
	}

private:
	static bool										mInitialized;
	static std::array<std::array<uint64_t, 64>, 12> mPieceKeys;		// [piece][square]
	static uint64_t									mSideKey;
	static std::array<uint64_t, 16>					mCastlingKeys;	// 16 combinations
	static std::array<uint64_t, 8>					mEnPassantKeys; // 8 files
};