/*
  ==============================================================================
	Module:         JsonConversion
	Description:    Conversion from/to json
  ==============================================================================
*/

#pragma once

#include "json.hpp"
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
	j = json{{jNetworkAdapterID, adapter.ID}, {jNetworkAdapterDesc, adapter.AdapterName}, {jNetworkAdapterIP, adapter.IPv4},
			 {jNetworkAdapterISubnet, adapter.Subnet}};
}


inline void from_json(const json &j, NetworkAdapter &adapter)
{
	j.at(jNetworkAdapterID).get_to(adapter.ID);
	j.at(jNetworkAdapterDesc).get_to(adapter.AdapterName);
	j.at(jNetworkAdapterIP).get_to(adapter.IPv4);
	j.at(jNetworkAdapterISubnet).get_to(adapter.Subnet);
}



//==============================================================================
//			Position
//==============================================================================


inline void to_json(json &j, const Position &pos)
{
	j = json{{jPositionX, pos.x}, {jPositionY, pos.y}};
}


inline void from_json(const json &j, Position &pos)
{
	j.at(jPositionX).get_to(pos.x);
	j.at(jPositionY).get_to(pos.y);
}



//==============================================================================
//			Possible Move
//==============================================================================


inline void to_json(json &j, const PossibleMove &possibleMove)
{
	j = json{{jMoveStart, possibleMove.start}, {jMoveEnd, possibleMove.end}, {jMoveType, possibleMove.type}, {jMovePromotion, possibleMove.promotionPiece}};
}


inline void from_json(const json &j, PossibleMove &possibleMove)
{
	j.at(jMoveStart).get_to(possibleMove.start);
	j.at(jMoveEnd).get_to(possibleMove.end);
	j.at(jMoveType).get_to(possibleMove.type);
	j.at(jMovePromotion).get_to(possibleMove.promotionPiece);
}


//==============================================================================
//			Discovery Endpoint
//==============================================================================


inline void to_json(json &j, const Endpoint &ep)
{
	j = json{{jDiscoveryIP, ep.IPAddress}, {jDiscoveryPort, ep.tcpPort}, {jDiscoveryName, ep.playerName}};
}

inline void from_json(const json &j, Endpoint &ep)
{
	j.at(jDiscoveryIP).get_to(ep.IPAddress);
	j.at(jDiscoveryPort).get_to(ep.tcpPort);
	j.at(jDiscoveryName).get_to(ep.playerName);
}

//==============================================================================
//			Connection Status Event
//==============================================================================


inline void to_json(json &j, const ConnectionStatusEvent &event)
{
	j = json{{jConnectEventType, event.state}, {jConnectEventError, event.errorMessage}, {jConnectEventEndpoint, event.remoteEndpoint}};
}

inline void from_json(const json &j, ConnectionStatusEvent &event)
{
	j.at(jConnectEventType).get_to(event.state);
	j.at(jConnectEventError).get_to(event.errorMessage);
	j.at(jConnectEventEndpoint).get_to(event.remoteEndpoint);
}


//==============================================================================
//			Invitation Request
//==============================================================================

inline void to_json(json &j, const InvitationRequest &inv)
{
	j = json{{jInvitationPlayerName, inv.playerName}, {jInvitationVersion, inv.version}};
}

inline void from_json(const json &j, InvitationRequest &inv)
{
	j.at(jInvitationPlayerName).get_to(inv.playerName);
	j.at(jInvitationVersion).get_to(inv.version);
}


//==============================================================================
//			Invitation Response
//==============================================================================

inline void to_json(json &j, const InvitationResponse &response)
{
	j = json{{jInvitationPlayerName, response.playerName}, {jInvitationAccepted, response.accepted}, {jInvitationReason, response.reason}};
}

inline void from_json(const json &j, InvitationResponse &response)
{
	j.at(jInvitationPlayerName).get_to(response.playerName);
	j.at(jInvitationAccepted).get_to(response.accepted);
	j.at(jInvitationReason).get_to(response.reason);
}
