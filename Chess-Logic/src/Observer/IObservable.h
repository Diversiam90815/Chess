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


template <typename ObserverType>
class ObservableBase
{
public:
	virtual void attachObserver(std::weak_ptr<ObserverType> observer) { mObservers.push_back(observer); }
	virtual void detachObserver(std::weak_ptr<ObserverType> observer)
	{
		std::shared_ptr<ObserverType> target = observer.lock();
		if (!target)
			return;

		mObservers.erase(std::remove_if(mObservers.begin(), mObservers.end(),
										[&target](const std::weak_ptr<ObserverType> &obs)
										{
											auto current = obs.lock();
											return !current || current == target;
										}),
						 mObservers.end());
	}

protected:
	std::vector<std::weak_ptr<ObserverType>> mObservers;
};


class IPlayerObservable : public ObservableBase<IPlayerObserver>
{
public:
	virtual ~IPlayerObservable() {};

	virtual void updateScore()											 = 0;
	virtual void addCapturedPiece(const PieceType captured)				 = 0;
	virtual void removeLastCapturedPiece()								 = 0;
};


class IMoveObservable : public ObservableBase<IMoveObserver>
{
public:
	virtual ~IMoveObservable() {};

	virtual Move executeMove(PossibleMove &move)					   = 0;
	virtual void addMoveToHistory(Move &move)						   = 0;
};


class IGameObservable : public ObservableBase<IGameObserver>
{
public:
	virtual ~IGameObservable() {};

	virtual void endGame(EndGameState state, PlayerColor winner)	   = 0;
	virtual void changeCurrentPlayer(PlayerColor player)			   = 0;
};


class IGameStateObservable : public ObservableBase<IGameStateObserver>
{
public:
	virtual ~IGameStateObservable() {};

	virtual void gameStateChanged(const GameState state)					= 0;
};


class IRemoteCommunicationObservable : public ObservableBase<IRemoteCommunicationObserver>
{
public:
	virtual ~IRemoteCommunicationObservable() {};

	virtual void receivedMessage(const json &j)										  = 0;
};
