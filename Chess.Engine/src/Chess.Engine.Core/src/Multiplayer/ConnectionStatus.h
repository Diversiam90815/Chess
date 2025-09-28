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

/// <summary>
/// Represents the various states of a network connection.
/// </summary>
enum class ConnectionState
{
	None				= 0,
	HostingSession		= 1,
	WaitingForARemote	= 2,
	Connected			= 3,
	Disconnected		= 4,
	Error				= 5,
	ConnectionRequested = 6, // Client has requested a connection to the host
	PendingHostApproval = 7, // Waiting for the host to approve the connection
	ClientFoundHost		= 9, // Client found a host
	SetPlayerColor		= 10,
	GameStarted			= 11,
};

/// <summary>
/// Represents an event indicating the status of a connection, including its state, an optional error message, and an optional remote endpoint.
/// </summary>
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
