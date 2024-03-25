#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <iostream>
#include <array>
#include <random>
#include <tuple>

using boost::asio::ip::udp;

namespace HP {



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
}