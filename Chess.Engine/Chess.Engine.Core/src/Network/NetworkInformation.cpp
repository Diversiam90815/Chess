/*
  ==============================================================================
	Module:         NetworkInformation
	Description:    Information about the local Network setup
  ==============================================================================
*/

#include "NetworkInformation.h"


NetworkInformation::NetworkInformation() {}


NetworkInformation::~NetworkInformation()
{
	deinit();
}


bool NetworkInformation::init()
{
	WSADATA wsaData;
	int		result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0)
	{
		LOG_ERROR("WSAStartup failed with error {}", result);
		return false;
	}

	bool succeeded = getNetworkInformationFromOS();
	return succeeded;
}


void NetworkInformation::deinit()
{
	WSACleanup();

	if (mAdapterAddresses)
	{
		free(mAdapterAddresses);
		mAdapterAddresses = nullptr;
	}
}


bool NetworkInformation::getNetworkInformationFromOS()
{
	DWORD returnValue = 0;
	ULONG flags		  = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;

	// Get the required buffer size by a first call
	mOutBufLen		  = 0;
	returnValue		  = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, NULL, &mOutBufLen);

	if (returnValue != ERROR_BUFFER_OVERFLOW)
	{
		LOG_ERROR("GetAdapterAddresses failed with error {}", returnValue);
		return false;
	}
	else if (returnValue == ERROR_ACCESS_DENIED)
	{
		LOG_ERROR("Access denied: Running without admin privileges limits available information!");
		flags = 0;
	}

	// Allocate buffer of the required size
	mAdapterAddresses = (IP_ADAPTER_ADDRESSES *)malloc(mOutBufLen);
	if (mAdapterAddresses == nullptr)
	{
		LOG_ERROR("Error allocating memory for adapter addresses!");
		return false;
	}

	// Get the actual data by a second call
	returnValue = GetAdaptersAddresses(AF_UNSPEC, flags, NULL, mAdapterAddresses, &mOutBufLen);
	if (returnValue != NO_ERROR)
	{
		LOG_ERROR("GetAdapterAddresses failed with error : {}", returnValue);
		free(mAdapterAddresses);
		mAdapterAddresses = nullptr;
		return false;
	}

	return true;
}


void NetworkInformation::processAdapter()
{
	PIP_ADAPTER_ADDRESSES adapter = mAdapterAddresses;

	int					  ID	  = 1; // Giving each network adapter an ID

	while (adapter)
	{
		saveAdapter(adapter, ID);
		++ID;
		adapter = adapter->Next;
	}
}


void NetworkInformation::saveAdapter(const PIP_ADAPTER_ADDRESSES adapter, const int ID)
{
	std::string					description = WStringToStdString(adapter->Description);

	PIP_ADAPTER_UNICAST_ADDRESS unicast		= adapter->FirstUnicastAddress;

	while (unicast)
	{
		if (unicast->Address.lpSockaddr->sa_family == AF_INET)
		{
			std::string addressString	 = sockaddrToString(unicast->Address.lpSockaddr);
			std::string subnetMaskString = prefixLengthToSubnetMask(unicast->Address.lpSockaddr->sa_family, unicast->OnLinkPrefixLength);

			auto		adapter			 = NetworkAdapter(description, addressString, subnetMaskString, ID);

			mNetworkAdapters.push_back(adapter);
		}

		unicast = unicast->Next;
	}
}


void NetworkInformation::setCurrentNetworkAdapter(const NetworkAdapter &adapter)
{
	if (mCurrentNetworkAdapter != adapter)
	{
		mCurrentNetworkAdapter = adapter;
		FileManager::GetInstance()->setSelectedNetworkAdapter(adapter);

		LOG_INFO("Set user defined adapter to :");
		LOG_INFO("\t Description:\t {}", adapter.Description);
		LOG_INFO("\t IPv4: \t\t\t{}", adapter.IPv4);
		LOG_INFO("\t Subnet: \t\t{}", adapter.Subnet);
		LOG_INFO("\t ID: \t\t\t{}", adapter.ID);
	}
}


NetworkAdapter NetworkInformation::getCurrentNetworkAdapter() const
{
	return mCurrentNetworkAdapter;
}


void NetworkInformation::updateNetworkAdapter(NetworkAdapter &adapter)
{
	for (auto &it : mNetworkAdapters)
	{
		if (it == adapter)
		{
			it = adapter;
			break;
		}
	}
}


bool NetworkInformation::isAdapterCurrentlyAvailable(const NetworkAdapter &adapter)
{
	for (auto &localAdapter : mNetworkAdapters)
	{
		if (adapter == localAdapter)
			return true;
	}
	return false;
}


std::vector<NetworkAdapter> NetworkInformation::getAvailableNetworkAdapters() const
{
	return mNetworkAdapters;
}


std::string NetworkInformation::sockaddrToString(SOCKADDR *sa) const
{
	char addressBuffer[INET6_ADDRSTRLEN] = {0};

	if (sa->sa_family == AF_INET)
	{
		sockaddr_in *sockaddr_ipv4 = (sockaddr_in *)sa;
		inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), addressBuffer, sizeof(addressBuffer));
	}
	else if (sa->sa_family == AF_INET6)
	{
		sockaddr_in6 *sockaddr_ipv6 = (sockaddr_in6 *)sa;
		inet_ntop(AF_INET6, &(sockaddr_ipv6->sin6_addr), addressBuffer, sizeof(addressBuffer));
	}

	return std::string(addressBuffer);
}


std::string NetworkInformation::prefixLengthToSubnetMask(USHORT family, ULONG prefixLength) const
{
	if (family == AF_INET && prefixLength <= 32)
	{
		DWORD		   mask = (prefixLength == 0) ? 0 : (~0U << (32 - prefixLength));
		struct in_addr maskAddr;
		maskAddr.s_addr = htonl(mask);

		char maskBuffer[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &maskAddr, maskBuffer, INET_ADDRSTRLEN);

		return std::string(maskBuffer);
	}
	else if (family == AF_INET6 && prefixLength <= 128)
	{
		// IPv6 does not use subnet masks like IPv4 does. We might log the prefix length instead.
		return std::to_string(prefixLength);
	}

	return {};
}


AdapterTypes NetworkInformation::filterAdapterType(const DWORD Type) const
{
	switch (Type)
	{
	case IF_TYPE_ETHERNET_CSMACD: return AdapterTypes::Ethernet;
	case IF_TYPE_IEEE80211: return AdapterTypes::WiFi;
	case IF_TYPE_SOFTWARE_LOOPBACK: return AdapterTypes::Loopback;
	case IF_TYPE_PROP_VIRTUAL: return AdapterTypes::Virtual;
	default: return AdapterTypes::Other;
	}
}


bool NetworkInformation::getDefaultInterfaces(std::vector<NET_LUID> &pLUIDs)
{
	MIB_IPFORWARD_TABLE2 *routingTable = nullptr;

	pLUIDs.clear();

	if (GetIpForwardTable2(AF_INET, &routingTable) != NO_ERROR)
	{
		LOG_WARNING("Could not retrieve Routing Table!");

		if (routingTable)
			FreeMibTable(routingTable);

		return false;
	}

	for (ULONG i = 0; i < routingTable->NumEntries; ++i)
	{
		if (routingTable->Table[i].DestinationPrefix.PrefixLength == 0)
			pLUIDs.push_back(routingTable->Table[i].InterfaceLuid);
	}

	FreeMibTable(routingTable);
	return true;
}


std::string NetworkInformation::getHostName(const SOCKADDR *ip, const socklen_t ipLength)
{
	char nameBuffer[NI_MAXHOST];

	int	 success = getnameinfo(ip, ipLength, nameBuffer, NI_MAXHOST, nullptr, 0, NI_NAMEREQD);

	if (success != NULL)
		return {};

	return std::string(nameBuffer);
}


std::string NetworkInformation::getWifiSsid(const AdapterTypes type, const NET_LUID luid)
{
	std::string					networkName = "WiFi";
	WLAN_CONNECTION_ATTRIBUTES *connection;
	DOT11_SSID					ssid;
	GUID						guid;
	HANDLE						clientHandle{};
	DWORD						negotiatedVersion;
	DWORD						dataSize;
	void					   *data = NULL;
	DWORD						success;

	if (type == AdapterTypes::Virtual)
		networkName = "Virtual " + networkName;

	success = ConvertInterfaceLuidToGuid(&luid, &guid);

	if (success != NOERROR)
	{
		LOG_ERROR("Could not convert network interface luid to guid!");
		goto cleanup;
	}


	success = WlanOpenHandle(2, NULL, &negotiatedVersion, &clientHandle);

	if (success != NOERROR)
	{
		LOG_ERROR("Could not create wlan handle!");
		goto cleanup;
	}

	success = WlanQueryInterface(clientHandle, &guid, wlan_intf_opcode_current_connection, NULL, &dataSize, &data, NULL);

	if (success == ERROR_ACCESS_DENIED)
	{
		LOG_WARNING("Network access denied!");
		networkName = "Please allow network access";
		goto cleanup;
	}
	else if (success == NO_ERROR || !data)
	{
		LOG_ERROR("Could not access network ssid");
		goto cleanup;
	}

	connection = reinterpret_cast<WLAN_CONNECTION_ATTRIBUTES *>(data);
	if (connection->isState != wlan_interface_state_connected)
	{
		networkName = "Not Connected";
		goto cleanup;
	}

	ssid = connection->wlanAssociationAttributes.dot11Ssid;

	if (ssid.uSSIDLength <= 0)
		goto cleanup;

	networkName.assign((char *)ssid.ucSSID, ssid.uSSIDLength);

cleanup:
	if (data)
		WlanFreeMemory(data);
	if (clientHandle)
		WlanCloseHandle(clientHandle, NULL);

	return networkName;
}


std::string NetworkInformation::getNetworkGatename(const AdapterTypes type, const NET_LUID_LH luid, const std::string address)
{
	std::string			  networkName  = "Ethernet";
	MIB_IPFORWARD_TABLE2 *routingTable = nullptr;
	SOCKADDR_INET		  ip;
	NET_IFINDEX			  interfaceIndex;
	std::string			  hostName;
	DWORD				  success;

	if (type == AdapterTypes::Virtual)
		networkName = "Virtual " + networkName;

	success = ConvertInterfaceLuidToIndex(&luid, &interfaceIndex);

	if (success != NOERROR)
	{
		LOG_ERROR("Could not convert network interfacve luid to index!");
		goto cleanup;
	}

	success = GetIpForwardTable2(AF_UNSPEC, &routingTable);

	if (success != NOERROR)
	{
		LOG_ERROR("Could not get IP routing table!");
		goto cleanup;
	}

	for (int i = 0; i < routingTable->NumEntries; ++i)
	{
		if (routingTable->Table[i].DestinationPrefix.PrefixLength != 0 || routingTable->Table[i].InterfaceIndex != interfaceIndex)
			continue;

		ip = routingTable->Table[i].NextHop;

		if (ip.si_family != AF_INET && ip.si_family != AF_INET6)
			continue;

		if (ip.si_family == AF_INET)
			hostName = getHostName((SOCKADDR *)&ip, sizeof(ip.Ipv4));
		else if (ip.si_family == AF_INET6)
			hostName = getHostName((SOCKADDR *)&ip, sizeof(ip.Ipv6));

		if (hostName.length() == 0)
			continue;

		networkName += " via " + hostName;
		goto cleanup;
	}

	if (address.size() == 0)
		goto cleanup;

	networkName = networkName + " (" + address + ")";


cleanup:
	if (routingTable)
		FreeMibTable(routingTable);

	return networkName;
}


std::string NetworkInformation::getNetworkName(const AdapterTypes type, const NET_LUID_LH luid, const std::string address)
{
	std::string networkName = "";

	if (type == AdapterTypes::WiFi)
		networkName = getWifiSsid(type, luid);
	else if (type == AdapterTypes::Ethernet)
		networkName = getNetworkGatename(type, luid, address);

	return networkName;
}
