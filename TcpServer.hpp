#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <qdebug.h>
#include <string>
#include <nlohmann/json.hpp>

using nlohmann::json;
using boost::asio::ip::tcp;

class TcpConnection {

};

class TcpClient {

public:

	static void send(std::string request, std::string &response) {
		boost::asio::io_context ioc;
		tcp::socket socket(ioc);

		socket.connect(tcp::endpoint(boost::asio::ip::make_address_v4("192.168.1.109"), 3005));
		
		boost::system::error_code error;

		socket.write_some(boost::asio::buffer(request), error);

		if (error) {
			qWarning() << error.message();
		}
		
		char readBuffer[512];

		size_t size = socket.read_some(boost::asio::buffer(readBuffer), error);
		
		if (error) {
			qWarning() << error.message();
		}

		response = std::string(readBuffer);

		response.erase(response.end()-1); //for some reason one too much char is being sent

		qDebug() << response;

		socket.close();
	}

private:

};