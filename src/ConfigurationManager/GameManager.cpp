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
	mChessBoard->initializeBoard();

	mMovementManager = std::make_unique<MovementManager>(mChessBoard);

	mWhitePlayer.setPlayerColor(PieceColor::White);
	mBlackPlayer.setPlayerColor(PieceColor::Black);

	mWhitePlayer.setOnTurn(true);
	mCurrentPlayer = PieceColor::White;
}


void GameManager::switchTurns()
{
	if (mCurrentPlayer == PieceColor::White)
	{
		mWhitePlayer.setOnTurn(false);
		mBlackPlayer.setOnTurn(true);
		mCurrentPlayer = PieceColor::Black;
		return;
	}
	mBlackPlayer.setOnTurn(false);
	mWhitePlayer.setOnTurn(true);
	mCurrentPlayer = PieceColor::White;
}


void GameManager::executeMove(PossibleMove &move)
{
	Move executedMove = mMovementManager->executeMove(move);

	if (executedMove.capturedPiece != PieceType::DefaultType)
	{
		if (mCurrentPlayer == PieceColor::White)
		{
			mWhitePlayer.addCapturedPiece(executedMove.capturedPiece);
			mBlackPlayer.updateScore();
		}
		else
		{
			mBlackPlayer.addCapturedPiece(executedMove.capturedPiece);
			mWhitePlayer.updateScore();
		}
	}

	switchTurns();
}
