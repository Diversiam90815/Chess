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

	void switchTurns();

	void adaptScore();


private:
	Player							 mWhitePlayer;
	Player							 mBlackPlayer;

	std::unique_ptr<MovementManager> mMovementManager;
	std::shared_ptr<ChessBoard>		 mChessBoard;
};
