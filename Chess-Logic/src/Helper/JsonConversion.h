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
	j = json{{"ID", adapter.ID},		 {"Description", adapter.description}, {"IPv4", adapter.IPv4},
			 {"Subnet", adapter.subnet}, {"Eligible", adapter.eligible},	   {"Selected", adapter.selected}};
}


inline void from_json(const json &j, NetworkAdapter &adapter)
{
	j.at("ID").get_to(adapter.ID);
	j.at("Description").get_to(adapter.description);
	j.at("IPv4").get_to(adapter.IPv4);
	j.at("Subnet").get_to(adapter.subnet);
	j.at("Eligible").get_to(adapter.eligible);
	j.at("Selected").get_to(adapter.selected);
}



//==============================================================================
//			Position
//==============================================================================


inline void to_json(json &j, const Position &pos)
{
	j = json{{"x", pos.x}, {"y", pos.y}};
}


inline void from_json(const json &j, Position &pos)
{
	j.at("x").get_to(pos.x);
	j.at("y").get_to(pos.y);
}



//==============================================================================
//			Possible Move
//==============================================================================


inline void to_json(json &j, const PossibleMove &possibleMove)
{
	j = json{{"start", possibleMove.start}, {"end", possibleMove.end}, {"type", possibleMove.type}, {"promotion", possibleMove.promotionPiece}};
}


inline void from_json(const json &j, PossibleMove &possibleMove)
{
	j.at("start").get_to(possibleMove.start);
	j.at("end").get_to(possibleMove.end);
	j.at("type").get_to(possibleMove.type);
	j.at("promotion").get_to(possibleMove.promotionPiece);
}


//==============================================================================
//			Discovery Endpoint
//==============================================================================

inline void to_json(json &j, const Endpoint &ep)
{
	j = json{{"IPAddress", ep.IPAddress}, {"tcpPort", ep.tcpPort}, {"playerName", ep.playerName}};
}

inline void from_json(const json &j, Endpoint &ep)
{
	j.at("IPAddress").get_to(ep.IPAddress);
	j.at("tcpPort").get_to(ep.tcpPort);
	j.at("playerName").get_to(ep.playerName);
}
