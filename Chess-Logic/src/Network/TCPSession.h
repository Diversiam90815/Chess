/*
  ==============================================================================
	Class:          TCPSession
	Description:    Managing the socket and session used for the multiplayer mode
  ==============================================================================
*/

#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <functional>
#include <vector>
#include <json.hpp>


using json = nlohmann::json;
using boost::asio::ip::tcp;

using MessageHandler = std::function<void(const json &message)>;


class TCPSession
{
public:
	explicit TCPSession(boost::asio::io_context &ioContext);

	tcp::socket &socket();

	void		 start();

private:
	void		handleRead(const boost::system::error_code &error, size_t bytesRead);

	void		handleWrite(const boost::system::error_code &error);

	tcp::socket mSocket;

	enum
	{
		max_length = 1024
	};

	char		   mData[max_length]{};

	MessageHandler mMessageHandler;
};
