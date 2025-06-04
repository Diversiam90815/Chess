/*
  ==============================================================================
	Module:			UICommunication
	Description:    Communication layer for frontend updates from the backend
  ==============================================================================
*/

#pragma once

#include <combaseapi.h>
#include <strsafe.h>

#include "IObserver.h"
#include "EngineAPIDefines.h"
#include "Player.h"


enum class MessageType
{
	EndGameState			  = 1,
	PlayerScoreUpdated		  = 2,
	PlayerCapturedPiece		  = 3,
	PlayerChanged			  = 4,
	GameStateChanged		  = 5,
	MoveHistoryUpdated		  = 6,
	ConnectionStateChanged	  = 7,
	ClientRequestedConnection = 8
};


struct PlayerCapturedPieceEvent
{
	PlayerColor playerColor;
	PieceType	pieceType;
	bool		captured; // False if we undo the move and remove a piece
};


struct ConnectionEvent
{
	ConnectionState state{ConnectionState::Disconnected};
	char			errorMessage[MAX_STRING_LENGTH];
};

struct MoveHistoryEvent
{
	bool added;							  // false if cleared
	char moveNotation[MAX_STRING_LENGTH]; // If move is being added, this is the move notation
};


class UICommunication : public IMoveObserver, public IGameObserver, public IPlayerObserver, public IGameStateObserver, public IConnectionStatusObserver
{
public:
	UICommunication()  = default;
	~UICommunication() = default;

	void setDelegate(PFN_CALLBACK callback);

	void onScoreUpdate(PlayerColor player, int value) override;
	void onAddCapturedPiece(PlayerColor player, PieceType captured) override;
	void onRemoveLastCapturedPiece(PlayerColor player, PieceType captured) override;

	void onExecuteMove(const PossibleMove &move) override {}
	void onAddToMoveHistory(Move &move) override;
	void onClearMoveHistory() override;

	void onGameStateChanged(GameState state) override;
	void onEndGame(EndGameState state, PlayerColor winner) override;
	void onChangeCurrentPlayer(PlayerColor player) override;

	void onConnectionStateChanged(ConnectionState state, const std::string &errorMessage = "") override;
	void onPendingHostApproval(const std::string &remotePlayerName) override;


private:
	bool			   communicateToUI(MessageType type, void *message) const;

	PFN_CALLBACK	   mDelegate = nullptr;
	mutable std::mutex mDelegateMutex;
};
