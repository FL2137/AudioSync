#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <qdebug.h>
#include <string>
#include <nlohmann/json.hpp>

using nlohmann::json;
using boost::asio::ip::tcp;


const std::string SERVER_ADDRESS = "192.168.1.109";
const int PORT = 3005;

class TcpConnection : public boost::enable_shared_from_this<TcpConnection> {
public:
    typedef boost::shared_ptr<TcpConnection> pointer;

    static pointer create(boost::asio::io_context& ioc, std::function<void(std::string, std::string&)> requestParser) {
        return pointer(new TcpConnection(ioc, requestParser));
    }

    tcp::socket& socket() {
        return _socket;
    }


    void start() {
        boost::asio::async_read(
            _socket,
            boost::asio::buffer(readBuffer),
            boost::asio::transfer_at_least(1),
            boost::bind(&TcpConnection::handleRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        );
    }

private:

    TcpConnection(boost::asio::io_context& ioc, std::function<void(std::string, std::string&)> requestParser) : _socket(ioc) {
        this->requestParser = requestParser;
    }

    std::function<void(std::string, std::string&)> requestParser;

    void handleRead(const boost::system::error_code& error, size_t bytesTransferred) {
        if (error) {
            std::cerr << "Error receiving message from client: " << error.message() << std::endl;
        }
        else {

            request = std::string(readBuffer);

            requestParser(request, response);

            boost::asio::async_write(
                _socket,
                boost::asio::buffer(response, response.size()),
                boost::bind(&TcpConnection::handleWrite, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
            );

            std::cout << "server sent " << response << std::endl;
        }
    }

    void handleWrite(const boost::system::error_code& error, size_t bytesTransferred) {
        if (!error) {
            std::cout << "Sent via socket: " << bytesTransferred << std::endl;
        }
        else {
            std::cerr << "Error writing response" << std::endl;
            _socket.close(); //is this necesarry
        }
    }


    static const int BUFFER_SIZE = 512;
    char readBuffer[BUFFER_SIZE];
    std::string response = "notParsed";
    std::string request = "";

    tcp::socket _socket;
};

class TcpServer {

};

class TcpRequest {
public:
	std::string type;
	int uid;
	json data;


	std::string stringify() {
		json json;
		json["type"] = type;
		json["uid"] = uid;
		json["data"] = data.dump();
		return json.dump();
	}
};

