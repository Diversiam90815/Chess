/*
  ==============================================================================
	Module:         RemoteCommunication
	Description:    Managing the messages themselves, sending and receiving
  ==============================================================================
*/

#pragma once

#include "MultiplayerMessageStruct.h"


class RemoteCommunication
{
public:
	RemoteCommunication()  = default;
	~RemoteCommunication() = default;

	bool read(MultiplayerMessageType type, std::vector<uint8_t> &destination);
	bool write(std::vector<uint8_t> &data, std::string remoteIP, int remotePort, MultiplayerMessageType type);
};
