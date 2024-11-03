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


private:
	void							 switchTurns();

	PieceColor						 mCurrentPlayer;

	Player							 mWhitePlayer;
	Player							 mBlackPlayer;

	GameState						 mCurrentState;
	MoveState						 mCurrentMoveState;

	std::unique_ptr<MovementManager> mMovementManager;
};
