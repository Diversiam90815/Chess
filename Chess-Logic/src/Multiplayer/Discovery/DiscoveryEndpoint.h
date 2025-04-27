/*
  ==============================================================================
	Module:         DiscoveryEndpoint
	Description:    Endpoint struct defining the local/remote endpoint
  ==============================================================================
*/

#pragma once

#include <string>


struct Endpoint
{
	std::string IPAddress{};
	int			tcpPort{0};
	std::string playerName{};

	bool		operator==(Endpoint &other) { return this->IPAddress == other.IPAddress && this->tcpPort == other.tcpPort && this->playerName == other.playerName; }
};
