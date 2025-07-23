/*
  ==============================================================================
	Module:         MoveEvaluation
	Description:    Evaluation of moves
  ==============================================================================
*/


#include "MoveEvaluation.h"


int MoveEvaluation::getBasicEvaluation(const PossibleMove &move)
{
	int score = 0;

	// Capture bonus
	if ((move.type & MoveType::Capture) == MoveType::Capture)
		score += 10;

	// Check bonus
	if ((move.type & MoveType::Check) == MoveType::Check)
		score += 5;

	// Checkmate bonus
	if ((move.type & MoveType::Checkmate) == MoveType::Checkmate)
		score += 1000;

	// Promotion bonus
	if ((move.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
		score += 8;

	// Castling bonus
	if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside || (move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
		score += 3;

	return score;
}
