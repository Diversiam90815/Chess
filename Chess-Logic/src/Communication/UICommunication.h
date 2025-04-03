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
	EndGameState		= 1,
	PlayerScoreUpdated	= 2,
	PlayerCapturedPiece = 3,
	PlayerChanged		= 4,
	GameStateChanged	= 5,
	MoveHistoryAdded	= 6
};


struct PlayerCapturedPieceEvent
{
	PlayerColor playerColor;
	PieceType	pieceType;
	bool		captured; // False if we undo the move and remove a piece
};


class UICommunication : public IMoveObserver, public IGameObserver, public IPlayerObserver, public IGameStateObserver
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
	void onEndGame(EndGameState state, PlayerColor winner) override;
	void onChangeCurrentPlayer(PlayerColor player) override;
	// void onMoveStateInitiated() override;



private:
	bool		 communicateToUI(MessageType type, void *message) const;

	PFN_CALLBACK mDelegate = nullptr;
};
