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
		score += 50;

	// Check bonus
	if ((move.type & MoveType::Check) == MoveType::Check)
		score += 15;

	// Checkmate bonus
	if ((move.type & MoveType::Checkmate) == MoveType::Checkmate)
		score += 1000;

	// Promotion bonus
	if ((move.type & MoveType::PawnPromotion) == MoveType::PawnPromotion)
	{
		score += 800;

		// Additional bonus for promoting to queen
		if (move.promotionPiece == PieceType::Queen)
			score += 100;
	}

	// Castling bonus
	if ((move.type & MoveType::CastlingKingside) == MoveType::CastlingKingside || (move.type & MoveType::CastlingQueenside) == MoveType::CastlingQueenside)
		score += 30;

	return score;
}
