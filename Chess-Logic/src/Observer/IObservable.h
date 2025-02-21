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
	virtual void attachObserver(IPlayerObserver *observer)	= 0;
	virtual void detachObserver(IPlayerObserver *observer)	= 0;

	virtual void updateScore()								= 0;
	virtual void addCapturedPiece(const PieceType captured) = 0;
	virtual void removeLastCapturedPiece()					= 0;
};


class IMoveObservable
{
public:
	virtual ~IMoveObservable() {};
	virtual void attachObserver(IMoveObserver *observer) = 0;
	virtual void detachObserver(IMoveObserver *observer) = 0;

	virtual Move executeMove(PossibleMove &move)		 = 0;
	virtual void addMoveToHistory(Move &move)			 = 0;
};


class IGameObservable
{
public:
	virtual ~IGameObservable() {};
	virtual void attachObserver(IGameObserver *observer) = 0;
	virtual void detachObserver(IGameObserver *observer) = 0;

	virtual void endGame(PlayerColor winner)			 = 0;
	virtual void gameStateChanged(GameState state)		 = 0;
	virtual void changeCurrentPlayer(PlayerColor player) = 0;
	virtual void moveStateInitiated()					 = 0;
};
