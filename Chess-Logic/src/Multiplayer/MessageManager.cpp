/*
  ==============================================================================
	Module:         MessageManager
	Description:    Managing received messages in multiplayer mode
  ==============================================================================
*/

#include "MessageManager.h"


MessageManager::MessageManager()
{
	mDispatcher.registerHandler(MultiplayerMessageType::Move, [this](const IMultiplayerMessage &msg) { this->handleMoveMessage(msg); });
}


MessageManager::~MessageManager() {}


void MessageManager::onMessageReceived(const json &j)
{
	mDispatcher.dispatchMessage(j);
}


void MessageManager::handleMoveMessage(const IMultiplayerMessage &message)
{
	const MoveMessage &moveMsg = static_cast<const MoveMessage &>(message);

	// Implementation needs to be refined at a later point
	//GameManager		  *manager = GameManager::GetInstance();

	//PossibleMove	   remoteMove;
	//remoteMove.start		  = moveMsg.move.start;
	//remoteMove.end			  = moveMsg.move.end;
	//remoteMove.type			  = moveMsg.move.type;
	//remoteMove.promotionPiece = moveMsg.move.promotionPiece;

	//manager->executeMove(remoteMove);

	// TODO: need to refactor
}
