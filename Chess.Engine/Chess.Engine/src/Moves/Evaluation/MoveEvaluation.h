/*
  ==============================================================================
	Module:         MoveEvaluation
	Description:    Evaluation of moves
  ==============================================================================
*/

#pragma once

#include "Move.h"


class MoveEvaluation
{
public:
	MoveEvaluation()  = default;
	~MoveEvaluation() = default;

	int getBasicEvaluation(const PossibleMove &move);
};
