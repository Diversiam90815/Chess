/*
  ==============================================================================
	Module:			Multiplayer Integration Tests
	Description:    Integration tests for multiplayer components working together
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>

#include "MultiplayerManager.h"
#include "Discovery/DiscoveryService.h"
#include "RemoteMessaging/RemoteCommunication.h"
#include "TCPConnection/TCPServer.h"
#include "TCPConnection/TCPClient.h"
#include "TCPConnection/TCPSession.h"


namespace MultiplayerTests
{

class MockIntegrationObserver : public IConnectionStatusObserver, public IDiscoveryObserver
{
public:
	MOCK_METHOD(void, onConnectionStateChanged, (const ConnectionStatusEvent event), (override));
	MOCK_METHOD(void, onLocalPlayerChosen, (const Side localPlayer), (override));
	MOCK_METHOD(void, onRemotePlayerChosen, (Side remotePlayer), (override));
	MOCK_METHOD(void, onLocalReadyFlagSet, (const bool flag), (override));
	MOCK_METHOD(void, onRemoteFound, (const Endpoint &remote), (override));
};


class MultiplayerIntegrationTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		mockObserver = std::make_shared<MockIntegrationObserver>();

		// Give some time for any background operations to settle
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	void TearDown() override
	{
		// Clean up any resources
		if (hostManager)
		{
			hostManager->disconnect();
		}
		if (clientManager)
		{
			clientManager->disconnect();
		}

		// Give time for cleanup
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}

	std::shared_ptr<MultiplayerManager>		 hostManager;
	std::shared_ptr<MultiplayerManager>		 clientManager;
	std::shared_ptr<MockIntegrationObserver> mockObserver;
};


TEST_F(MultiplayerIntegrationTests, HostAndClientDiscoveryFlow)
{
	// Create and initialize host and client managers
	hostManager			= std::make_shared<MultiplayerManager>();
	clientManager		= std::make_shared<MultiplayerManager>();

	std::string localIP = "127.0.0.1";
	hostManager->init(localIP);
	clientManager->init(localIP);

	clientManager->attachObserver(mockObserver);

	// Expect the client to find the host
	EXPECT_CALL(*mockObserver, onConnectionStateChanged(::testing::_)).Times(::testing::AtLeast(1));

	// Start host session
	bool hostResult = hostManager->hostSession();
	EXPECT_TRUE(hostResult) << "Host should start successfully";

	// Start client discovery
	bool clientResult = clientManager->startClient();
	EXPECT_TRUE(clientResult) << "Client should start successfully";

	// Give time for discovery process
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	SUCCEED() << "Host and client discovery flow completed without crashes";
}


TEST_F(MultiplayerIntegrationTests, MultiplayerManagerLifecycle)
{
	// Test complete lifecycle : init -> host -> disconnect -> reset

	hostManager			   = std::make_shared<MultiplayerManager>();

	// Init
	std::string localIP	   = "127.0.0.1";
	bool		initResult = hostManager->init(localIP);
	hostManager->attachObserver(mockObserver);

	EXPECT_TRUE(initResult) << "MultiplayerManager should initialize successfully";

	// Host a session
	bool hostResult = hostManager->hostSession();
	EXPECT_TRUE(hostResult) << "Hosting session should succeed";

	// Set local player and ready state
	hostManager->localPlayerChosen(Side::White);
	hostManager->localReadyFlagSet(true);

	// Simulate some operations
	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	// Disconnect
	EXPECT_NO_THROW(hostManager->disconnect()) << "Disconnect should not throw";

	// Reset
	EXPECT_NO_THROW(hostManager->reset()) << "Reset should not throw";

	SUCCEED() << "Complete multiplayer manager lifecycle completed successfully";
}


TEST_F(MultiplayerIntegrationTests, DiscoveryServiceIntegration)
{
	// Test Discovery with actual MultiplayerManager integration

	// Initialize both managers
	hostManager			 = std::make_shared<MultiplayerManager>();
	clientManager		 = std::make_shared<MultiplayerManager>();

	std::string hostIP	 = "192.168.1.110";
	std::string clientIP = "192.168.1.100";

	hostManager->init(hostIP);
	clientManager->init(clientIP);

	// Start host which should start discovery in server mode
	bool hostStarted = hostManager->hostSession();
	EXPECT_TRUE(hostStarted) << "Host should start successfully";

	// Start client which should start discovery in client mode
	bool clientStarted = clientManager->startClient();
	EXPECT_TRUE(clientStarted) << "Client should start successfully";

	// Let discovery run for a while
	std::this_thread::sleep_for(std::chrono::milliseconds(800));

	// Both should be operational without crashes
	SUCCEED() << "Discovery service integration completed successfully";
}


TEST_F(MultiplayerIntegrationTests, RemoteCommunicationIntegration)
{
	// Test RemoteCommunication integration by setting up a host
	hostManager = std::make_shared<MultiplayerManager>();
	hostManager->init("127.0.0.1");
	hostManager->setInternalObservers();

	// Start host which sets up RemoteCommunication internally
	bool result = hostManager->hostSession();
	EXPECT_TRUE(result) << "Host should start and set up communication";

	// Test sending responses through the system
	EXPECT_NO_THROW(hostManager->sendConnectResponse(true, "")) << "Sending connect response should not throw";

	EXPECT_NO_THROW(hostManager->sendConnectResponse(false, "Test rejection")) << "Sending rejection response should not throw";

	// Let the system process for a bit
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}


TEST_F(MultiplayerIntegrationTests, PlayerReadyStateFlow)
{
	hostManager = std::make_shared<MultiplayerManager>();
	hostManager->init("127.0.0.1");
	hostManager->attachObserver(mockObserver);

	// Test the ready state flow
	EXPECT_CALL(*mockObserver, onLocalPlayerChosen(Side::White)).Times(1);
	EXPECT_CALL(*mockObserver, onLocalReadyFlagSet(true)).Times(1);

	// Simulate game setup flow
	hostManager->localPlayerChosen(Side::White);
	hostManager->localReadyFlagSet(true);

	// Test ready state checking
	hostManager->setRemotePlayerReadyForGameFlag(true);
	bool bothReady = hostManager->checkIfReadyForGame();
	EXPECT_TRUE(bothReady) << "Both players should be ready";

	// Test not ready state
	hostManager->setRemotePlayerReadyForGameFlag(false);
	bool notReady = hostManager->checkIfReadyForGame();
	EXPECT_FALSE(notReady) << "Should not be ready when remote is not ready";
}


TEST_F(MultiplayerIntegrationTests, RemotePlayerChosenFlow)
{
	hostManager = std::make_shared<MultiplayerManager>();
	hostManager->init("127.0.0.1");
	hostManager->attachObserver(mockObserver);

	// When remote chooses White, local should become Black
	EXPECT_CALL(*mockObserver, onRemotePlayerChosen(Side::Black)).Times(1);

	hostManager->onRemotePlayerChosenReceived(Side::White);
}



TEST_F(MultiplayerIntegrationTests, ConnectionStateTransitions)
{
	hostManager = std::make_shared<MultiplayerManager>();
	hostManager->init("127.0.0.1");
	hostManager->attachObserver(mockObserver);

	// Expect connection state changes during the flow
	EXPECT_CALL(*mockObserver, onConnectionStateChanged(::testing::_)).Times(::testing::AtLeast(1));

	// Start hosting which should trigger state changes
	bool result = hostManager->hostSession();
	EXPECT_TRUE(result);

	// Give time for state changes to propagate
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}


TEST_F(MultiplayerIntegrationTests, MultipleManagerInstances)
{
	// Test that multiple MultiplayerManager instances can coexist
	auto manager1 = std::make_shared<MultiplayerManager>();
	auto manager2 = std::make_shared<MultiplayerManager>();
	auto manager3 = std::make_shared<MultiplayerManager>();

	// Initialize all on different "networks" (same IP but different behavior)
	manager1->init("127.0.0.1");
	manager2->init("127.0.0.2");
	manager3->init("127.0.0.3");

	// Start one as host
	bool hostResult = manager1->hostSession();
	EXPECT_TRUE(hostResult) << "First manager should host successfully";

	// Start others as clients
	bool client1Result = manager2->startClient();
	bool client2Result = manager3->startClient();
	EXPECT_TRUE(client1Result) << "Second manager should start client successfully";
	EXPECT_TRUE(client2Result) << "Third manager should start client successfully";

	// Let them run for a bit
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	// Clean up
	manager1->disconnect();
	manager2->disconnect();
	manager3->disconnect();

	SUCCEED() << "Multiple manager instances handled successfully";
}


TEST_F(MultiplayerIntegrationTests, NetworkErrorHandling)
{
	hostManager		= std::make_shared<MultiplayerManager>();

	// Test initialization with invalid IP
	bool initResult = hostManager->init("");
	EXPECT_FALSE(initResult) << "Initialization with empty IP should fail";

	// Test hosting without initialization
	bool hostResult = hostManager->hostSession();
	EXPECT_FALSE(hostResult) << "Hosting without initialization should fail";

	// Test client start without initialization
	bool clientResult = hostManager->startClient();
	EXPECT_FALSE(clientResult) << "Starting client without initialization should fail";

	// Test operations on uninitialized manager
	EXPECT_NO_THROW(hostManager->disconnect()) << "Disconnect should not throw on uninitialized manager";
	EXPECT_NO_THROW(hostManager->reset()) << "Reset should not throw on uninitialized manager";
}


TEST_F(MultiplayerIntegrationTests, ConcurrentOperations)
{
	// Test concurrent operations on MultiplayerManager
	hostManager = std::make_shared<MultiplayerManager>();
	hostManager->init("127.0.0.1");

	// Start host
	bool result = hostManager->hostSession();
	EXPECT_TRUE(result);

	// Perform multiple concurrent operations
	std::vector<std::thread> workers;

	// Worker 1: Player color changes
	workers.emplace_back(
		[this]()
		{
			for (int i = 0; i < 5; ++i)
			{
				hostManager->localPlayerChosen(i % 2 == 0 ? Side::White : Side::Black);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		});

	// Worker 2: Ready flag changes
	workers.emplace_back(
		[this]()
		{
			for (int i = 0; i < 5; ++i)
			{
				hostManager->localReadyFlagSet(i % 2 == 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		});

	// Worker 3: Remote state simulation
	workers.emplace_back(
		[this]()
		{
			for (int i = 0; i < 5; ++i)
			{
				hostManager->setRemotePlayerReadyForGameFlag(i % 2 == 0);
				hostManager->checkIfReadyForGame();
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		});

	// Wait for all workers to complete
	for (auto &worker : workers)
	{
		worker.join();
	}

	SUCCEED() << "Concurrent operations completed without issues";
}



} // namespace MultiplayerTests