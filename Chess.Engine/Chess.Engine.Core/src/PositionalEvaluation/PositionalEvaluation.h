/*
  ==============================================================================
	Module:         PositionalEvaluation
	Description:    Positional evaluation for chess positions
  ==============================================================================
*/


#pragma once


#include "Parameters.h"
#include "LightChessBoard.h"
#include "Evaluation/MoveEvaluation.h";


struct PositionalEvaluationResult
{
	int materialScore	= 0;
	int positionalScore = 0;
	int kingSafetyScore = 0;
	int mobilityScore	= 0;
	int tacticalScore	= 0;
	int structuralScore = 0;

	int getTotalScore() const { return materialScore + positionalScore + kingSafetyScore + mobilityScore + tacticalScore + structuralScore; }
};

// Evaluation weights by game phase
struct EvaluationWeights
{
	float materialWeight   = 1.0f;
	float positionalWeight = 1.0f;
	float kingSafetyWeight = 1.0f;
	float mobilityWeight   = 1.0f;
	float tacticalWeight   = 1.0f;
	float structuralWeight = 1.0f;
};


class PositionalEvaluation
{
public:
	PositionalEvaluation();
	~PositionalEvaluation();

	int						   evaluatePosition(const LightChessBoard &board, PlayerColor player);
	PositionalEvaluationResult evaluatePositionDetailed(const LightChessBoard &board, PlayerColor player);

	GamePhase				   determineGamePhase(const LightChessBoard &board) const;
	PlayerColor				   getOpponent(PlayerColor player) const;

	void					   setEvaluationWeights(EvaluationWeights &weights) { mEvaluationWeights = weights; }


private:
	EvaluationWeights				getWeightsForPhase(GamePhase phase) const;

	EvaluationWeights				mEvaluationWeights;

	std::shared_ptr<MoveEvaluation> mMoveEvaluation;
};
