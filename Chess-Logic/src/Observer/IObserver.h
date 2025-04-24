/*
  ==============================================================================
	Module:			IObserver
	Description:    Interface observer class used by the Observer Pattern
  ==============================================================================
*/

#pragma once

#include <json.hpp>

#include "Move.h"
#include "Parameters.h"
#include "NetworkAdapter.h"


class IPlayerObserver
{
public:
	virtual ~IPlayerObserver() {};

	virtual void onScoreUpdate(PlayerColor player, int value)					   = 0;
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

	virtual void onEndGame(EndGameState state, PlayerColor winner) = 0;
	virtual void onChangeCurrentPlayer(PlayerColor player)		   = 0;
};


class IGameStateObserver
{
public:
	virtual ~IGameStateObserver() {};

	virtual void onGameStateChanged(GameState state) = 0;
};


class IRemoteReceiverObserver
{
public:
	virtual ~IRemoteReceiverObserver() {};

	virtual void onMessageReceived(const nlohmann::json &j) = 0;
};


class INetworkObserver
{
public:
	virtual ~INetworkObserver() {};

	virtual void onNetworkAdapterChanged(const NetworkAdapter &adapter) = 0;
};