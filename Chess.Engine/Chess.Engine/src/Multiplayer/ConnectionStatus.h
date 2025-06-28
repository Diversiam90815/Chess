/*
  ==============================================================================
	Module:         ConnectionStatus
	Description:    Connection status struct holding information
					to the current connection event
  ==============================================================================
*/

#pragma once

#include <string>
#include "Discovery/DiscoveryEndpoint.h"


enum class ConnectionState
{
	None				= 0,
	Disconnected		= 1,
	HostingSession		= 2,
	WaitingForARemote	= 3,
	Connecting			= 4,
	Connected			= 5,
	Disconnecting		= 6,
	Error				= 7,
	ConnectionRequested = 8,  // Client has requested a connection to the host
	PendingHostApproval = 9,  // Waiting for the host to approve the connection
	ClientFoundHost		= 10, // Client found a host
	SetPlayerColor		= 11,
	GameStarted			= 12
};


struct ConnectionStatusEvent
{
	ConnectionStatusEvent() {};
	ConnectionStatusEvent(const ConnectionState state) : state(state) {};
	ConnectionStatusEvent(const ConnectionState state, const std::string error) : state(state), errorMessage(error) {};
	ConnectionStatusEvent(const ConnectionState state, const Endpoint remote) : state(state), remoteEndpoint(remote) {};

	bool			operator==(const ConnectionStatusEvent &other) { return state == other.state; };

	ConnectionState state{ConnectionState::None};
	std::string		errorMessage{};
	Endpoint		remoteEndpoint{};
};
