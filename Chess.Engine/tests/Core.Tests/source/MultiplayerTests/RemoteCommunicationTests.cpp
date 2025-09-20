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
	EXPECT_CALL(*mockSession, isConnected()).WillRepeatedly(::testing::Return(true));

	bool result = remoteCom->init(mockSession);

	EXPECT_TRUE(result) << "Initialization should succeed with valid session";
	EXPECT_TRUE(remoteCom->isInitialized()) << "Should be initialized after successful init";
}



TEST_F(RemoteCommunicationTests, InitializationWithNullSession)
{
	bool result = remoteCom->init(nullptr);

	EXPECT_FALSE(result) << "Initialization should fail with null session";
	EXPECT_FALSE(remoteCom->isInitialized()) << "Should not be initialized after failed init";
}


TEST_F(RemoteCommunicationTests, InitializationWithDisconnectedSession)
{
	EXPECT_CALL(*mockSession, isConnected()).WillOnce(::testing::Return(false));

	bool result = remoteCom->init(mockSession);

	EXPECT_FALSE(result) << "Initialization should fail with disconnected session";
	EXPECT_FALSE(remoteCom->isInitialized()) << "Should not be initialized after failed init";
}


TEST_F(RemoteCommunicationTests, Deinitialization)
{
	EXPECT_CALL(*mockSession, isConnected()).WillRepeatedly(::testing::Return(true));
	EXPECT_CALL(*mockSession, stopReadAsync()).Times(1);

	remoteCom->init(mockSession);
	EXPECT_TRUE(remoteCom->isInitialized()) << "Should be initialized";

	remoteCom->deinit();
	EXPECT_FALSE(remoteCom->isInitialized()) << "Should not be initialized after deinit";
}


TEST_F(RemoteCommunicationTests, StartAndStop)
{
	EXPECT_CALL(*mockSession, isConnected()).WillRepeatedly(::testing::Return(true));
	EXPECT_CALL(*mockSession, startReadAsync(::testing::_)).Times(1);
	EXPECT_CALL(*mockSession, stopReadAsync()).Times(::testing::AtLeast(1));

	remoteCom->init(mockSession);

	EXPECT_NO_THROW(remoteCom->start()) << "Start should not throw";
	EXPECT_NO_THROW(remoteCom->stop()) << "Stop should not throw";
}


TEST_F(RemoteCommunicationTests, WriteMessage)
{
	EXPECT_CALL(*mockSession, isConnected()).WillRepeatedly(::testing::Return(true));
	EXPECT_CALL(*mockSession, sendMessage(::testing::_)).WillOnce(::testing::Return(true));

	remoteCom->init(mockSession);

	std::vector<uint8_t> testData = {0x01, 0x02, 0x03};
	EXPECT_NO_THROW(remoteCom->write(MultiplayerMessageType::Move, testData)) << "Write should not throw with valid data";
}


TEST_F(RemoteCommunicationTests, WriteMessageWhenNotInitialized)
{
	std::vector<uint8_t> testData = {0x01, 0x02, 0x03};

	// Should not crash when not initialized
	EXPECT_NO_THROW(remoteCom->write(MultiplayerMessageType::Move, testData)) << "Write should not throw even when not initialized";
}


TEST_F(RemoteCommunicationTests, OnSendMessageCallback)
{
	EXPECT_CALL(*mockSession, isConnected()).WillRepeatedly(::testing::Return(true));

	remoteCom->init(mockSession);

	std::vector<uint8_t> testData = {0x10, 0x20, 0x30};

	EXPECT_NO_THROW(remoteCom->onSendMessage(MultiplayerMessageType::Chat, testData)) << "onSendMessage callback should not throw";
}


TEST_F(RemoteCommunicationTests, ReadMessageWhenEmpty)
{
	EXPECT_CALL(*mockSession, isConnected()).WillRepeatedly(::testing::Return(true));

	remoteCom->init(mockSession);

	MultiplayerMessageType type;
	std::vector<uint8_t>   data;

	bool				   result = remoteCom->read(type, data);
	EXPECT_FALSE(result) << "Read should return false when no messages available";
}


TEST_F(RemoteCommunicationTests, ReceiveMessagesWhenNotInitialized)
{
	bool result = remoteCom->receiveMessages();
	EXPECT_FALSE(result) << "receiveMessages should return false when not initialized";
}


TEST_F(RemoteCommunicationTests, SendMessagesWhenNotInitialized)
{
	bool result = remoteCom->sendMessages();
	EXPECT_FALSE(result) << "sendMessages should return false when not initialized";
}


TEST_F(RemoteCommunicationTests, MultipleInitializeAttempts)
{
	EXPECT_CALL(*mockSession, isConnected()).WillRepeatedly(::testing::Return(true));
	EXPECT_CALL(*mockSession, stopReadAsync()).Times(::testing::AtLeast(1));

	// First initialization
	bool result1 = remoteCom->init(mockSession);
	EXPECT_TRUE(result1) << "First initialization should succeed";
	EXPECT_TRUE(remoteCom->isInitialized()) << "Should be initialized";

	// Second initialization should replace the first
	bool result2 = remoteCom->init(mockSession);
	EXPECT_TRUE(result2) << "Second initialization should succeed";
	EXPECT_TRUE(remoteCom->isInitialized()) << "Should still be initialized";
}


TEST_F(RemoteCommunicationTests, StopWithoutStart)
{
	EXPECT_CALL(*mockSession, isConnected()).WillRepeatedly(::testing::Return(true));

	remoteCom->init(mockSession);

	// Should not crash when stopping without starting
	EXPECT_NO_THROW(remoteCom->stop()) << "Stop should not throw even without start";
}

} // namespace MultiplayerTests
