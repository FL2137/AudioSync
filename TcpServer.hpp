#pragma once
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <qdebug.h>
#include <qobject.h>
#include <qtcpserver.h>
#include <qtcpsocket.h>
#include <string>
#include <nlohmann/json.hpp>
#include <functional>
#include <iostream>

using nlohmann::json;
using boost::asio::ip::tcp;

const std::string SERVER_ADDRESS = "192.168.1.109";
const int PORT = 3005;

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

class TcpServer : public QObject {

    Q_OBJECT

private:

    TcpServer(boost::asio::io_context& _io_context, std::string address, int port, std::function<void(std::string, std::string&)> requestParser)
        : io_context(_io_context), acceptor(io_context, tcp::endpoint(boost::asio::ip::make_address_v4(address), port))
    {
        this->requestParser = requestParser;
        startAccept(requestParser);
    }

public slots:
    
    //priv functions
private:

    

    void startAccept(std::function<void(std::string, std::string&)> f) {
        TcpConnection::pointer newConnection = TcpConnection::create(io_context, f);

        acceptor.async_accept(newConnection->socket(),
            boost::bind(&TcpServer::handleAccept, this, newConnection, boost::asio::placeholders::error));
    }

    void handleAccept(TcpConnection::pointer newConnection, const boost::system::error_code& error) {
        if (!error) {
            newConnection->start();
        }

        startAccept(requestParser);
    }

    //priv variables
private:
    boost::asio::io_context& io_context;
    tcp::acceptor acceptor;
    std::function<void(std::string, std::string&)> requestParser;
};

class Server : public QTcpServer {

    Q_OBJECT

public:
    Server(std::function<void(std::string, std::string&)> parseFun, QObject *parent);

    static std::string base64_encode(const std::string& in);

    static std::string base64_decode(const std::string& in);

public slots:
    void incomingConnection();


private:
    std::function<void(std::string, std::string&)> parseFunction;
    QTcpServer *tcpServer;
};