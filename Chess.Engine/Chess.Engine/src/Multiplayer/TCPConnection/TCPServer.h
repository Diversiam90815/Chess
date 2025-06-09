/*
  ==============================================================================
	Module:         TCPServer
	Description:    Server implementation used for the multiplayer mode
  ==============================================================================
*/


#pragma once

#include <asio.hpp>
#include <functional>

#include "TCPSession.h"
#include "Logging.h"


using asio::ip::tcp;

using SessionHandler		   = std::function<void(std::shared_ptr<TCPSession> session)>; // Callback invoked when a new session is accepted

using ConnectionRequestHandler = std::function<void(const std::string &remoteIPv4)>;


class TCPServer
{
public:
	TCPServer(asio::io_context &ioContext);
	~TCPServer();

	void	  startAccept();

	const int getBoundPort() const;

	void	  setSessionHandler(SessionHandler handler);
	void	  setConnectionRequestHandler(ConnectionRequestHandler handler);

	void	  respondToConnectionRequest(bool accepted);


private:

	asio::io_context		   &mIoContext;

	tcp::acceptor				mAcceptor;

	int							mBoundPort{0};

	std::shared_ptr<TCPSession> mPendingSession;

	SessionHandler				mSessionHandler;
	ConnectionRequestHandler	mConnectionRequestHandler;
};
