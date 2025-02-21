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
#include "ChessLogicAPIDefines.h"
#include "Player.h"


enum class MessageType
{
	PlayerHasWon		= 1,
	InitiateMove		= 2,
	PlayerScoreUpdated	= 3,
	PlayerCapturedPiece = 4,
	MoveExecuted		= 5,
	PlayerChanged		= 6,
	GameStateChanged	= 7,
	MoveHistoryAdded	= 8
};

struct PlayerCapturedPieceEvent
{
	PlayerColor playerColor;
	PieceType	pieceType;
	bool		captured; // False if we undo the move and remove a piece
};


class UICommunication : public IMoveObserver, public IGameObserver, public IPlayerObserver
{
public:
	UICommunication()  = default;
	~UICommunication() = default;

	void setDelegate(PFN_CALLBACK callback);

	void onScoreUpdate(PlayerColor player, int value) override;
	void onAddCapturedPiece(PlayerColor player, PieceType captured) override;
	void onRemoveLastCapturedPiece(PlayerColor player, PieceType captured) override;

	void onExecuteMove() override;
	void onAddToMoveHistory(Move &move) override;

	void onGameStateChanged(GameState state) override;
	void onEndGame(PlayerColor winner) override;
	void onChangeCurrentPlayer(PlayerColor player) override;
	void onMoveStateInitiated() override;

private:
	bool		 communicateToUI(MessageType type, void *message) const;

	PFN_CALLBACK mDelegate = nullptr;
};
