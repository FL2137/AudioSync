#pragma once

#include <qwebsocket.h>
#include <qdebug.h>
#include <qobject.h>
#include <qstring.h>
#include <functional>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "nlohmann/json.hpp"

using nlohmann::json;

class WebsocketClient : public QObject {
	Q_OBJECT

public:
	WebsocketClient(const QUrl& url, QObject* parent = nullptr);
	static void makeRequest(QWebSocket* socket, QObject* sender, std::function<void(const QString& response)>* handler, const json request);

signals:
	void closed();

private slots:
	void onConnected();
	void onTextMessageReceived(QString message);

private:
	QWebSocket socket;
	bool debug;
};

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace _net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;

class BeastClient {

public:

	static void syncBeast(const std::string& request, std::string& response) {
		_net::io_context ioc;
		tcp::resolver resolver{ ioc };

		websocket::stream<tcp::socket> ws{ ioc };

		tcp::endpoint remoteHost(_net::ip::make_address("192.168.1.109"), 3005);

		const auto results = resolver.resolve(remoteHost);

		auto ep = _net::connect(ws.next_layer(), results);

		std::string host = "192.168.1.109:3005";

		ws.set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
			req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-coro");
		}));

		ws.handshake(host, "/");

		ws.write(_net::buffer(request));

		beast::flat_buffer buffer;

		ws.read(buffer);

		ws.close(websocket::close_code::normal);
		response = beast::buffers_to_string(buffer.data());
	}
};