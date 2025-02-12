/*
  ==============================================================================
	Class:          TCPServer
	Description:    Server implementation used for the multiplayer mode
  ==============================================================================
*/ 

#include "TCPServer.h"


TCPServer::TCPServer() {}


TCPServer::~TCPServer() {}


void TCPServer::init(boost::asio::io_context &ioContext, unsigned short port) {}


void TCPServer::startAccept() {}


void TCPServer::setMessageHandler() {}


void TCPServer::handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code &error) {}


void TCPServer::doRead(std::shared_ptr<tcp::socket> socket) {}
