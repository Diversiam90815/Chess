/*
  ==============================================================================
	Module:         TCPServer
	Description:    Server implementation used for the multiplayer mode
  ==============================================================================
*/


#pragma once

#include <asio.hpp>

#include "TCPInterfaces.h"
#include "TCPSession.h"
#include "Logging.h"


using asio::ip::tcp;


class TCPServer : public ITCPServer
{
public:
	TCPServer(asio::io_context &ioContext);
	~TCPServer();

	void startAccept() override;

	int	 getBoundPort() const override;

	void setSessionHandler(SessionHandler handler) override;

	void respondToConnectionRequest(bool accepted) override;


private:
	asio::io_context		   &mIoContext;

	tcp::acceptor				mAcceptor;

	int							mBoundPort{0};

	std::shared_ptr<TCPSession> mPendingSession;

	SessionHandler				mSessionHandler;
};
