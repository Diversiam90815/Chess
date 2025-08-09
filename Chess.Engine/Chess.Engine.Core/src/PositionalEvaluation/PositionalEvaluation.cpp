/*
  ==============================================================================
	Module:         PositionalEvaluation
	Description:    Positional evaluation for chess positions
  ==============================================================================
*/

#include "PositionalEvaluation.h"


PositionalEvaluation::PositionalEvaluation() {}


PositionalEvaluation::~PositionalEvaluation() {}


int PositionalEvaluation::evaluatePosition(const LightChessBoard &board, PlayerColor player)
{
	return 0;
}


PositionalEvaluationResult PositionalEvaluation::evaluatePositionDetailed(const LightChessBoard &board, PlayerColor player)
{
	return PositionalEvaluationResult();
}


GamePhase PositionalEvaluation::determineGamePhase(const LightChessBoard &board) const
{
	return GamePhase();
}


PlayerColor PositionalEvaluation::getOpponent(PlayerColor player) const
{
	return PlayerColor();
}


EvaluationWeights PositionalEvaluation::getWeightsForPhase(GamePhase phase) const
{
	return EvaluationWeights();
}
