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
	ep.IPAddress  = invalidIP;
	ep.playerName = "TestPlayer";
	ep.tcpPort	  = 8080;
	discoveryService->addRemoteToList(ep);

	Endpoint endpoint = discoveryService->getEndpointFromIP(invalidIP);

	EXPECT_FALSE(endpoint.isValid()) << "Should return invalid endpoint for empty IP";
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
