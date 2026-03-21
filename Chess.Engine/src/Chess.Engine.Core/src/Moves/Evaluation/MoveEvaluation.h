/*
  ==============================================================================
	Module:         MoveEvaluation
	Description:    Move evaluation and ordering for alpha-beta search.
					Maintains search-accumulated heuristics (killers, history).
  ==============================================================================
*/

#pragma once

#include "Move.h"
#include "Chessboard.h"
#include "PieceValues.h"

#include <array>
#include <cstring>


/**
 * @brief	Scores and orders moves for alpha-beta search.
 *
 *			Ordering priority (highest first):
 *			1. TT best-move (from transposition table)
 *			2. Captures scored by MVV-LVA
 *			3. Killer moves (quiet moves that caused beta cutoffs)
 *			4. History heuristic (quiet moves that improved alpha)
 *			5. Remaining quiet moves
 *
 *			Stateful: accumulates killer/history data during a search.
 *			Call clearSearchState() before each new top-level search.
 */
class MoveEvaluation
{
public:
	MoveEvaluation()  = default;
	~MoveEvaluation() = default;

	//=========================================================================
	// Core Interface
	//=========================================================================

	/**
	 * @brief	Orders moves in-place by estimated quality (best first).
	 * @param	moves	Move list to sort.
	 * @param	board	Current board state (for piece lookups in MVV-LVA).
	 * @param	ttMove	Best move from transposition table (may be null).
	 * @param	ply		Current search ply (for killer move indexing).
	 */
	void orderMoves(MoveList &moves, const Chessboard &board, Move ttMove, int ply) const;

	/**
	 * @brief	Order only capture moves by MVV-LVA (for quiescence).
	 * @param	moves	Move list containing only captures.
	 * @param	board	Current board state.
	 */
	void orderCaptures(MoveList &moves, const Chessboard &board) const;


	//=========================================================================
	// Search Feedback (called by CPUPlayer on cutoffs)
	//=========================================================================

	/**
	 * @brief	Record a killer move (quiet move that caused a beta cutoff).
	 * @param	move	The quiet move.
	 * @param	ply		Search ply where the cutoff occurred.
	 */
	void updateKillerMove(Move move, int ply);

	/**
	 * @brief	Boost history score for a quiet move that caused a cutoff.
	 * @param	move	The quiet move.
	 * @param	depth	Remaining search depth (deeper = bigger bonus).
	 */
	void updateHistory(Move move, int depth);

	/**
	 * @brief	Reset all search-accumulated state.
	 *			Call before each new top-level search.
	 */
	void clearSearchState();


private:
	//=========================================================================
	// Move Scoring
	//=========================================================================

	/**
	 * @brief	Compute a priority score for a single move.
	 */
	[[nodiscard]] int									   evaluateMove(Move move, const Chessboard &board, Move ttMove, int ply) const;

	/**
	 * @brief	MVV-LVA score for a capture move.
	 *			Most Valuable Victim - Least Valuable Attacker.
	 */
	[[nodiscard]] static int							   evaluateMVV_LVA(Move move, const Chessboard &board);

	/**
	 * @brief	Check if a move matches a stored killer for the given ply.
	 */
	[[nodiscard]] bool									   isKillerMove(Move move, int ply) const;

	/**
	 * @brief	Lookup history heuristic score for a quiet move.
	 */
	[[nodiscard]] int									   getHistoryScore(Move move) const;


	//=========================================================================
	// Score Tiers (ensure strict ordering between categories)
	//=========================================================================

	static constexpr int								   SCORE_TT_MOVE   = 10'000'000;
	static constexpr int								   SCORE_CAPTURE   = 5'000'000;
	static constexpr int								   SCORE_KILLER_1  = 4'000'000;
	static constexpr int								   SCORE_KILLER_2  = 3'900'000;
	static constexpr int								   SCORE_PROMOTION = 3'000'000;


	//=========================================================================
	// Killer Moves (2 slots per ply)
	//=========================================================================

	static constexpr int								   MAX_PLY		   = 64;
	static constexpr int								   KILLERS_PER_PLY = 2;

	std::array<std::array<Move, KILLERS_PER_PLY>, MAX_PLY> mKillers{};


	//=========================================================================
	// History Heuristic Table [from][to]
	//=========================================================================

	int													   mHistory[64][64]{};
};
