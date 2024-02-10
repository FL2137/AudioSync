#include "TcpClient.hpp"


void TcpClient::send(TcpRequest request, std::string& response) {

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

	response = std::string(readBuffer, 512);

	if (error) {
		qWarning() << error.message();
	}
	qDebug() << response;

	socket.close();

}