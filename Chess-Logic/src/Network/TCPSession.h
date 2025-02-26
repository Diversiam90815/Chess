/*
  ==============================================================================
	Module:         TCPSession
	Description:    Managing the socket and session used for the multiplayer mode
  ==============================================================================
*/

#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <functional>
#include <vector>
#include <json.hpp>


using json = nlohmann::json;
using boost::asio::ip::tcp;

using MessageHandler = std::function<void(const json &message)>;


class TCPSession : public boost::enable_shared_from_this<TCPSession>
{
public:
	typedef boost::shared_ptr<TCPSession> pointer;

	static pointer						  create(boost::asio::io_context &io_context) { return pointer(new TCPSession(io_context)); }

	tcp::socket							 &socket();

	void								  start();

	const int							  getBoundPort() const { return mBoundPort; }

private:
	explicit TCPSession(boost::asio::io_context &ioContext);

	void		handleRead(const boost::system::error_code &error, size_t bytesRead);

	void		handleWrite(const boost::system::error_code &error);

	tcp::socket mSocket;

	enum
	{
		max_length = 1024
	};

	char		   mData[max_length]{};

	int			   mBoundPort{0};

	MessageHandler mMessageHandler = nullptr;
};
