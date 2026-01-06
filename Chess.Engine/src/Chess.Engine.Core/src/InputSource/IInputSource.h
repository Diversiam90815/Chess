/*
  ==============================================================================
	Module:         IInputSource
	Description:    Abstract input source for different UI implementations
  ==============================================================================
*/

#pragma once

#include "BitboardTypes.h"
#include "Move.h"
#include "Parameters.h"


class StateMachine;

/**
 * @brief	Interface for input sources (WinUI, Console, Network, Tests).
 */
class IInputSource
{
public:
	virtual ~IInputSource()												   = default;

	/**
	 * @brief	Called when legal moves are available for a selected square.
	 */
	virtual void onLegalMovesAvailable(Square from, const MoveList &moves) = 0;

	/**
	 * @brief	Called when a move has been executed.
	 */
	virtual void onMoveExecuted(Move move, bool fromRemote)				   = 0;

	/**
	 * @brief	Called when move was undone.
	 */
	virtual void onMoveUndone()											   = 0;

	/**
	 * @brief	Called when promotion piece selection is needed.
	 */
	virtual void onPromotionRequired()									   = 0;

	/**
	 * @brief	Called when game state changes.
	 */
	virtual void onGameStateChanged(GameState state)					   = 0;

	/**
	 * @brief	Called when game ends.
	 */
	virtual void onGameEnded(EndGameState state, Side winner)			   = 0;

	/**
	 * @brief	Called to request board state update in UI.
	 */
	virtual void onBoardStateChanged()									   = 0;
};
