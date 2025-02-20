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

	virtual void onScoreUpdate()											   = 0;
	virtual void omPlayerChanged(PlayerColor player)						   = 0;
	virtual void onAddCapturedPiece(PlayerColor player, PieceType captured)	   = 0;
	virtual void onRemoteCapturedPiece(PlayerColor player, PieceType captured) = 0;
};


class IMoveObserver
{
public:
	virtual ~IMoveObserver() {};

	virtual void onExecuteMove(PossibleMove &move) = 0;
	virtual void onAddToMoveHistory(Move &move)	   = 0;
};


class IGameObserver
{
public:
	virtual ~IGameObserver() {};

	virtual void onGameStateChanged(GameState state) = 0;
	virtual void onEndGame(PlayerColor winner) = 0;
};
