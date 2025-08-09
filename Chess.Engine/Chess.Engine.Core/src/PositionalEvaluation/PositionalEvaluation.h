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
	float materialWeight   = 1.0f; // TODO: Adjust values
	float positionalWeight = 1.0f; // TODO: Adjust values
	float kingSafetyWeight = 1.0f; // TODO: Adjust values
	float mobilityWeight   = 1.0f; // TODO: Adjust values
	float tacticalWeight   = 1.0f; // TODO: Adjust values
	float structuralWeight = 1.0f; // TODO: Adjust values
};


class PositionalEvaluation
{
public:
	PositionalEvaluation(std::shared_ptr<MoveEvaluation> moveEvaluation);
	~PositionalEvaluation();

	int						   evaluatePosition(const LightChessBoard &board, PlayerColor player);
	PositionalEvaluationResult evaluatePositionDetailed(const LightChessBoard &board, PlayerColor player);

	void					   setEvaluationWeights(EvaluationWeights &weights) { mEvaluationWeights = weights; }

	int						   evaluateMaterial(const LightChessBoard &board, PlayerColor player);
	int						   evaluatePositionalAdvantage(const LightChessBoard &board, PlayerColor player);
	int						   evaluateKingSafety(const LightChessBoard &board, PlayerColor player);
	int						   evaluateMobility(const LightChessBoard &board, PlayerColor player);
	int						   evaluateTacticalOpportunities(const LightChessBoard &board, PlayerColor player);
	int						   evaluatePawnStructure(const LightChessBoard &board, PlayerColor player);


private:
	GamePhase						determineGamePhase(const LightChessBoard &board) const;

	PlayerColor						getOpponent(PlayerColor player) const;

	EvaluationWeights				getWeightsForPhase(GamePhase phase) const;

	int								evaluateBestMovesOpportunity(const LightChessBoard &board, PlayerColor player, int maxMoves);

	EvaluationWeights				mEvaluationWeights;

	std::shared_ptr<MoveEvaluation> mMoveEvaluation;
};
