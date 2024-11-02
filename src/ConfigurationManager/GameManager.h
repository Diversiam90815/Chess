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

	void init();

	void executeMove(PossibleMove &move);


private:
	void							 switchTurns();

	PieceColor						 mCurrentPlayer;

	Player							 mWhitePlayer;
	Player							 mBlackPlayer;

	std::unique_ptr<MovementManager> mMovementManager;

};
