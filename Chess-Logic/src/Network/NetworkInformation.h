/*
  ==============================================================================

	Class:          NetworkInformation

	Description:    Information about the local Network setup

  ==============================================================================
*/

#pragma once

#include <Windows.h>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <iptypes.h>
#include <WS2tcpip.h>

#include <vector>

#include "Logging.h"
#include "Helper.h"


struct NetworkAdapterInformation
{
	NetworkAdapterInformation(const std::string &description, const std::string &ipv4, const std::string &subnet, const int id)
		: description(description), IPv4(ipv4), subnet(subnet), ID(id)
	{
		eligible = filterSubnet(subnet);
	}


	bool		filterSubnet(const std::string &subnet) { return subnet == "255.255.255.0"; }

	bool		operator==(const NetworkAdapterInformation &other) const { return std::tie(description, subnet) == std::tie(other.description, other.subnet); }
	bool		operator!=(const NetworkAdapterInformation &other) const { return !(*this == other); }


	std::string description{};
	std::string IPv4{};
	std::string subnet{};
	int			ID{0};
	bool		eligible{false};
};


class NetworkInformation
{
public:
	NetworkInformation();
	~NetworkInformation();

	bool					  init();

	void					  deinit();

	bool					  getNetworkInformationFromOS();

	void					  processAdapter();

	void					  saveAdapter(const PIP_ADAPTER_ADDRESSES adapter, const int ID);

	void					  setCurrentNetworkAdapter(const NetworkAdapterInformation &adapter);
	
	NetworkAdapterInformation getCurrentNetworkAdapter() const;


private:
	std::string							   sockaddrToString(SOCKADDR *sa) const;
	std::string							   prefixLengthToSubnetMask(USHORT family, ULONG prefixLength) const;


	PIP_ADAPTER_ADDRESSES				   mAdapterAddresses;

	ULONG								   mOutBufLen;

	std::vector<NetworkAdapterInformation> mNetworkAdapters;

	NetworkAdapterInformation			   mCurrentNetworkAdapter;
};
