/*
  ==============================================================================
	Module:			MultiplayerManager Tests
	Description:    Testing the MultiplayerManager class from the multiplayer module
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <thread>
#include <chrono>

#include "MultiplayerManager.h"


namespace MultiplayerTests
{

// Mock observers for testing
class MockConnectionStatusObserver : public IConnectionStatusObserver
{
public:
	MOCK_METHOD(void, onConnectionStateChanged, (const ConnectionStatusEvent event), (override));
	MOCK_METHOD(void, onLocalPlayerChosen, (const PlayerColor localPlayer), (override));
	MOCK_METHOD(void, onRemotePlayerChosen, (PlayerColor remotePlayer), (override));
	MOCK_METHOD(void, onLocalReadyFlagSet, (const bool flag), (override));
};


class MultiplayerManagerTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		multiplayerManager = std::make_shared<MultiplayerManager>();
		mockObserver	   = std::make_shared<MockConnectionStatusObserver>();

		// Give some time for the IO context to start
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	void TearDown() override
	{
		if (multiplayerManager)
		{
			multiplayerManager->disconnect();
		}

		// Give time for cleanup
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::shared_ptr<MultiplayerManager>			  multiplayerManager;
	std::shared_ptr<MockConnectionStatusObserver> mockObserver;
};


TEST_F(MultiplayerManagerTests, InitializationWithValidIP)
{
	std::string localIP = "192.168.1.100";

	EXPECT_NO_THROW(multiplayerManager->init(localIP)) << "Initialization should not throw with valid IP";

	EXPECT_TRUE(multiplayerManager->init(localIP)) << "Initialization should return true for valid IP";
}


TEST_F(MultiplayerManagerTests, InitializationWithEmptyIP)
{
	std::string localIP = "";

	EXPECT_NO_THROW(multiplayerManager->init(localIP)) << "Initialization should not throw with emty IP";

	EXPECT_FALSE(multiplayerManager->init(localIP)) << "Initialization with empty IP should return false";
}


TEST_F(MultiplayerManagerTests, HostSession)
{
	std::string localhost = "127.0.0.1";
	multiplayerManager->init(localhost);

	bool result = multiplayerManager->hostSession();

	// Result depends on whether IP is valid and port binding succeeds
	//	LocalHost should generally succeed
	EXPECT_TRUE(result) << "Host session should succeed on localhost (127.0.0.1)";
}


TEST_F(MultiplayerManagerTests, HostSessionWithoutInit)
{
	bool result = multiplayerManager->hostSession();
	EXPECT_FALSE(result) << "Host session should fail without initialization";
}


TEST_F(MultiplayerManagerTests, StartClient)
{
	std::string localIP = "127.0.0.1";
	multiplayerManager->init(localIP);

	bool result = multiplayerManager->startClient();
	EXPECT_TRUE(result) << "Start client should succeed after initialization";
}


TEST_F(MultiplayerManagerTests, StartClientWithoutInit)
{
	bool result = multiplayerManager->startClient();
	EXPECT_FALSE(result) << "Start client should fail without initialization";
}

TEST_F(MultiplayerManagerTests, LocalPlayerChosen)
{
	multiplayerManager->attachObserver(mockObserver);

	EXPECT_CALL(*mockObserver, onLocalPlayerChosen(PlayerColor::White)).Times(1);

	multiplayerManager->localPlayerChosen(PlayerColor::White);
}


TEST_F(MultiplayerManagerTests, LocalPlayerChosenSamePlayerTwice)
{
	multiplayerManager->attachObserver(mockObserver);

	// Should only be called once, not on the second identical call
	EXPECT_CALL(*mockObserver, onLocalPlayerChosen(PlayerColor::White)).Times(1);

	multiplayerManager->localPlayerChosen(PlayerColor::White);
	multiplayerManager->localPlayerChosen(PlayerColor::White); // Should be ignored
}


TEST_F(MultiplayerManagerTests, LocalPlayerReadySet)
{
	multiplayerManager->attachObserver(mockObserver);

	EXPECT_CALL(*mockObserver, onLocalReadyFlagSet(true)).Times(1);
	multiplayerManager->localReadyFlagSet(true);
}


TEST_F(MultiplayerManagerTests, CheckIfReadyForGameBothReady)
{
	// Set both local and remote players as ready
	multiplayerManager->localReadyFlagSet(true);
	multiplayerManager->setRemotePlayerReadyForGameFlag(true);

	bool result = multiplayerManager->checkIfReadyForGame();
	EXPECT_TRUE(result) << "Should be ready for game when both players are ready";
}


TEST_F(MultiplayerManagerTests, CheckIfReadyForGameOnlyLocalReady)
{
	multiplayerManager->localReadyFlagSet(true);
	multiplayerManager->setRemotePlayerReadyForGameFlag(false);

	bool result = multiplayerManager->checkIfReadyForGame();
	EXPECT_FALSE(result) << "Should not be ready for game when only local player is ready";
}


TEST_F(MultiplayerManagerTests, CheckIfReadyForGameOnlyRemoteReady)
{
	multiplayerManager->localReadyFlagSet(false);
	multiplayerManager->setRemotePlayerReadyForGameFlag(true);

	bool result = multiplayerManager->checkIfReadyForGame();
	EXPECT_FALSE(result) << "Should not be ready for game when only remote player is ready";
}


TEST_F(MultiplayerManagerTests, CheckIfReadyForGameNeitherReady)
{
	multiplayerManager->localReadyFlagSet(false);
	multiplayerManager->setRemotePlayerReadyForGameFlag(false);

	bool result = multiplayerManager->checkIfReadyForGame();

	EXPECT_FALSE(result) << "Should not be ready for game if neither player is ready";
}


TEST_F(MultiplayerManagerTests, GetLocalPlayerNameNotEmpty)
{
	std::string localPlayerName = multiplayerManager->getLocalPlayerName();
	EXPECT_FALSE(localPlayerName.empty()) << "Local player name should not be empty";
}


TEST_F(MultiplayerManagerTests, OnRemotePlayerChosenReceived)
{
	multiplayerManager->attachObserver(mockObserver);

	// When remote chooses White, local should become Black
	EXPECT_CALL(*mockObserver, onRemotePlayerChosen(PlayerColor::Black)).Times(1);

	multiplayerManager->onRemotePlayerChosenReceived(PlayerColor::White);
}


TEST_F(MultiplayerManagerTests, JoinSessionWithoutValidEndpoint)
{
	// Without a valid remote endpoint, join should not succeed
	EXPECT_NO_THROW(multiplayerManager->joinSession()) << "Join session should not throw even without valid endpoint";
}


TEST_F(MultiplayerManagerTests, DisconnectWithoutConnection)
{
	// Should not crash when disconnecting without being connected
	EXPECT_NO_THROW(multiplayerManager->disconnect()) << "Disconnect should not throw even without connection";
}


TEST_F(MultiplayerManagerTests, Reset)
{
	std::string localIP = "127.0.0.1";
	multiplayerManager->init(localIP);
	multiplayerManager->hostSession();

	EXPECT_NO_THROW(multiplayerManager->reset()) << "Reset should not throw";
}

} // namespace MultiplayerTests
