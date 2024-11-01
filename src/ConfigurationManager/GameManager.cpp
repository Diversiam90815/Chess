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
	mChessBoard = std::make_shared<ChessBoard>();
	mChessBoard
		->initializeBoard();

	mMovementManager = std::make_unique<MovementManager>(mChessBoard);

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
