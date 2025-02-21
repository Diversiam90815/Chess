/*
  ==============================================================================
	Module:			UICommunication
	Description:    Communication layer for frontend updates from the backend
  ==============================================================================
*/

#include "UICommunication.h"
#include <strsafe.h>
#include "Player.h"


void UICommunication::setDelegate(PFN_CALLBACK pDelegate)
{
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


void UICommunication::onExecuteMove()
{
	communicateToUI(MessageType::MoveExecuted, nullptr);
}


void UICommunication::onAddToMoveHistory(Move &move)
{
	std::string numberedNotation = std::to_string(move.number) + ". " + move.notation;
	size_t		len				 = numberedNotation.size();
	size_t		bufferSize		 = (len + 1) * sizeof(char);
	char	   *strCopy			 = static_cast<char *>(CoTaskMemAlloc(bufferSize));

	if (strCopy != nullptr)
	{
		HRESULT hr = StringCbCopyA(strCopy, bufferSize, numberedNotation.c_str());

		if (SUCCEEDED(hr))
		{
			communicateToUI(MessageType::MoveHistoryAdded, strCopy);
		}
	}
}


void UICommunication::onGameStateChanged(GameState state)
{
	communicateToUI(MessageType::GameStateChanged, &state);
}


void UICommunication::onEndGame(PlayerColor winner)
{
	communicateToUI(MessageType::PlayerHasWon, &winner);
}


void UICommunication::onChangeCurrentPlayer(PlayerColor player)
{
	communicateToUI(MessageType::PlayerChanged, &player);
}


void UICommunication::onMoveStateInitiated()
{
	communicateToUI(MessageType::InitiateMove, nullptr);
}


bool UICommunication::communicateToUI(MessageType type, void *message) const
{
	if (mDelegate)
	{
		mDelegate(static_cast<int>(type), message);
		return true;
	}
	return false;
}
