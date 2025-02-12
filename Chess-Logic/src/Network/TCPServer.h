/*
  ==============================================================================
	Class:          TCPServer
	Description:    Server implementation used for the multiplayer mode
  ==============================================================================
*/


#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <json.hpp>


using json = nlohmann::json;
using boost::asio::ip::tcp;

using MessageHandler = std::function<void(const json &message, std::shared_ptr<tcp::socket> socket)>;


class TCPServer
{
	TCPServer();
	~TCPServer();

	void init(boost::asio::io_context &ioContext, unsigned short port);

	void startAccept();
	
	void setMessageHandler();


private:
	void					 handleAccept(std::shared_ptr<tcp::socket> socket, const boost::system::error_code &error);
	
	void					 doRead(std::shared_ptr<tcp::socket> socket);


	boost::asio::io_context &mIoContext;

	tcp::acceptor			 mAcceptor;
	
	MessageHandler			 mMessageHandler;
};
