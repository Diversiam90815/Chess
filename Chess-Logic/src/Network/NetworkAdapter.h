/*
  ==============================================================================
	Module:         NetworkAdapter
	Description:    Storing information about the network adapter
  ==============================================================================
*/

#pragma once

#include <string>


struct NetworkAdapter
{
	NetworkAdapter() = default;

	NetworkAdapter(const std::string &description, const std::string &ipv4, const std::string &subnet, const int id) : description(description), IPv4(ipv4), subnet(subnet), ID(id)
	{
		eligible = filterSubnet(subnet);
	}


	bool		filterSubnet(const std::string &subnet) { return subnet == "255.255.255.0"; }

	bool		operator==(const NetworkAdapter &other) const { return std::tie(description, subnet) == std::tie(other.description, other.subnet); }
	bool		operator!=(const NetworkAdapter &other) const { return !(*this == other); }


	std::string description{};
	std::string IPv4{};
	std::string subnet{};
	int			ID{0};
	bool		eligible{false};
	bool		selected{false};
};
