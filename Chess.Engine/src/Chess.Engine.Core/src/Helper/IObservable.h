/*
  ==============================================================================
	Module:			IObserver
	Description:    Interface observer class used by the Observer Pattern
  ==============================================================================
*/

#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "IObserver.h"
#include "Move.h"
#include "Multiplayer/RemoteMessaging/MultiplayerMessageStruct.h"


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

	virtual void updateScore()								= 0;
	virtual void addCapturedPiece(const PieceType captured) = 0;
	virtual void removeLastCapturedPiece()					= 0;
};

//
// class IMoveObservable : public ObservableBase<IMoveObserver>
//{
// public:
//	virtual ~IMoveObservable() {};
//
//	virtual Move executeMove(PossibleMove &move, bool fromRemote) = 0;
//	virtual void addMoveToHistory(Move &move)					  = 0;
//	virtual void clearMoveHistory()								  = 0;
//};


class IGameObservable : public ObservableBase<IGameObserver>
{
public:
	virtual ~IGameObservable() {};

	virtual void notifyMoveExecuted(Move move, bool fromRemote) = 0;
	virtual void notifyMoveUndone()								= 0;
	virtual void changeCurrentPlayer(Side player)				= 0;
	virtual void endGame(EndGameState state, Side winner)		= 0;
};


class IGameStateObservable : public ObservableBase<IGameStateObserver>
{
public:
	virtual ~IGameStateObservable() {};

	virtual void gameStateChanged(const GameState state) = 0;
};


class IRemoteReceiverObservable : public ObservableBase<IRemoteReceiverObserver>
{
public:
	virtual ~IRemoteReceiverObservable() {};

	virtual void receivedMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) = 0;
};


class INetworkObservable : public ObservableBase<INetworkObserver>
{
public:
	virtual ~INetworkObservable() {};

	virtual void networkAdapterChanged(const NetworkAdapter &adapter) = 0;
};


class IRemoteMessagesObservable : public ObservableBase<IRemoteMessagesObserver>
{
public:
	virtual ~IRemoteMessagesObservable() {};

	virtual void remoteConnectionStateReceived(const ConnectionState &state)		  = 0;
	virtual void remoteMoveReceived(const PossibleMove &move)						  = 0;
	virtual void remoteChatMessageReceived(const std::string &message)				  = 0;
	virtual void remoteInvitationReceived(const InvitationRequest &invite)			  = 0;
	virtual void remoteInvitationResponseReceived(const InvitationResponse &response) = 0;
	virtual void remotePlayerChosenReceived(const PlayerColor player)				  = 0;
	virtual void remotePlayerReadyFlagReceived(const bool flag)						  = 0;
};


class IRemoteSenderObservable : public ObservableBase<IRemoteSenderObserver>
{
public:
	virtual ~IRemoteSenderObservable() {};

	virtual void sendMessage(MultiplayerMessageType type, std::vector<uint8_t> &message) = 0;
};


class IDiscoveryObservable : public ObservableBase<IDiscoveryObserver>
{
public:
	virtual ~IDiscoveryObservable() {};

	virtual void remoteFound(const Endpoint &remote) = 0;
};


class IConnectionStatusObservable : public ObservableBase<IConnectionStatusObserver>
{
public:
	virtual ~IConnectionStatusObservable() {};

	virtual void connectionStatusChanged(const ConnectionStatusEvent event) = 0;
	virtual void localPlayerChosen(const PlayerColor localPlayer)			= 0;
	virtual void remotePlayerChosen(const PlayerColor localPlayer)			= 0;
	virtual void localReadyFlagSet(const bool flag)							= 0;
};


class ICPUMoveObservable : public ObservableBase<ICPUMoveObserver>
{
public:
	virtual ~ICPUMoveObservable() {};

	virtual void moveCalculated(PossibleMove move) = 0;
};