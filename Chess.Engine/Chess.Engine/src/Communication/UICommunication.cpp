/*
  ==============================================================================
	Module:			UICommunication
	Description:    Communication layer for frontend updates from the backend
  ==============================================================================
*/

#include "UICommunication.h"


void UICommunication::setDelegate(PFN_CALLBACK pDelegate)
{
	std::lock_guard<std::mutex> lock(mDelegateMutex);
	mDelegate = pDelegate;
}


void UICommunication::onScoreUpdate(PlayerColor player, int value)
{
	Score score = Score(player, value);
	communicateToUI(MessageType::PlayerScoreUpdated, &score);
}


void UICommunication::onAddCapturedPiece(PlayerColor player, PieceType captured)
{
	PlayerCapturedPieceEvent event{};
	event.playerColor = player;
	event.pieceType	  = captured;
	event.captured	  = true; // We captured a piece
	communicateToUI(MessageType::PlayerCapturedPiece, &event);
}


void UICommunication::onRemoveLastCapturedPiece(PlayerColor player, PieceType captured)
{
	PlayerCapturedPieceEvent event{};
	event.playerColor = player;
	event.pieceType	  = captured;
	event.captured	  = false; // we removed the last captured piece
	communicateToUI(MessageType::PlayerCapturedPiece, &event);
}


void UICommunication::onAddToMoveHistory(Move &move)
{
	std::string numberedNotation = std::to_string(move.number) + ". " + move.notation;
	size_t		len				 = numberedNotation.size();
	size_t		bufferSize		 = (len + 1) * sizeof(char);
	char	   *strCopy			 = static_cast<char *>(CoTaskMemAlloc(bufferSize));

	if (!strCopy)
		return;

	HRESULT hr = StringCbCopyA(strCopy, bufferSize, numberedNotation.c_str());

	if (!SUCCEEDED(hr))
		return;

	communicateToUI(MessageType::MoveHistoryAdded, strCopy);
}


void UICommunication::onGameStateChanged(GameState state)
{
	communicateToUI(MessageType::GameStateChanged, &state);
}


void UICommunication::onEndGame(EndGameState state, PlayerColor winner)
{
	communicateToUI(MessageType::EndGameState, &state);
}


void UICommunication::onChangeCurrentPlayer(PlayerColor player)
{
	communicateToUI(MessageType::PlayerChanged, &player);
}


void UICommunication::onConnectionStateChanged(const ConnectionStatusEvent event)
{
	CConnectionEvent tmpEvent = convertToCStyleConnectionStateEvent(event);

	communicateToUI(MessageType::ConnectionStateChanged, &tmpEvent);
}


bool UICommunication::communicateToUI(MessageType type, void *message) const
{
	PFN_CALLBACK delegate = nullptr;
	{
		std::lock_guard<std::mutex> lock(mDelegateMutex);
		delegate = mDelegate;
	}

	if (delegate)
	{
		delegate(static_cast<int>(type), message);
		return true;
	}

	LOG_WARNING("Failed to communicate to UI. Error: Delegate is null");
	return false;
}


CConnectionEvent UICommunication::convertToCStyleConnectionStateEvent(const ConnectionStatusEvent state)
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

	if (state.remoteEndpoint.isValid())
	{
		size_t len		  = state.remoteEndpoint.playerName.size();
		size_t bufferSize = (len + 1) * sizeof(char);
		char  *strCopy	  = static_cast<char *>(CoTaskMemAlloc(bufferSize));
		StringCbCopyA(c_style_state.remoteName, bufferSize, state.remoteEndpoint.playerName.c_str());
	}

	return c_style_state;
}
