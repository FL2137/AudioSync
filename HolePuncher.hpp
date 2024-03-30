#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <iostream>
#include <array>
#include <random>
#include <tuple>
#include <memory>

using boost::asio::ip::udp;

class HolePuncher {
public:

#pragma pack(push, 1)
	struct StunRequest {
		StunRequest() {

			//generate random transaction ID

			std::random_device device;
			std::mt19937 generator(device());
			std::uniform_int_distribution<int> distribution(0, 255);

			//yh
			srand(time(NULL));

			for (int i = 0; i < 12; i++) {
				transactionID[i] = rand() % 256;
			}
		}

		const int16_t messageType = htons(0x0001);
		const int16_t messageLen = htons(0x0000);
		const int32_t magicCookie = htonl(0x2112A442);
		std::array<unsigned char, 12> transactionID = { 0 };
	};
#pragma pack(pop)


#pragma pack(push, 1)
	struct StunAttribute {
		int16_t type;
		int16_t len;
		int32_t port;
		int32_t address;
	};
#pragma pack(pop)


#pragma pack(push, 1)
	struct StunResponse {
		int16_t messageType;
		int16_t messageLen;
		int32_t magicCookie;
		std::array<unsigned char, 12> transactionID;
		StunAttribute attribXORMAPPEDADDRESS;
	};
#pragma pack(pop)



	static std::tuple<std::string, std::string> punchAhole(std::unique_ptr<udp::socket>& socket) {
		auto stunAddress = boost::asio::ip::make_address("142.250.15.127"); //address of stun2.l.google.com

		udp::endpoint stunEndpoint(stunAddress, 3748);

		StunRequest stunRequest;
		boost::system::error_code error;

		socket->send_to(boost::asio::buffer(&stunRequest, sizeof(stunRequest)), stunEndpoint, 0, error);

		if (error.value() != 0) {
			std::cout << error.message() << std::endl;
		}

		udp::endpoint remoteEp;
		std::array<char, 128> buffer;


		int received = socket->receive_from(boost::asio::buffer(buffer), remoteEp, 0, error);
		if (error.value() != 0) {
			std::cout << error.message() << std::endl;
		}

		StunResponse* srsp = reinterpret_cast<StunResponse*>(&buffer);

		if (srsp->magicCookie != stunRequest.magicCookie) {
			std::cerr << "magicCookies do not match...\n";
		}

		for (int i = 0; i < 12; i++) {
			std::cout << std::hex << (int)stunRequest.transactionID[i] << " - " << std::hex << (int)srsp->transactionID[i] << std::endl;
		}

		std::string address = "";
		std::string port = "";

		srsp->magicCookie = ntohl(srsp->magicCookie);

		//parse XOR-MAPPED-ADDRESS
		for (int i = 0; i < 4; i++) {
			address += std::to_string(((srsp->magicCookie >> (24 - (i * 8))) & 0xff) ^ ((ntohl(srsp->attribXORMAPPEDADDRESS.address) >> 24 - (i * 8)) & 0xff)) + ".";
		}
		address.pop_back();//get rid of the last dot

		port = std::to_string((ntohl(srsp->attribXORMAPPEDADDRESS.port) & 0xffFF) ^ (srsp->magicCookie >> 16));

		return std::make_tuple(address, port);


	}
};