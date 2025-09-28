/*
  ==============================================================================
	Module:         NetworkAdapter
	Description:    Storing information about the network adapter
  ==============================================================================
*/

#pragma once

#include <string>

/// <summary>
/// Enumerates the types of network adapters.
/// </summary>
enum class AdapterTypes
{
	Ethernet = 1,
	WiFi	 = 2,
	Loopback = 3,
	Virtual	 = 4,
	Other	 = 5
};

/// <summary>
/// Defines the visibility states for an adapter.
/// </summary>
enum class AdapterVisibility
{
	Hidden		= 1,
	Visible		= 2,
	Recommended = 3
};

/// <summary>
/// Represents a network adapter with associated properties and utility functions.
/// </summary>
struct NetworkAdapter
{
	NetworkAdapter() = default;

	NetworkAdapter(const std::string &adapterName,
				   const std::string &networkName,
				   const std::string &ipv4,
				   const std::string &subnet,
				   const int		  id,
				   bool				  isDefaultRoute,
				   AdapterTypes		  type,
				   AdapterVisibility  visibility)
		: AdapterName(adapterName), NetworkName(networkName), IPv4(ipv4), Subnet(subnet), ID(id), IsDefaultRoute(isDefaultRoute), Type(type), Visibility(visibility)
	{
		eligible = filterSubnetMask();
	}


	bool			  operator==(const NetworkAdapter &other) const { return std::tie(AdapterName, Subnet) == std::tie(other.AdapterName, other.Subnet); }
	bool			  operator!=(const NetworkAdapter &other) const { return !(*this == other); }

	bool			  isValid() const { return !AdapterName.empty() && !IPv4.empty() && ID != 0; }

	bool			  filterSubnetMask() const { return Subnet == "255.255.255.0"; }

	std::string		  AdapterName{};
	std::string		  NetworkName{};
	std::string		  IPv4{};
	std::string		  Subnet{};
	int				  ID{0};
	bool			  IsDefaultRoute{false};
	bool			  eligible{false};
	AdapterTypes	  Type{AdapterTypes::Other};
	AdapterVisibility Visibility{};
};
