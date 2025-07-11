/*
  ==============================================================================
	Module:			Endpoint Tests
	Description:    Testing the Endpoint struct from the multiplayer module
  ==============================================================================
*/

#include <gtest/gtest.h>

#include "Discovery/DiscoveryEndpoint.h"


class EndpointTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		// Set up valid endpoint data
		validEndpoint.IPAddress	 = "192.168.1.100";
		validEndpoint.tcpPort	 = 8080;
		validEndpoint.playerName = "TestPlayer";
	}

	Endpoint validEndpoint;
};


TEST_F(EndpointTests, DefaultConstruction)
{
	Endpoint endpoint;

	EXPECT_TRUE(endpoint.IPAddress.empty()) << "Default IP address should be empty";
	EXPECT_EQ(endpoint.tcpPort, 0) << "Default TCP port should be 0";
	EXPECT_TRUE(endpoint.playerName.empty()) << "Default player name should be empty";
}


TEST_F(EndpointTests, IsValidWithValidData)
{
	EXPECT_TRUE(validEndpoint.isValid()) << "Endpoint with valid data should be valid";
}


TEST_F(EndpointTests, IsValidWithEmptyIPAddress)
{
	validEndpoint.IPAddress = "";
	EXPECT_FALSE(validEndpoint.isValid()) << "Endpoint with empty IP address should be invalid";
}


TEST_F(EndpointTests, IsValidWithZeroPort)
{
	validEndpoint.tcpPort = 0;
	EXPECT_FALSE(validEndpoint.isValid()) << "Endpoint with zero port should be invalid";
}

TEST_F(EndpointTests, IsValidWithEmptyPlayerName)
{
	validEndpoint.playerName = "";
	EXPECT_TRUE(validEndpoint.isValid()) << "Endpoint with empty player name should still be valid";
}


TEST_F(EndpointTests, EqualityOperatorTrue)
{
	Endpoint endpoint1;
	endpoint1.IPAddress	 = "192.168.1.100";
	endpoint1.tcpPort	 = 8080;
	endpoint1.playerName = "TestPlayer";

	Endpoint endpoint2;
	endpoint2.IPAddress	 = "192.168.1.100";
	endpoint2.tcpPort	 = 8080;
	endpoint2.playerName = "TestPlayer";

	EXPECT_TRUE(endpoint1 == endpoint2) << "Identical endpoints should be equal";
}


TEST_F(EndpointTests, EqualityOperatorFalseDifferentIP)
{
	Endpoint endpoint1;
	endpoint1.IPAddress	 = "192.168.1.100";
	endpoint1.tcpPort	 = 8080;
	endpoint1.playerName = "TestPlayer";

	Endpoint endpoint2;
	endpoint2.IPAddress	 = "192.168.1.101";
	endpoint2.tcpPort	 = 8080;
	endpoint2.playerName = "TestPlayer";

	EXPECT_FALSE(endpoint1 == endpoint2) << "Endpoints with different IP addresses should not be equal";
}


TEST_F(EndpointTests, EqualityOperatorFalseDifferentPort)
{
	Endpoint endpoint1;
	endpoint1.IPAddress	 = "192.168.1.100";
	endpoint1.tcpPort	 = 8080;
	endpoint1.playerName = "TestPlayer";

	Endpoint endpoint2;
	endpoint2.IPAddress	 = "192.168.1.100";
	endpoint2.tcpPort	 = 8081;
	endpoint2.playerName = "TestPlayer";

	EXPECT_FALSE(endpoint1 == endpoint2) << "Endpoints with different ports should not be equal";
}


TEST_F(EndpointTests, EqualityOperatorFalseDifferentPlayerName)
{
	Endpoint endpoint1;
	endpoint1.IPAddress	 = "192.168.1.100";
	endpoint1.tcpPort	 = 8080;
	endpoint1.playerName = "TestPlayer1";

	Endpoint endpoint2;
	endpoint2.IPAddress	 = "192.168.1.100";
	endpoint2.tcpPort	 = 8080;
	endpoint2.playerName = "TestPlayer2";

	EXPECT_FALSE(endpoint1 == endpoint2) << "Endpoints with different player names should not be equal";
}


TEST_F(EndpointTests, ValidIPAddresses)
{
	std::vector<std::string> validIPs = {"192.168.1.1", "10.0.0.1", "172.16.0.1", "127.0.0.1"};

	for (const auto &ip : validIPs)
	{
		Endpoint endpoint;
		endpoint.IPAddress	= ip;
		endpoint.tcpPort	= 8080;
		endpoint.playerName = "TestPlayer";

		EXPECT_TRUE(endpoint.isValid()) << "IP address " << ip << " should be valid";
	}
}


TEST_F(EndpointTests, ValidPortNumbers)
{
	std::vector<int> validPorts = {
		1,	  // Minimum valid port
		80,	  // HTTP
		443,  // HTTPS
		8080, // Common alternative HTTP
		65535 // Maximum valid port
	};

	for (int port : validPorts)
	{
		Endpoint endpoint;
		endpoint.IPAddress	= "192.168.1.100";
		endpoint.tcpPort	= port;
		endpoint.playerName = "TestPlayer";

		EXPECT_TRUE(endpoint.isValid()) << "Port number " << port << " should be valid";
	}
}
