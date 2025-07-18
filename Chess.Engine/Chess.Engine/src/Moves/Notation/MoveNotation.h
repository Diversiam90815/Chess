/*
  ==============================================================================
	Module:         MoveNotation
	Description:    Transforming the Move class into a Standart Algebraic Notation string
  ==============================================================================
*/

#pragma once

#include <string>

#include "Move.h"
#include "MoveType.h"


class MoveNotation
{
public:
	MoveNotation();
	~MoveNotation();

	std::string generateStandardAlgebraicNotation(Move &move);

	std::string castlingToSAN(Move &move);

	std::string getPositionString(Position &pos);

	char		getFileFromPosition(Position &pos);

	char		getRankFromPosition(Position &pos);

	char		getPieceType(PieceType type);
};