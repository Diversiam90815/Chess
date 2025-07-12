/*
  ==============================================================================
	Module:			DiscoveryService Tests
	Description:    Testing the DiscoveryService class from the multiplayer module
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <thread>
#include <chrono>

#include "Discovery/DiscoveryService.h"


namespace MultiplayerTests
{

// Mock observer for testing discovery events
class MockDiscoveryObserver : public IDiscoveryObserver
{
public:
	MOCK_METHOD(void, onRemoteFound, (const Endpoint &remote), (override));
};


class DiscoveryServiceTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		ioContext		 = std::make_unique<asio::io_context>();
		discoveryService = std::make_unique<DiscoveryService>(*ioContext);
		mockObserver	 = std::make_shared<MockDiscoveryObserver>();

		// Start IO context in a separate thread
		ioThread		 = std::thread([this]() { ioContext->run(); });
	}

	void TearDown() override
	{
		if (discoveryService)
		{
			discoveryService->deinit();
			discoveryService->stop();
		}

		ioContext->stop();
		if (ioThread.joinable())
		{
			ioThread.join();
		}
	}

	std::unique_ptr<asio::io_context>	   ioContext;
	std::unique_ptr<DiscoveryService>	   discoveryService;
	std::shared_ptr<MockDiscoveryObserver> mockObserver;
	std::thread							   ioThread;
};


TEST_F(DiscoveryServiceTests, DefaultConstruction)
{
	EXPECT_FALSE(discoveryService->isRunning()) << "DiscoveryService should not be running by default";
}


TEST_F(DiscoveryServiceTests, InitializationWithValidParameters)
{
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "127.0.0.1";
	unsigned short port		  = 8080;

	bool		   result	  = discoveryService->init(playerName, localIP, port);
	EXPECT_TRUE(result) << "Initialization should succeed with valid parameters";
}


TEST_F(DiscoveryServiceTests, InitializationWithEmptyPlayerName)
{
	std::string	   playerName = "";
	std::string	   localIP	  = "127.0.0.1";
	unsigned short port		  = 8080;

	bool		   result	  = discoveryService->init(playerName, localIP, port);
	EXPECT_FALSE(result) << "Initialization should fail with empty player name";
}


TEST_F(DiscoveryServiceTests, InitializationWithEmptyIP)
{
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "";
	unsigned short port		  = 8080;

	bool		   result	  = discoveryService->init(playerName, localIP, port);
	EXPECT_FALSE(result) << "Initialization should fail with empty IP address";
}



TEST_F(DiscoveryServiceTests, StartDiscoveryInServerMode)
{
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "127.0.0.1";
	unsigned short port		  = 8080;

	bool		   initResult = discoveryService->init(playerName, localIP, port);
	ASSERT_TRUE(initResult) << "Initialization should succeed";

	EXPECT_NO_THROW(discoveryService->startDiscovery(DiscoveryMode::Server)) << "Starting discovery in server mode should not throw";
}


TEST_F(DiscoveryServiceTests, StartDiscoveryInClientMode)
{
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "127.0.0.1";
	unsigned short port		  = 0; // Client doesn't need a specific port

	bool		   initResult = discoveryService->init(playerName, localIP, port);
	ASSERT_TRUE(initResult) << "Initialization should succeed";

	EXPECT_NO_THROW(discoveryService->startDiscovery(DiscoveryMode::Client)) << "Starting discovery in client mode should not throw";
}


TEST_F(DiscoveryServiceTests, StartDiscoveryWithoutInitialization)
{
	EXPECT_THROW(discoveryService->startDiscovery(DiscoveryMode::Server), std::exception) << "Starting discovery without initialization should throw";
}

TEST_F(DiscoveryServiceTests, GetEndpointFromValidIP)
{
	// Add remote endpoint to the list
	std::string testIP = "192.168.1.100";
	Endpoint	ep;
	ep.IPAddress  = testIP;
	ep.playerName = "TestPlayer";
	ep.tcpPort	  = 8080;
	discoveryService->addRemoteToList(ep);

	Endpoint endpoint = discoveryService->getEndpointFromIP(testIP);

	EXPECT_EQ(endpoint.IPAddress, testIP) << "Returned endpoint should have the queried IP";
}


TEST_F(DiscoveryServiceTests, GetEndpointFromInvalidIP)
{
	std::string invalidIP = "";

	Endpoint	ep;
	ep.IPAddress  = "192.168.1.100";
	ep.playerName = "TestPlayer";
	ep.tcpPort	  = 8080;
	discoveryService->addRemoteToList(ep);

	Endpoint endpoint = discoveryService->getEndpointFromIP(invalidIP);

	EXPECT_FALSE(endpoint.isValid()) << "Should return invalid endpoint for empty IP";
}


TEST_F(DiscoveryServiceTests, DuplicateEndpointFiltering)
{
	// Init local discovery
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "127.0.0.1";
	unsigned short port		  = 8080;

	bool		   initResult = discoveryService->init(playerName, localIP, port);
	ASSERT_TRUE(initResult) << "Initialization should succeed";

	// Setup in client mode to trigger observer notifications
	discoveryService->attachObserver(mockObserver);
	discoveryService->startDiscovery(DiscoveryMode::Client);

	// Create first endpoint
	Endpoint ep1;
	ep1.IPAddress  = "192.168.1.100";
	ep1.tcpPort	   = 9000;
	ep1.playerName = "RemotePlayer1";

	// Create identical second endpoint
	Endpoint epDuplicate;
	epDuplicate.IPAddress  = "192.168.1.100";
	epDuplicate.tcpPort	   = 9000;
	epDuplicate.playerName = "RemotePlayer1";

	// Create different endpoint
	Endpoint ep2;
	ep2.IPAddress  = "192.168.1.101";
	ep2.tcpPort	   = 9001;
	ep2.playerName = "RemotePlayer2";

	// Expect observer call only twice (for unique endpoints)
	EXPECT_CALL(*mockObserver, onRemoteFound(::testing::_)).Times(2);

	// Add endpoints to the list
	discoveryService->addRemoteToList(ep1);
	discoveryService->addRemoteToList(epDuplicate);
	discoveryService->addRemoteToList(ep2);

	// Verify both unique endpoints can be retrieved
	Endpoint retrieved1 = discoveryService->getEndpointFromIP("192.168.1.100");
	Endpoint retrieved2 = discoveryService->getEndpointFromIP("192.168.1.101");

	EXPECT_TRUE(retrieved1.isValid()) << "First endpoint should be retrievable";
	EXPECT_TRUE(retrieved2.isValid()) << "Second endpoint should be retrievable";
	EXPECT_EQ(retrieved1.playerName, "RemotePlayer1") << "First endpoint should have correct player name";
	EXPECT_EQ(retrieved2.playerName, "RemotePlayer2") << "Second endpoint should have correct player name";
}


TEST_F(DiscoveryServiceTests, LocalEndpointFiltering)
{
	// Init Discovery with local player
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "192.168.1.50";
	unsigned short port		  = 8080;

	bool		   initResult = discoveryService->init(playerName, localIP, port);
	ASSERT_TRUE(initResult) << "Initialization should succeed";

	// Set up in client mode to trigger observer notifications
	discoveryService->attachObserver(mockObserver);
	discoveryService->startDiscovery(DiscoveryMode::Client);

	// Create endpoint with same IP as local machine
	Endpoint localEndpoint;
	localEndpoint.IPAddress	 = localIP; // Same as local IP
	localEndpoint.tcpPort	 = 9000;
	localEndpoint.playerName = "LocalPlayer";

	// Create endpoint with different IP
	Endpoint remoteEndpoint;
	remoteEndpoint.IPAddress  = "192.168.1.100";
	remoteEndpoint.tcpPort	  = 9001;
	remoteEndpoint.playerName = "RemotePlayer";

	// Expect observer call only once
	EXPECT_CALL(*mockObserver, onRemoteFound(::testing::_)).Times(1);

	// Add endpoints
	discoveryService->addRemoteToList(localEndpoint);
	discoveryService->addRemoteToList(remoteEndpoint);

	// Verify only the remote endpoint is stored
	Endpoint retrievedLocal	 = discoveryService->getEndpointFromIP(localIP);
	Endpoint retrievedRemote = discoveryService->getEndpointFromIP("192.168.1.100");

	EXPECT_FALSE(retrievedLocal.isValid()) << "Local endpoint should not be stored";
	EXPECT_TRUE(retrievedRemote.isValid()) << "Remote endpoint should be retrievable";
	EXPECT_EQ(retrievedRemote.playerName, "RemotePlayer") << "Remote endpoint should have correct player name";
}


TEST_F(DiscoveryServiceTests, EmptyEndpointFoundFiltering)
{
	// Init Discovery with local player
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "192.168.1.50";
	unsigned short port		  = 8080;

	bool		   initResult = discoveryService->init(playerName, localIP, port);
	ASSERT_TRUE(initResult) << "Initialization should succeed";

	// Set up in client mode to trigger observer notifications
	discoveryService->attachObserver(mockObserver);
	discoveryService->startDiscovery(DiscoveryMode::Client);

	// Create empty endpoint 
	Endpoint emptyEndpoint;
	emptyEndpoint.IPAddress  = "";
	emptyEndpoint.tcpPort	  = 0;
	emptyEndpoint.playerName = "";

	// Expect no observer call 
	EXPECT_CALL(*mockObserver, onRemoteFound(::testing::_)).Times(0);

	// Add endpoints
	EXPECT_NO_THROW(discoveryService->addRemoteToList(emptyEndpoint)) << "Storing and empty endpoint should not throw";
}


TEST_F(DiscoveryServiceTests, RemoteFoundServerNoObserverNotification)
{
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "127.0.0.1";
	unsigned short port		  = 8080;

	bool		   initResult = discoveryService->init(playerName, localIP, port);
	ASSERT_TRUE(initResult) << "Initialization should succeed";

	// Set up in SERVER mode (observers should NOT be notified)
	discoveryService->attachObserver(mockObserver);
	discoveryService->startDiscovery(DiscoveryMode::Server);

	// Create test endpoint
	Endpoint endpoint;
	endpoint.IPAddress	= "192.168.1.100";
	endpoint.tcpPort	= 9000;
	endpoint.playerName = "RemotePlayer";

	EXPECT_CALL(*mockObserver, onRemoteFound(::testing::_)).Times(0);

	// Add endpoint
	discoveryService->addRemoteToList(endpoint);

	// Endpoint should still be stored (just no observer notification)
	Endpoint retrieved = discoveryService->getEndpointFromIP("192.168.1.100");
	EXPECT_TRUE(retrieved.isValid()) << "Endpoint should be stored even in server mode";
	EXPECT_EQ(retrieved.playerName, "RemotePlayer") << "Endpoint should have correct data";
}


TEST_F(DiscoveryServiceTests, StartAndStopLifecycle)
{
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "127.0.0.1";
	unsigned short port		  = 8080;

	bool		   initResult = discoveryService->init(playerName, localIP, port);
	ASSERT_TRUE(initResult) << "Initialization should succeed";

	// Start the service
	discoveryService->start();
	EXPECT_TRUE(discoveryService->isRunning()) << "Service should be running after start";

	// Small delay to let the service actually start
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Stop the service
	discoveryService->stop();

	// Wait a bit for the stop to take effect
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_FALSE(discoveryService->isRunning()) << "Service should not be running after stop";
}


TEST_F(DiscoveryServiceTests, MultipleStartCalls)
{
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "127.0.0.1";
	unsigned short port		  = 8080;

	bool		   initResult = discoveryService->init(playerName, localIP, port);
	ASSERT_TRUE(initResult) << "Initialization should succeed";

	// Multiple start calls should not cause issues
	EXPECT_NO_THROW(discoveryService->start()) << "First start should not throw";
	EXPECT_NO_THROW(discoveryService->start()) << "Second start should not throw";
}


TEST_F(DiscoveryServiceTests, StopWithoutStart)
{
	// Should not crash when stopping without starting
	EXPECT_NO_THROW(discoveryService->stop()) << "Stop should not throw even without start";
}


TEST_F(DiscoveryServiceTests, DeinitializationCleanup)
{
	std::string	   playerName = "TestPlayer";
	std::string	   localIP	  = "127.0.0.1";
	unsigned short port		  = 8080;

	bool		   initResult = discoveryService->init(playerName, localIP, port);
	ASSERT_TRUE(initResult) << "Initialization should succeed";

	discoveryService->start();

	// Deinit should clean up properly
	EXPECT_NO_THROW(discoveryService->deinit()) << "Deinitialization should not throw";

	// After deinit, the service should not be running
	EXPECT_FALSE(discoveryService->isRunning()) << "Service should not be running after deinit";
}

} // namespace MultiplayerTests
