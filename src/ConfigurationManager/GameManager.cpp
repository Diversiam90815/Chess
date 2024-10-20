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
	mWhitePlayer.setPlayerColor(PieceColor::White);
	mBlackPlayer.setPlayerColor(PieceColor::Black);

	mWhitePlayer.setOnTurn(true);
}


void GameManager::switchTurns()
{
	if (mWhitePlayer.isOnTurn())
	{
		mWhitePlayer.setOnTurn(false);
		mBlackPlayer.setOnTurn(true);
		return;
	}
	mBlackPlayer.setOnTurn(false);
	mWhitePlayer.setOnTurn(true);
}


void GameManager::adaptScore()
{
}
