/*
  ==============================================================================
	Module:			WinUIInputSource
	Description:    Communication layer for frontend updates from the backend
  ==============================================================================
*/

#include "WinUIInputSource.h"


void WinUIInputSource::setDelegate(PFN_CALLBACK pDelegate)
{
	std::lock_guard<std::mutex> lock(mDelegateMutex);
	mDelegate = pDelegate;
}


void WinUIInputSource::onAddCapturedPiece(Side player, PieceType captured)
{
	PlayerCapturedPieceEvent event{};
	event.playerColor = player;
	event.pieceType	  = captured;
	event.captured	  = true; // We captured a piece
	sendToUI(MessageType::PlayerCapturedPiece, &event);
}


void WinUIInputSource::onRemoveLastCapturedPiece(Side player, PieceType captured)
{
	PlayerCapturedPieceEvent event{};
	event.playerColor = player;
	event.pieceType	  = captured;
	event.captured	  = false; // we removed the last captured piece
	sendToUI(MessageType::PlayerCapturedPiece, &event);
}


void WinUIInputSource::onLegalMovesAvailable(Square from, const MoveList &moves)
{
	sendToUI(MessageType::LegalMovesCalculated, nullptr);
}


void WinUIInputSource::onMoveExecuted(Move move, bool fromRemote)
{
	MoveEvent event{};

	event.data			 = move.raw();

	std::string notation = "";
	HRESULT		hr		 = StringCbCopyA(event.moveNotation, MAX_STRING_LENGTH, notation.c_str());

	sendToUI(MessageType::MoveExecuted, &event);
}


void WinUIInputSource::onMoveUndone()
{
	sendToUI(MessageType::MoveUndone, nullptr);
}


void WinUIInputSource::onPromotionRequired()
{
	sendToUI(MessageType::PawnPromotion, nullptr);
}


void WinUIInputSource::onGameStateChanged(GameState state)
{
	UIGamePhase uiPhase = mapToUIPhase(state);

	sendToUI(MessageType::GameStateChanged, &uiPhase);
}


void WinUIInputSource::onGameEnded(EndGameState state, Side winner)
{
	EndgameStateEvent event;
	event.state	 = state;
	event.winner = winner;

	sendToUI(MessageType::EndGameState, &event);
}


void WinUIInputSource::onBoardStateChanged()
{
	sendToUI(MessageType::BoardStateChanged, nullptr);
}


void WinUIInputSource::onConnectionStateChanged(const ConnectionStatusEvent event)
{
	CConnectionEvent tmpEvent = convertToCStyleConnectionStateEvent(event);

	sendToUI(MessageType::ConnectionStateChanged, &tmpEvent);
}


void WinUIInputSource::onRemotePlayerChosen(Side local)
{
	sendToUI(MessageType::MultiplayerPlayerChosen, &local);
}


bool WinUIInputSource::sendToUI(MessageType type, void *message) const
{
	std::lock_guard lock(mDelegateMutex);

	if (!mDelegate)
		return false;

	mDelegate(static_cast<int>(type), message);
	return true;
}


CConnectionEvent WinUIInputSource::convertToCStyleConnectionStateEvent(const ConnectionStatusEvent state)
{
	CConnectionEvent c_style_state{};
	c_style_state.state = static_cast<int>(state.state);

	if (state.state == ConnectionState::Error)
	{
		size_t len		  = state.errorMessage.size();
		size_t bufferSize = (len + 1) * sizeof(char);
		char  *strCopy	  = static_cast<char *>(CoTaskMemAlloc(bufferSize));
		StringCbCopyA(c_style_state.errorMessage, bufferSize, state.errorMessage.c_str());
	}

	if (!state.remoteEndpoint.playerName.empty())
	{
		size_t len		  = state.remoteEndpoint.playerName.size();
		size_t bufferSize = (len + 1) * sizeof(char);
		char  *strCopy	  = static_cast<char *>(CoTaskMemAlloc(bufferSize));
		StringCbCopyA(c_style_state.remoteName, bufferSize, state.remoteEndpoint.playerName.c_str());
	}

	return c_style_state;
}


UIGamePhase WinUIInputSource::mapToUIPhase(GameState state)
{
	switch (state)
	{
	case GameState::Init: return UIGamePhase::Initializing;

	case GameState::WaitingForInput:
	case GameState::WaitingForTarget: return UIGamePhase::PlayerTurn;

	case GameState::WaitingForRemoteMove:
	case GameState::WaitingForCPUMove: return UIGamePhase::OpponentTurn;

	case GameState::PawnPromotion: return UIGamePhase::PromotionDialog;

	case GameState::GameOver: return UIGamePhase::GameEnded;

	default: return UIGamePhase::Initializing;
	}
}
