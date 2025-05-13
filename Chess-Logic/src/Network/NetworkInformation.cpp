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
		LOG_INFO("\t Description:\t {}", adapter.description);
		LOG_INFO("\t IPv4: \t\t\t{}", adapter.IPv4);
		LOG_INFO("\t Subnet: \t\t{}", adapter.subnet);
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


NetworkAdapter NetworkInformation::getFirstEligibleAdapter() const
{
	for (auto &adapter : mNetworkAdapters)
	{
		if (adapter.eligible)
			return adapter;
	}
	return {};
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
