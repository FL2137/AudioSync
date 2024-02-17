#pragma once
#include <nlohmann/json.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <qdebug.h>
#include "TcpServer.hpp"

using nlohmann::json;
using boost::asio::ip::tcp;

class TcpRequest;

class TcpClient {

public:
	static void send(TcpRequest request, json& response);


};

