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


/**
 * @brief	Implements a TCP server that listens for and accepts incoming connections,
 *			creating TCPSession instances for each accepted socket.
 */
class TCPServer : public ITCPServer
{
public:
	TCPServer(asio::io_context &ioContext);
	~TCPServer();

	/**
	 * @brief	Begin asynchronous accept loop. Each accepted connection invokes session handler.
	 */
	void startAccept() override;

	int	 getBoundPort() const override;

	/**
	 * @brief	Register callback for accepted session creation.
	 */
	void setSessionHandler(SessionHandler handler) override;

	/**
	 * @brief	Respond to a pending connection request (e.g., handshake).
	 * @param	accepted -> True to allow session progression, false to close.
	 */
	void respondToConnectionRequest(bool accepted) override;


private:
	asio::io_context		   &mIoContext;

	tcp::acceptor				mAcceptor;

	int							mBoundPort{0};

	std::shared_ptr<TCPSession> mPendingSession;

	SessionHandler				mSessionHandler;
};
