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

#include <vector>
#include <unordered_set>

#include "Logging.h"
#include "Conversion.h"
#include "NetworkAdapter.h"

/// <summary>
/// Provides functionality to query and manage network adapter information on the system.
/// </summary>
class NetworkInformation
{
public:
	NetworkInformation();
	~NetworkInformation();

	bool							   init();

	void							   deinit();

	void							   processAdapter();

	void							   setCurrentNetworkAdapter(const NetworkAdapter &adapter);
	const NetworkAdapter			  &getCurrentNetworkAdapter() const;

	NetworkAdapter					   isAdapterCurrentlyAvailable(const NetworkAdapter &adapter);

	const std::vector<NetworkAdapter> &getAvailableNetworkAdapters() const;


private:
	// RAII helpers
	struct WinsockSession
	{
		bool ok{false};
		WinsockSession()
		{
			WSADATA wsa{};
			ok = (WSAStartup(MAKEWORD(2, 2), &wsa) == 0);

			if (!ok)
				LOG_ERROR("WSAStartup failed!");
		}

		~WinsockSession()
		{
			if (ok)
				WSACleanup();
		}
	};

	struct IpForwardTable
	{
		MIB_IPFORWARD_TABLE2 *ptr{nullptr};
		~IpForwardTable()
		{
			if (ptr)
				FreeMibTable(ptr);
		}
	};

	struct WlanHandle
	{
		HANDLE h{};
		~WlanHandle()
		{
			if (h)
				WlanCloseHandle(h, nullptr);
		}
	};

	struct WlanQueryData
	{
		void *ptr{};
		~WlanQueryData()
		{
			if (ptr)
				WlanFreeMemory(ptr);
		}
	};

	using AdapterBuffer = std::unique_ptr<IP_ADAPTER_ADDRESSES, void (*)(IP_ADAPTER_ADDRESSES *)>;

	void							saveAdapter(const PIP_ADAPTER_ADDRESSES adapter, const int ID, std::unordered_set<ULONG64> &defaultRouteLuidValues);
	bool							getNetworkInformationFromOS();

	std::string						sockaddrToString(SOCKADDR *sa) const;
	std::string						prefixLengthToSubnetMask(USHORT family, ULONG prefixLength) const;
	AdapterTypes					filterAdapterType(const DWORD Type) const;
	AdapterVisibility				determineAdapterVisibility(bool isDefaultRoute, bool IPv4Enabled, AdapterTypes type, IF_OPER_STATUS status);

	bool							getDefaultInterfaces(std::vector<NET_LUID> &pLUIDs);
	std::string						getHostName(const SOCKADDR *ip, const socklen_t ipLength);
	std::string						getWifiSsid(const AdapterTypes type, const NET_LUID luid);
	std::string						getNetworkGatename(const AdapterTypes type, const NET_LUID_LH luid, const std::string address);
	std::string						getNetworkName(const AdapterTypes type, const NET_LUID_LH luid, const std::string address);


	AdapterBuffer					mAdapterAddresses{nullptr, [](IP_ADAPTER_ADDRESSES *p)
									  {
										  if (p)
											  free(p);
									  }};
	ULONG							mOutBufLen{0};

	std::unique_ptr<WinsockSession> mWinsockSession;

	std::vector<NetworkAdapter>		mNetworkAdapters{};
	NetworkAdapter					mCurrentNetworkAdapter{};
};
