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


void MoveEvaluation::orderCaptures(MoveList &moves, const Chessboard &board) const {}


void MoveEvaluation::updateKillerMove(Move move, int ply) {}


void MoveEvaluation::updateHistory(Move move, int depth) {}


void MoveEvaluation::clearSearchState() {}


int	 MoveEvaluation::evaluateMove(Move move, const Chessboard &board, Move ttMove, int ply) const
{
	return 0;
}


int MoveEvaluation::evaluateMVV_LVA(Move move, const Chessboard &board)
{
	return 0;
}


bool MoveEvaluation::isKillerMove(Move move, int ply) const
{
	return false;
}


int MoveEvaluation::getHistoryScore(Move move) const
{
	return 0;
}
