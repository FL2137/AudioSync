#include "TcpClient.hpp"


void TcpClient::send(TcpRequest request, json& response) {

	std::string SERVER_ADDRESS = "192.168.1.109";
	const int PORT = 3005;

	boost::asio::io_context ioc;
	tcp::socket socket(ioc);

	socket.connect(tcp::endpoint(boost::asio::ip::make_address_v4(SERVER_ADDRESS), PORT));

	boost::system::error_code error;

	socket.write_some(boost::asio::buffer(request.stringify()), error);

	if (error) {
		qWarning() << error.message();
	}

	char readBuffer[512];

	size_t size = socket.read_some(boost::asio::buffer(readBuffer, 512), error);

	//VERY BAD but works for now
	for (int i = 0; i < 512; i++) {
		if (readBuffer[i] == '}') {
			readBuffer[i + 1] = '\0';
			break;
		}
	}

	std::string responseStr = std::string(readBuffer);
	if (json::accept(responseStr))
		response = json::parse(responseStr);
	else {
		response = json("");
		qWarning() << "Error json string";
	}



	if (error) {
		qWarning() << error.message();
	}

	socket.close();
}