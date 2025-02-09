/*
  ==============================================================================
	Class:          NetworkDiscovery
	Description:    Discovery of other Chess Apps on the local network using multicast packages
  ==============================================================================
*/

#include "DiscoveryService.h"


DiscoveryService::DiscoveryService() {}


DiscoveryService::~DiscoveryService() {}


void DiscoveryService::init(asio::io_context &io_context, unsigned short tcpPort, const std::string &playerName) {}


void DiscoveryService::start() {}


void DiscoveryService::stop() {}


void DiscoveryService::setPeerCallback(PeerCallback callback) {}


void DiscoveryService::sendPackage(std::string IPAddress) {}


void DiscoveryService::handleSend(const boost::system::error_code &error, size_t bytesSent) {}


void DiscoveryService::startReceive() {}


void DiscoveryService::handleReceive(const boost::system::error_code &error, size_t bytesReceived) {}
