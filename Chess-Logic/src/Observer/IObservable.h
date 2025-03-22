/*
  ==============================================================================
	Module:			IObserver
	Description:    Interface observer class used by the Observer Pattern
  ==============================================================================
*/

#pragma once
#include <memory>


#include "IObserver.h"
#include "Move.h"
#include "json.hpp"


using json = nlohmann::json;


class IPlayerObservable
{
public:
	virtual ~IPlayerObservable() {};
	virtual void attachObserver(std::weak_ptr<IPlayerObserver> observer) = 0;
	virtual void detachObserver(std::weak_ptr<IPlayerObserver> observer) = 0;

	virtual void updateScore()											 = 0;
	virtual void addCapturedPiece(const PieceType captured)				 = 0;
	virtual void removeLastCapturedPiece()								 = 0;
};


class IMoveObservable
{
public:
	virtual ~IMoveObservable() {};
	virtual void attachObserver(std::weak_ptr<IMoveObserver> observer) = 0;
	virtual void detachObserver(std::weak_ptr<IMoveObserver> observer) = 0;

	virtual Move executeMove(PossibleMove &move)					   = 0;
	virtual void addMoveToHistory(Move &move)						   = 0;
};


class IGameObservable
{
public:
	virtual ~IGameObservable() {};
	virtual void attachObserver(std::weak_ptr<IGameObserver> observer) = 0;
	virtual void detachObserver(std::weak_ptr<IGameObserver> observer) = 0;

	virtual void endGame(EndGameState state, PlayerColor winner)	   = 0;
	virtual void changeCurrentPlayer(PlayerColor player)			   = 0;
	// virtual void moveStateInitiated()							 = 0;
};


class IGameStateObservable
{
public:
	virtual ~IGameStateObservable() {};
	virtual void attachObserver(std::weak_ptr<IGameStateObserver> observer) = 0;
	virtual void detachObserver(std::weak_ptr<IGameStateObserver> observer) = 0;

	virtual void gameStateChanged(const GameState state)					= 0;
};


class IRemoteCommunicationObservable
{
public:
	virtual ~IRemoteCommunicationObservable() {};
	virtual void attachObserver(std::weak_ptr<IRemoteCommunicationObserver> observer) = 0;
	virtual void detachObserver(std::weak_ptr<IRemoteCommunicationObserver> observer) = 0;

	virtual void receivedMessage(const json &j)										  = 0;
};
