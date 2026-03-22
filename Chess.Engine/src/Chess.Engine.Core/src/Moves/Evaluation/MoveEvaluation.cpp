/*
  ==============================================================================
	Module:         MoveEvaluation
	Description:    Move evaluation and ordering for alpha-beta search.
					Maintains search-accumulated heuristics (killers, history).
  ==============================================================================
*/

#include "MoveEvaluation.h"

#include <algorithm>


void MoveEvaluation::orderMoves(MoveList &moves, const Chessboard &board, Move ttMove, int ply) const
{
	// incremental selection sort (cheap for small lists with max ~218)
	// and we often get a cutoff before examining all moves
	for (size_t i = 0; i < moves.size(); ++i)
	{
		size_t bestIdx	 = i;
		int	   bestScore = evaluateMove(moves[i], board, ttMove, ply);

		for (size_t j = i + 1; j < moves.size(); ++j)
		{
			int s = evaluateMove(moves[j], board, ttMove, ply);

			if (s > bestScore)
			{
				bestScore = s;
				bestIdx	  = j;
			}
		}

		if (bestIdx != i)
			std::swap(moves[i], moves[bestIdx]);
	}
}


void MoveEvaluation::orderCaptures(MoveList &moves, const Chessboard &board) const
{
	for (size_t i = 0; i < moves.size(); ++i)
	{
		size_t bestIdx	 = i;
		int	   bestScore = evaluateMVV_LVA(moves[i], board);

		for (size_t j = i + 1; j < moves.size(); ++j)
		{
			int s = evaluateMVV_LVA(moves[j], board);

			if (s > bestScore)
			{
				bestScore = s;
				bestIdx	  = j;
			}
		}

		if (bestIdx != i)
			std::swap(moves[i], moves[bestIdx]);
	}
}


void MoveEvaluation::updateKillerMove(Move move, int ply)
{
	if (ply < 0 || ply >= MAX_PLY)
		return;

	// do not store captures as killers (only quiet moves)
	if (move.isCapture())
		return;

	// shift: slot 0 -> slot 1, then store slot 0
	if (mKillers[ply][0] != move)
	{
		mKillers[ply][1] = mKillers[ply][0];
		mKillers[ply][0] = move;
	}
}


void MoveEvaluation::updateHistory(Move move, int depth)
{
	if (move.isCapture())
		return;

	int from = to_index(move.from());
	int to	 = to_index(move.to());

	// depth-squared bonus (deeper cutoffs are more valuable)
	mHistory[from][to] += depth * depth;

	// prevent overflow by scaling all entries
	if (mHistory[from][to] > 1'000'000)
	{
		for (auto &row : mHistory)
			for (auto &val : row)
				val /= 2;
	}
}


void MoveEvaluation::clearSearchState()
{
	for (auto &ply : mKillers)
		ply.fill(Move::none());

	std::memset(mHistory, 0, sizeof(mHistory));
}


int MoveEvaluation::evaluateMove(Move move, const Chessboard &board, Move ttMove, int ply) const
{
	// 1 TT best move (always search first)
	if (ttMove.isValid() && move == ttMove)
		return SCORE_TT_MOVE;

	// 2 Captures - MVV-LVA
	if (move.isCapture())
		return SCORE_CAPTURE + evaluateMVV_LVA(move, board);

	// 3 promotions
	if (move.isPromotion())
		return SCORE_PROMOTION;

	// 4 killer moves
	if (isKillerMove(move, ply))
		return (move == mKillers[ply][0]) ? SCORE_KILLER_1 : SCORE_KILLER_2;

	// 5 history heuristic
	return getHistoryScore(move);
}


int MoveEvaluation::evaluateMVV_LVA(Move move, const Chessboard &board)
{
	PieceType victim   = board.pieceAt(move.to());
	PieceType attacker = board.pieceAt(move.from());

	if (victim == PieceType::None || attacker == PieceType::None)
		return 0;

	// High victim value + low attacker value = best capture
	return PieceValues::BY_TYPE[victim] * 10 - PieceValues::BY_TYPE[attacker];
}


bool MoveEvaluation::isKillerMove(Move move, int ply) const
{
	if (ply < 0 || ply >= MAX_PLY)
		return false;

	return (move == mKillers[ply][0]) || (move == mKillers[ply][1]);
}


int MoveEvaluation::getHistoryScore(Move move) const
{
	return mHistory[to_index(move.from())][to_index(move.to())];
}
