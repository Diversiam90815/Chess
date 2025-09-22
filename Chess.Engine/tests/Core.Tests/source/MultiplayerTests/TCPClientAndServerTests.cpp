/*
  ==============================================================================
	Module:			TCPServer and TCPClient Tests
	Description:    Testing the TCPServer and TCPClient classes from the multiplayer module
  ==============================================================================
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>

#include "TCPConnection/TCPServer.h"
#include "TCPConnection/TCPClient.h"


namespace MultiplayerTests
{

class TCPServerClientTests : public ::testing::Test
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
		server.reset();
		client.reset();

		ioContext->stop();
		if (ioThread.joinable())
		{
			ioThread.join();
		}
	}

	std::unique_ptr<asio::io_context> ioContext;
	std::thread						  ioThread;
	std::unique_ptr<TCPServer>		  server;
	std::unique_ptr<TCPClient>		  client;
};


TEST_F(TCPServerClientTests, TCPServerConstruction)
{
	EXPECT_NO_THROW(server = std::make_unique<TCPServer>(*ioContext)) << "TCPServer construction should not throw";

	EXPECT_NE(server, nullptr) << "TCPServer should be constructed successfully";
}


TEST_F(TCPServerClientTests, TCPClientConstruction)
{
	EXPECT_NO_THROW(client = std::make_unique<TCPClient>(*ioContext)) << "TCPClient construction should not throw";

	EXPECT_NE(client, nullptr) << "TCPClient should be constructed successfully";
}


TEST_F(TCPServerClientTests, TCPServerStartAccept)
{
	server = std::make_unique<TCPServer>(*ioContext);

	EXPECT_NO_THROW(server->startAccept()) << "Starting accept should not throw";
}


TEST_F(TCPServerClientTests, TCPServerGetBoundPort)
{
	server = std::make_unique<TCPServer>(*ioContext);
	server->startAccept();

	int port = server->getBoundPort();
	EXPECT_GT(port, 0) << "Bound port should be greater than 0 after starting accept";
	EXPECT_LE(port, 65535) << "Bound port should be within valid range";
}


TEST_F(TCPServerClientTests, TCPClientConnect)
{
	client = std::make_unique<TCPClient>(*ioContext);

	// Connecting to non-existent host should not throw immediately (it should handle the error asynchronously)
	EXPECT_NO_THROW(client->connect("127.0.0.1", 12345)) << "Client connect should not throw";
}


TEST_F(TCPServerClientTests, MultipleServerInstances)
{
	// Test that multiple servers can be created (on different ports)
	auto server1 = std::make_unique<TCPServer>(*ioContext);
	auto server2 = std::make_unique<TCPServer>(*ioContext);

	server1->startAccept();
	server2->startAccept();

	int port1 = server1->getBoundPort();
	int port2 = server2->getBoundPort();

	EXPECT_GT(port1, 0) << "First server should have valid port";
	EXPECT_GT(port2, 0) << "Second server should have valid port";
	EXPECT_NE(port1, port2) << "Servers should be on separate ports";
}


TEST_F(TCPServerClientTests, MultipleClientInstances)
{
	// Test that multiple clients can be created
	auto client1 = std::make_unique<TCPClient>(*ioContext);
	auto client2 = std::make_unique<TCPClient>(*ioContext);

	EXPECT_NE(client1, nullptr) << "First client should be created";
	EXPECT_NE(client2, nullptr) << "Second client should be created";

	// Both should be able to set handlers without interfering
	client1->setConnectHandler([](ITCPSession::pointer session) {});
	client2->setConnectHandler([](ITCPSession::pointer session) {});

	SUCCEED() << "Multiple clients created successfully";
}



} // namespace MultiplayerTests