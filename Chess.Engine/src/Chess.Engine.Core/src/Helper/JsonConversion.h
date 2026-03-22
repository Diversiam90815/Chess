/*
  ==============================================================================
	Module:         JsonConversion
	Description:    Conversion from/to json
  ==============================================================================
*/

#pragma once

#include <nlohmann/json.hpp>

#include "NetworkAdapter.h"
#include "Move.h"
#include "RemoteMessaging/MultiplayerMessageStruct.h"
#include "Discovery/DiscoveryEndpoint.h"
#include "ConnectionStatus.h"


using json = nlohmann::json;


//==============================================================================
//			NetworkAdapter
//==============================================================================


inline void to_json(json &j, const NetworkAdapter &adapter)
{
	j = json{{JSONConversion::NetworkAdapterID, adapter.ID},
			 {JSONConversion::NetworkAdapterDesc, adapter.AdapterName},
			 {JSONConversion::NetworkAdapterIP, adapter.IPv4},
			 {JSONConversion::NetworkAdapterISubnet, adapter.Subnet}};
}


inline void from_json(const json &j, NetworkAdapter &adapter)
{
	j.at(JSONConversion::NetworkAdapterID).get_to(adapter.ID);
	j.at(JSONConversion::NetworkAdapterDesc).get_to(adapter.AdapterName);
	j.at(JSONConversion::NetworkAdapterIP).get_to(adapter.IPv4);
	j.at(JSONConversion::NetworkAdapterISubnet).get_to(adapter.Subnet);
}


//==============================================================================
//			Discovery Endpoint
//==============================================================================


inline void to_json(json &j, const Endpoint &ep)
{
	j = json{{JSONConversion::DiscoveryIP, ep.IPAddress}, {JSONConversion::DiscoveryPort, ep.tcpPort}, {JSONConversion::DiscoveryName, ep.playerName}};
}

inline void from_json(const json &j, Endpoint &ep)
{
	j.at(JSONConversion::DiscoveryIP).get_to(ep.IPAddress);
	j.at(JSONConversion::DiscoveryPort).get_to(ep.tcpPort);
	j.at(JSONConversion::DiscoveryName).get_to(ep.playerName);
}

//==============================================================================
//			Connection Status Event
//==============================================================================


inline void to_json(json &j, const ConnectionStatusEvent &event)
{
	j = json{
		{JSONConversion::ConnectEventType, event.state}, {JSONConversion::ConnectEventError, event.errorMessage}, {JSONConversion::ConnectEventEndpoint, event.remoteEndpoint}};
}

inline void from_json(const json &j, ConnectionStatusEvent &event)
{
	j.at(JSONConversion::ConnectEventType).get_to(event.state);
	j.at(JSONConversion::ConnectEventError).get_to(event.errorMessage);
	j.at(JSONConversion::ConnectEventEndpoint).get_to(event.remoteEndpoint);
}


//==============================================================================
//			Invitation Request
//==============================================================================

inline void to_json(json &j, const InvitationRequest &inv)
{
	j = json{{JSONConversion::InvitationPlayerName, inv.playerName}, {JSONConversion::InvitationVersion, inv.version}};
}

inline void from_json(const json &j, InvitationRequest &inv)
{
	j.at(JSONConversion::InvitationPlayerName).get_to(inv.playerName);
	j.at(JSONConversion::InvitationVersion).get_to(inv.version);
}


//==============================================================================
//			Invitation Response
//==============================================================================

inline void to_json(json &j, const InvitationResponse &response)
{
	j = json{
		{JSONConversion::InvitationPlayerName, response.playerName}, {JSONConversion::InvitationAccepted, response.accepted}, {JSONConversion::InvitationReason, response.reason}};
}

inline void from_json(const json &j, InvitationResponse &response)
{
	j.at(JSONConversion::InvitationPlayerName).get_to(response.playerName);
	j.at(JSONConversion::InvitationAccepted).get_to(response.accepted);
	j.at(JSONConversion::InvitationReason).get_to(response.reason);
}
