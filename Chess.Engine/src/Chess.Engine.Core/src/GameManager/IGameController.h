/*
  ==============================================================================
	Module:         IGameController
	Description:    Interface for game logic operations
  ==============================================================================
*/

#pragma once

#include "BitboardTypes.h"
#include "Move.h"
#include "Parameters.h"

/**
 * @brief	Interface for game logic operations.
 */
class IGameController
{
public:
	virtual ~IGameController()																			 = default;

	//=========================================================================
	// Game Lifecycle
	//=========================================================================

	virtual bool		 initializeGame(GameConfiguration config)										 = 0;
	virtual void		 resetGame()																	 = 0;

	//=========================================================================
	// Move Operations
	//=========================================================================

	virtual void		 getLegalMovesFromSquare(Square sq, MoveList &moves)							 = 0;
	virtual bool		 executeMove(Move move, bool fromRemote)										 = 0;
	virtual bool		 undoLastMove()																	 = 0;

	//=========================================================================
	// Queries
	//=========================================================================

	virtual bool		 isPromotionMove(Square from, Square to) const									 = 0;
	virtual Move		 findMove(Square from, Square to, PieceTypes promotion = PieceTypes::None) const = 0;
	virtual EndGameState checkEndGame()																	 = 0;

	//=========================================================================
	// Turn Management
	//=========================================================================

	virtual Side		 getCurrentSide() const															 = 0;
	virtual bool		 isLocalPlayerTurn() const														 = 0;
	virtual void		 switchTurns()																	 = 0;

	//=========================================================================
	// CPU
	//=========================================================================

	virtual bool		 isCPUTurn() const																 = 0;
	virtual void		 requestCPUMoveAsync()															 = 0;
};
