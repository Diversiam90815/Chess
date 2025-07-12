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








} // namespace MultiplayerTests
