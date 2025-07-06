/*
  ==============================================================================
	Module:			UICommunication
	Description:    Communication layer for frontend updates from the backend
  ==============================================================================
*/

#pragma once

#include <combaseapi.h>
#include <strsafe.h>
#include <mutex>

#include "IObserver.h"
#include "EngineAPIDefines.h"
#include "Player.h"


enum class MessageType
{
	EndGameState			= 1,
	PlayerScoreUpdated		= 2,
	PlayerCapturedPiece		= 3,
	PlayerChanged			= 4,
	GameStateChanged		= 5,
	MoveHistoryUpdated		= 6,
	ConnectionStateChanged	= 7,
	MultiplayerPlayerChosen = 8,
};


struct PlayerCapturedPieceEvent
{
	PlayerColor playerColor;
	PieceType	pieceType;
	bool		captured; // False if we undo the move and remove a piece
};


struct CConnectionEvent
{
	int	 state;
	char remoteName[MAX_STRING_LENGTH];
	char errorMessage[MAX_STRING_LENGTH];
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

	void onExecuteMove(const PossibleMove &move, bool fromRemote = false) override {}
	void onAddToMoveHistory(Move &move) override;
	void onClearMoveHistory() override;

	void onGameStateChanged(GameState state) override;
	void onEndGame(EndGameState state, PlayerColor winner) override;
	void onChangeCurrentPlayer(PlayerColor player) override;

	void onConnectionStateChanged(const ConnectionStatusEvent event) override;
	void onLocalPlayerChosen(const PlayerColor localPlayer) {}
	void onRemotePlayerChosen(PlayerColor local) override; // This is already the local player. This is called if the remote chose the player so we set it to the opposite
	void onLocalReadyFlagSet(const bool flag) {}


private:
	bool			   communicateToUI(MessageType type, void *message) const;

	CConnectionEvent   convertToCStyleConnectionStateEvent(const ConnectionStatusEvent state);

	PFN_CALLBACK	   mDelegate = nullptr;
	mutable std::mutex mDelegateMutex;
};
