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
#include "Discovery/DiscoveryEndpoint.h"

using json = nlohmann::json;


//==============================================================================
//			NetworkAdapter
//==============================================================================


inline void to_json(json &j, const NetworkAdapter &adapter)
{
	j = json{{jNetworkAdapterID, adapter.ID},		   {jNetworkAdapterDesc, adapter.description},	 {jNetworkAdapterIP, adapter.IPv4},
			 {jNetworkAdapterISubnet, adapter.subnet}, {jNetworkAdapterIEligible, adapter.eligible}, {jNetworkAdapterISelected, adapter.selected}};
}


inline void from_json(const json &j, NetworkAdapter &adapter)
{
	j.at(jNetworkAdapterID).get_to(adapter.ID);
	j.at(jNetworkAdapterDesc).get_to(adapter.description);
	j.at(jNetworkAdapterIP).get_to(adapter.IPv4);
	j.at(jNetworkAdapterISubnet).get_to(adapter.subnet);
	j.at(jNetworkAdapterIEligible).get_to(adapter.eligible);
	j.at(jNetworkAdapterISelected).get_to(adapter.selected);
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
