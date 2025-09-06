/*
  ==============================================================================
	Module:         NetworkInformation
	Description:    Information about the local Network setup
  ==============================================================================
*/

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iptypes.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <wlanapi.h>
//
//#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib, "Iphlpapi.lib")

#include <vector>

#include "Logging.h"
#include "Conversion.h"
#include "NetworkAdapter.h"



class NetworkInformation
{
public:
	NetworkInformation();
	~NetworkInformation();

	bool						init();

	void						deinit();

	bool						getNetworkInformationFromOS();

	void						processAdapter();

	void						saveAdapter(const PIP_ADAPTER_ADDRESSES adapter, const int ID);

	void						setCurrentNetworkAdapter(const NetworkAdapter &adapter);
	NetworkAdapter				getCurrentNetworkAdapter() const;

	void						updateNetworkAdapter(NetworkAdapter &adapter);

	bool						isAdapterCurrentlyAvailable(const NetworkAdapter &adapter);

	std::vector<NetworkAdapter> getAvailableNetworkAdapters() const;


private:
	std::string					sockaddrToString(SOCKADDR *sa) const;
	std::string					prefixLengthToSubnetMask(USHORT family, ULONG prefixLength) const;

	bool						getDefaultInterfaces(std::vector<NET_LUID> &pLUIDs);

	std::string					getHostName(const SOCKADDR *ip, const socklen_t ipLength);

	std::string					getWifiSsid(const AdapterTypes type, const NET_LUID luid);

	std::string					getNetworkGatename(const AdapterTypes type, const NET_LUID_LH luid, const std::string address);

	std::string					getNetworkName(const AdapterTypes type, const NET_LUID_LH luid, const std::string address);


	PIP_ADAPTER_ADDRESSES		mAdapterAddresses = nullptr;

	ULONG						mOutBufLen{0};

	std::vector<NetworkAdapter> mNetworkAdapters{};

	NetworkAdapter				mCurrentNetworkAdapter{};
};
