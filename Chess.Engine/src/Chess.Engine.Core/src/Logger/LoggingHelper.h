/*
  ==============================================================================
	Module:         LoggingHelper
	Description:    Helper class for simplifying logging messages
  ==============================================================================
*/

#pragma once

#include <string>

#include "Parameters.h"
#include "ConnectionStatus.h"
#include "Move.h"

/// <summary>
/// Provides utility functions for converting game-related types and states to their string representations and for logging moves and board states.
/// This is to ensure more readable logging.
/// </summary>
class LoggingHelper
{
public:
	static std::string playerColourToString(PlayerColor player);
	static std::string positionToString(Position pos);
	static std::string pieceTypeToString(PieceType piece);

	static std::string gameStateToString(GameState state);
	static std::string moveTypeToString(MoveType type);

	static std::string boardStateToString(const int *boardState);

	static std::string boolToString(const bool value);

	static std::string connectionStateToString(const ConnectionState &state);

	static void		   logMove(Move &move);
	static void		   logBoardState(const int *boardState);

private:
	// Helper method to map encoded board values to characters
	static char encodeToChar(int encoded);
};
