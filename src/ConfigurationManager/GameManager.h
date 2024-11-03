/*
  ==============================================================================

	Class:          GameManager

	Description:    Manager for the Chess game

  ==============================================================================
*/


#pragma once

#include "MovementManager.h"
#include "Player.h"


class GameManager
{
public:
	GameManager();
	~GameManager();

	void	  init();

	void	  executeMove(PossibleMove &move);

	void	  setCurrentGameState(GameState state);
	GameState getCurrentGameState() const;

	void	  setCurrentMoveState(MoveState state);
	MoveState getCurrentMoveState() const;



private:
	void							 switchTurns();

	void							 handleMoveStateChanges(PossibleMove &move);

	void							 checkForEndGameConditions();

	PieceColor						 mCurrentPlayer;

	Player							 mWhitePlayer;
	Player							 mBlackPlayer;

	GameState						 mCurrentState;
	MoveState						 mCurrentMoveState;

	std::unique_ptr<MovementManager> mMovementManager;
};
