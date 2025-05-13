/*
  ==============================================================================
	Module:         MultiplayerMoveManager
	Description:    Managing, sending and receiving moves in a multiplayer scenario
  ==============================================================================
*/

#pragma once

#include <vector>

#include "Move.h"
#include "MoveSerialization.h"
#include "IObservable.h"
#include "NetworkManager.h"

using json = nlohmann::json;


class MultiplayerMoveManager : public IMoveObservable
{
public:
	MultiplayerMoveManager();
	~MultiplayerMoveManager();

	void sendMove(const Move &move);

	void handleIncomingMessage(const json &message);


	void attachObserver(IMoveObserver *observer) override;
	void detachObserver(IMoveObserver *observer) override;

	Move executeMove(PossibleMove &move) override;
	void addMoveToHistory(Move &move) override;


private:
	std::shared_ptr<NetworkManager> mNetworkManager;

	std::vector<IMoveObserver>		mObserver;
};
