/*
  ==============================================================================
	Module:         Evaluation
	Description:    Positional Evaluation of the chess board
  ==============================================================================
*/


#pragma once

#include "Chessboard.h"
#include "BitboardUtils.h"
#include "PieceValues.h"


/**
 * @brief	Static board evaluation.
 *			Returns score relative to the side to move (positive = good).
 *			Designed as a stateless utility — all methods are static.
 */
class Evaluation
{
public:
	Evaluation()  = delete;
	~Evaluation() = delete;

	/**
	 * @brief	Evaluate the current board position.
	 * @param	board	The board to evaluate.
	 * @return	Score in centipawns, positive favoring side to move.
	 */
	[[nodiscard]] static int evaluate(const Chessboard &board);

private:
	//=========================================================================
	// Evaluation Components
	//=========================================================================

	/**
	 * @brief	Count raw material balance (white - black).
	 */
	[[nodiscard]] static int		   evaluateMaterial(const Chessboard &board);

	/**
	 * @brief	Evaluate piece placement using piece-square tables.
	 */
	[[nodiscard]] static int		   evaluatePieceSquareTables(const Chessboard &board);

	/**
	 * @brief	Evaluate pawn structure (doubled, isolated, passed).
	 */
	[[nodiscard]] static int		   evaluatePawnStructure(const Chessboard &board);

	/**
	 * @brief	Evaluate king safety.
	 */
	[[nodiscard]] static int		   evaluateKingSafety(const Chessboard &board);

	/**
	 * @brief	Evaluate mobility (number of legal moves available).
	 */
	[[nodiscard]] static int		   evaluateMobility(const Chessboard &board);


	//=========================================================================
	// Helpers
	//=========================================================================

	/**
	 * @brief	Sum piece-square values for a given piece bitboard.
	 * @param	bitboard	Bitboard of the piece.
	 * @param	table		Piece-square table (from white's perspective).
	 * @param	isWhite		If false, the square index is mirrored.
	 */
	[[nodiscard]] static int		   scorePieceSquare(U64 bitboard, const int table[64], bool isWhite);

	/**
	 * @brief	Mirror a square index vertically (for black's perspective).
	 */
	[[nodiscard]] static constexpr int mirrorSquare(int sq) { return sq ^ 56; }


	//=========================================================================
	// Piece-Square Tables (from white's perspective, a8 = index 0)
	//=========================================================================

	// clang-format off
	static constexpr int PST_PAWN[64] =
	{
		 0,   0,   0,   0,   0,   0,   0,   0,
		50,  50,  50,  50,  50,  50,  50,  50,
		10,  10,  20,  30,  30,  20,  10,  10,
		 5,   5,  10,  25,  25,  10,   5,   5,
		 0,   0,   0,  20,  20,   0,   0,   0,
		 5,  -5, -10,   0,   0, -10,  -5,   5,
		 5,  10,  10, -20, -20,  10,  10,   5,
		 0,   0,   0,   0,   0,   0,   0,   0,
	};

	static constexpr int PST_KNIGHT[64] =
	{
		-50, -40, -30, -30, -30, -30, -40, -50,
		-40, -20,   0,   0,   0,   0, -20, -40,
		-30,   0,  10,  15,  15,  10,   0, -30,
		-30,   5,  15,  20,  20,  15,   5, -30,
		-30,   0,  15,  20,  20,  15,   0, -30,
		-30,   5,  10,  15,  15,  10,   5, -30,
		-40, -20,   0,   5,   5,   0, -20, -40,
		-50, -40, -30, -30, -30, -30, -40, -50,
	};

	static constexpr int PST_BISHOP[64] =
	{
		-20, -10, -10, -10, -10, -10, -10, -20,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-10,   0,  10,  10,  10,  10,   0, -10,
		-10,   5,   5,  10,  10,   5,   5, -10,
		-10,   0,   5,  10,  10,   5,   0, -10,
		-10,  10,  10,  10,  10,  10,  10, -10,
		-10,   5,   0,   0,   0,   0,   5, -10,
		-20, -10, -10, -10, -10, -10, -10, -20,
	};

	static constexpr int PST_ROOK[64] =
	{
		 0,   0,   0,   0,   0,   0,   0,   0,
		 5,  10,  10,  10,  10,  10,  10,   5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		 0,   0,   0,   5,   5,   0,   0,   0,
	};

	static constexpr int PST_QUEEN[64] =
	{
		-20, -10, -10,  -5,  -5, -10, -10, -20,
		-10,   0,   0,   0,   0,   0,   0, -10,
		-10,   0,   5,   5,   5,   5,   0, -10,
		 -5,   0,   5,   5,   5,   5,   0,  -5,
		  0,   0,   5,   5,   5,   5,   0,  -5,
		-10,   5,   5,   5,   5,   5,   0, -10,
		-10,   0,   5,   0,   0,   0,   0, -10,
		-20, -10, -10,  -5,  -5, -10, -10, -20,
	};

	static constexpr int PST_KING_MIDDLEGAME[64] =
	{
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-30, -40, -40, -50, -50, -40, -40, -30,
		-20, -30, -30, -40, -40, -30, -30, -20,
		-10, -20, -20, -20, -20, -20, -20, -10,
		 20,  20,   0,   0,   0,   0,  20,  20,
		 20,  30,  10,   0,   0,  10,  30,  20,
	};
	// clang-format on
};
