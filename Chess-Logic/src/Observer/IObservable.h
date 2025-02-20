/*
  ==============================================================================
	Module:			IObserver
	Description:    Interface observer class used by the Observer Pattern
  ==============================================================================
*/

#pragma once

#include "IObserver.h"
#include "Move.h"


class IPlayerObservable
{
public:
	virtual ~IPlayerObservable() {};
	virtual void attachObserver(IPlayerObserver *observer);
	virtual void detachObserver(IPlayerObserver *observer);

	virtual void updateScore()												 = 0;
	virtual void playerChanged(PlayerColor player)							 = 0;
	virtual void addCapturedPiece(PlayerColor player, PieceType captured)	 = 0;
	virtual void remoteCapturedPiece(PlayerColor player, PieceType captured) = 0;
};


class IMoveObservable
{
public:
	virtual ~IMoveObservable() {};
	virtual void attachObserver(IMoveObserver *observer);
	virtual void detachObserver(IMoveObserver *observer);

	virtual void executeMove(PossibleMove &move) = 0;
	virtual void addMoveToHistory(Move &move)	 = 0;
};


class IGameObservable
{
public:
	virtual ~IGameObservable() {};
	virtual void attachObserver(IGameObserver *observer);
	virtual void detachObserver(IGameObserver *observer);

	virtual void endGame(PlayerColor winner)	   = 0;
	virtual void gameStateChanged(GameState state) = 0;
};