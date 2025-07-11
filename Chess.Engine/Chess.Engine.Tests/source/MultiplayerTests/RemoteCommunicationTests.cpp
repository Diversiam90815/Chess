/*
  ==============================================================================
	Module:			RemoteCommunication Tests
	Description:    Testing the RemoteCommunication class from the multiplayer module
  ==============================================================================
*/


#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <thread>
#include <chrono>

#include "RemoteMessaging/RemoteCommunication.h"
#include "TCPConnection/TCPSession.h"


namespace MultiplayerTests
{

// Mock TCPSession for testing
class MockTCPSession : public ITCPSession
{
public:
	MOCK_METHOD(bool, isConnected, (), (const, override));
	MOCK_METHOD(bool, sendMessage, (MultiplayerMessageStruct & message), (override));
	MOCK_METHOD(void, startReadAsync, (MessageReceivedCallback callback), (override));
	MOCK_METHOD(void, stopReadAsync, (), (override));
	MOCK_METHOD(int, getBoundPort, (), (const, override));
};


class RemoteCommunicationTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		mockSession = std::make_shared<MockTCPSession>();
		remoteCom	= std::make_unique<RemoteCommunication>();
	}

	void TearDown() override
	{
		if (remoteCom && remoteCom->isInitialized())
		{
			remoteCom->deinit();
		}
	}

	std::shared_ptr<MockTCPSession>		 mockSession;
	std::unique_ptr<RemoteCommunication> remoteCom;
};



TEST_F(RemoteCommunicationTests, DefaultConstruction)
{
	EXPECT_FALSE(remoteCom->isInitialized()) << "RemoteCommunication should not be initialized by default";
}


TEST_F(RemoteCommunicationTests, InitializationWithValidSession)
{
	EXPECT_CALL(*mockSession, isConnected()).WillOnce(::testing::Return(true));

	bool result = remoteCom->init(mockSession);

	EXPECT_TRUE(result) << "Initialization should succeed with valid session";
	EXPECT_TRUE(remoteCom->isInitialized()) << "Should be initialized after successful init";
}


} // namespace MultiplayerTests
