/*
  ==============================================================================

	Class:          GameManager

	Description:    Manager for the Chess game

  ==============================================================================
*/

#include "GameManager.h"


GameManager::GameManager()
{
}


GameManager::~GameManager()
{
}


void GameManager::init()
{
	whitePlayer.playerColor = PieceColor::White;
	blackPlayer.playerColor = PieceColor::Black;
}


void GameManager::switchTurns()
{
	if (whitePlayer.isOnTurn())
	{
		whitePlayer.setOnTurn(false);
		blackPlayer.setOnTurn(true);
		return;
	}
	blackPlayer.setOnTurn(false);
	whitePlayer.setOnTurn(true);
}
