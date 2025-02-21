/*
  ==============================================================================
	Module:			IObserver
	Description:    Interface observer class used by the Observer Pattern
  ==============================================================================
*/

#pragma once

#include "Move.h"


class IPlayerObserver
{
public:
	virtual ~IPlayerObserver() {};

	virtual void onScoreUpdate(Score updatedScore)								   = 0;
	virtual void onAddCapturedPiece(PlayerColor player, PieceType captured)		   = 0;
	virtual void onRemoveLastCapturedPiece(PlayerColor player, PieceType captured) = 0;
};


class IMoveObserver
{
public:
	virtual ~IMoveObserver() {};

	virtual void onExecuteMove()				= 0;
	virtual void onAddToMoveHistory(Move &move) = 0;
};


class IGameObserver
{
public:
	virtual ~IGameObserver() {};

	virtual void onGameStateChanged(GameState state)	   = 0;
	virtual void onEndGame(PlayerColor winner)			   = 0;
	virtual void onChangeCurrentPlayer(PlayerColor player) = 0;
};
