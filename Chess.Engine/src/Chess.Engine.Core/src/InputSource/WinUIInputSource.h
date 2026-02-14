/*
  ==============================================================================
	Module:			WinUIInputSource
	Description:    Communication layer for frontend updates from the backend
  ==============================================================================
*/

#pragma once

#include <combaseapi.h>
#include <strsafe.h>
#include <mutex>

#include "IObserver.h"
#include "PlainCDefines.h"
#include "Player.h"
#include "IInputSource.h"
#include "Notation/MoveNotation.h"


/// <summary>
/// Enumerates the different types of messages that can be sent or received in the game.
/// </summary>
enum class MessageType
{
	EndGameState			= 1,
	PlayerCapturedPiece		= 2,
	PlayerChanged			= 3,
	GameStateChanged		= 4,
	MoveExecuted			= 5,
	MoveUndone				= 6,
	ConnectionStateChanged	= 7,
	MultiplayerPlayerChosen = 8,
	BoardStateChanged		= 9,
	PawnPromotion			= 10,
	LegalMovesCalculated	= 11,
};


/// <summary>
/// Represents an event where a player captures or uncaptures a piece.
/// </summary>
struct PlayerCapturedPieceEvent
{
	Side	  playerColor;
	PieceType pieceType;
	bool	  captured; // False if we undo the move and remove a piece
};


/// <summary>
/// Represents an event in the move history, indicating whether a move was added or the history was cleared.
/// </summary>
struct MoveEvent
{
	uint16_t data;
	char	 moveNotation[MAX_STRING_LENGTH]; // If move is being added, this is the move notation
};


/// <summary>
/// Represents an event indicating the end state of a game, including the winner if applicable.
/// </summary>
struct EndgameStateEvent
{
	EndGameState state;
	Side		 winner = Side::None;
};


/// <summary>
/// WinUIInputSource manages communication between the user interface and the game logic, observing and handling various game and connection events.
/// </summary>
class WinUIInputSource : public IInputSource, public IPlayerObserver, public IConnectionStatusObserver
{
public:
	WinUIInputSource()	= default;
	~WinUIInputSource() = default;

	void setDelegate(PFN_CALLBACK callback);

	//=========================================================================
	// IInputSource Implementation (from StateMachine)
	//=========================================================================

	void onLegalMovesAvailable(Square from, const MoveList &moves) override;
	void onMoveExecuted(Move move, bool fromRemote) override;
	void onMoveUndone() override;
	void onPromotionRequired() override;
	void onGameStateChanged(GameState state) override;
	void onGameEnded(EndGameState state, Side winner) override;
	void onBoardStateChanged() override;

	//=========================================================================
	// IPlayerObserver (scores, captured pieces)
	//=========================================================================

	void onAddCapturedPiece(Side player, PieceType captured) override;
	void onRemoveLastCapturedPiece(Side player, PieceType captured) override;

	//=========================================================================
	// IConnectionStatusObserver (multiplayer)
	//=========================================================================

	void onConnectionStateChanged(const ConnectionStatusEvent event) override;
	void onLocalPlayerChosen(const Side localPlayer) {}
	void onRemotePlayerChosen(Side local) override; // This is already the local player. This is called if the remote chose the player so we set it to the opposite
	void onLocalReadyFlagSet(const bool flag) {}


private:
	bool			   sendToUI(MessageType type, void *message) const;

	CConnectionEvent   convertToCStyleConnectionStateEvent(const ConnectionStatusEvent state);

	UIGamePhase		   mapToUIPhase(GameState state);


	PFN_CALLBACK	   mDelegate = nullptr;
	mutable std::mutex mDelegateMutex;
};
