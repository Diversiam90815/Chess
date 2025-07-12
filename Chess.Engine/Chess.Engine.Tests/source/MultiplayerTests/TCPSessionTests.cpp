/*
  ==============================================================================
	Module:			TCPSession Tests
	Description:    Testing the TCPSession class from the multiplayer module
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <thread>
#include <chrono>

#include "TCPConnection/TCPSession.h"


class TCPSessionTests : public ::testing::Test
{
protected:
	void SetUp() override
	{
		ioContext = std::make_unique<asio::io_context>();

		// Start IO context in a separate thread
		ioThread  = std::thread([this]() { ioContext->run(); });

		// Give time for IO context to start
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	void TearDown() override
	{
		ioContext->stop();
		if (ioThread.joinable())
		{
			ioThread.join();
		}
	}

	std::unique_ptr<asio::io_context> ioContext;
	std::thread						  ioThread;
};


TEST_F(TCPSessionTests, CreateSession)
{
	auto session = TCPSession::create(*ioContext);

	EXPECT_NE(session, nullptr) << "Created session should not be null";
	EXPECT_FALSE(session->isConnected()) << "New session should not be connected";
}


TEST_F(TCPSessionTests, CreateSessionFromSocket)
{
	asio::ip::tcp::socket socket(*ioContext);
	auto				  session = TCPSession::create(std::move(socket));

	EXPECT_NE(session, nullptr) << "Created session from socket should not be null";
	EXPECT_FALSE(session->isConnected()) << "New session from socket should not be connected";
}


TEST_F(TCPSessionTests, GetBoundPort)
{
	auto session = TCPSession::create(*ioContext);

	int	 port	 = session->getBoundPort();
	EXPECT_GE(port, 0) << "Bound port should be non-negative";
}


TEST_F(TCPSessionTests, SendMessageWhenNotConnected)
{
	auto					 session = TCPSession::create(*ioContext);

	MultiplayerMessageStruct message;
	message.type = MultiplayerMessageType::Move;
	message.data = {0x01, 0x02, 0x03};

	bool result	 = session->sendMessage(message);
	EXPECT_FALSE(result) << "Sending message should fail when not connected";
}


TEST_F(TCPSessionTests, StartStopReadAsync)
{
	auto session	  = TCPSession::create(*ioContext);

	auto mockCallback = [](MultiplayerMessageStruct &message)
	{
		// Do nothing for test
	};

	EXPECT_NO_THROW(session->startReadAsync(mockCallback)) << "Starting async read should not throw";

	EXPECT_NO_THROW(session->stopReadAsync()) << "Stopping async read should not throw";
}


TEST_F(TCPSessionTests, StopReadAsyncWithoutStart)
{
	auto session = TCPSession::create(*ioContext);

	EXPECT_NO_THROW(session->stopReadAsync()) << "Stopping async read without starting should not throw";
}


TEST_F(TCPSessionTests, MultipleStartReadAsync)
{
	auto session  = TCPSession::create(*ioContext);

	auto callback = [](MultiplayerMessageStruct &message) {};

	// Multiple start calls should not cause issues
	EXPECT_NO_THROW(session->startReadAsync(callback)) << "First start async read should not throw";

	EXPECT_NO_THROW(session->startReadAsync(callback)) << "Second start async read should not throw";
}


TEST_F(TCPSessionTests, SessionPointerType)
{
	auto				session = TCPSession::create(*ioContext);

	// Test that the pointer type works correctly
	TCPSession::pointer ptr		= session;
	EXPECT_EQ(ptr, session) << "Pointer assignment should work correctly";
}


TEST_F(TCPSessionTests, SocketAccess)
{
	auto session = TCPSession::create(*ioContext);

	// Test that we can access the socket (this verifies the interface)
	EXPECT_NO_THROW(auto &socket = session->socket()) << "Socket access should not throw";
}


