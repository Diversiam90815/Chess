/*
  ==============================================================================
	Module:         RemoteCommunication
	Description:    Managing the messages themselves, sending and receiving
  ==============================================================================
*/ 

#include "RemoteCommunication.h"

bool RemoteCommunication::read(MultiplayerMessageType type, std::vector<uint8_t> &destination)
{
	return false;
}

bool RemoteCommunication::write(std::vector<uint8_t> &data, std::string remoteIP, int remotePort, MultiplayerMessageType type)
{
	return false;
}
