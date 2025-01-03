/*
  ==============================================================================

	Class:          LoggingHelper

	Description:    Helper class for simplifying logging messages

  ==============================================================================
*/


#pragma once

#include <string>

#include "Parameters.h"
#include "Move.h"

class LoggingHelper
{
public:
	static std::string playerColourToString(PlayerColor player);
	static std::string positionToString(Position pos);
	static std::string pieceTypeToString(PieceType piece);

	static std::string gameStateToString(GameState state);
	static std::string moveStateToString(MoveState state);
	static std::string moveTypeToString(MoveType type);

	static std::string boardStateToString(const int *boardState);

	static void		   logMove(Move &move);
	static void		   logBoardState(const int *boardState);

private:
	static char encodeToChar(int encoded);
};
