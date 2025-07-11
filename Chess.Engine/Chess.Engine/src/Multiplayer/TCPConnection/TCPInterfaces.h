/*
  ==============================================================================
	Module:         TCPInterfaces
	Description:    Interface for TCP modules
  ==============================================================================
*/

#pragma once

#include <functional>
#include <memory>
#include "RemoteMessaging/MultiplayerMessageStruct.h"


class ITCPSession
{
public:
	using MessageReceivedCallback								  = std::function<void(MultiplayerMessageStruct &message)>;
	using pointer												  = std::shared_ptr<ITCPSession>;

	virtual ~ITCPSession()										  = default;

	virtual bool isConnected() const							  = 0;
	virtual bool sendMessage(MultiplayerMessageStruct &message)	  = 0;
	virtual void startReadAsync(MessageReceivedCallback callback) = 0;
	virtual void stopReadAsync()								  = 0;
	virtual int	 getBoundPort() const							  = 0;
};


class ITCPServer
{
public:
	using SessionHandler								   = std::function<void(ITCPSession::pointer session)>;

	virtual ~ITCPServer()								   = default;
	virtual void startAccept()							   = 0;
	virtual int	 getBoundPort() const					   = 0;
	virtual void setSessionHandler(SessionHandler handler) = 0;
	virtual void respondToConnectionRequest(bool accepted) = 0;
};


class ITCPClient
{
public:
	using ConnectHandler												 = std::function<void(ITCPSession::pointer session)>;
	using ConnectTimeoutHandler											 = std::function<void()>;

	virtual ~ITCPClient()												 = default;
	virtual void connect(const std::string &host, unsigned short port)	 = 0;
	virtual void setConnectHandler(ConnectHandler handler)				 = 0;
	virtual void setConnectTimeoutHandler(ConnectTimeoutHandler handler) = 0;
};