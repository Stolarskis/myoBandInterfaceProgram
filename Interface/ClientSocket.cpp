#include  <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_PORT  "5070"
#define Default_Address  "127.0.0.1"

class ClientSocket {

	WSADATA wsaData;

	//Init Winsock Library
	int iResult;

public:
	int sendSignal() {
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed: %d\n", iResult);
			return 1;
		}

		struct addrinfo *result = NULL,
			*ptr = NULL,
			hints;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		//std::cout << hints.ai_family << std::endl;

		iResult = getaddrinfo(Default_Address, DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {

			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return 1;
		}
		SOCKET ConnectSocket = INVALID_SOCKET;


		// Attempt to connect to the first address returned by
		// the call to getaddrinfo
		ptr = result;

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError());

			freeaddrinfo(result);
			WSACleanup();

			std::cout << "Breaks at Connecting Socket to Server \n" << std::endl;
			return 1;
		}

		// Connect to server.
		std::cout << ConnectSocket << std::endl;
		std::cout << ConnectSocket << std::endl;
		std::cout << (int)ptr->ai_addrlen << std::endl;
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
		}

		// Should really try the next address returned by getaddrinfo
		// if the connect call failed
		// But for this simple example we just free the resources
		// returned by getaddrinfo and print an error message

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET) {
			printf("Unable to connect to server!\n");
			WSACleanup();
			std::cout << "Breaks at Connecting Socket to Server \n" << std::endl;
			return 1;
		}


		std::string sendbuf = "this is a test";

		// Send an initial buffer
		iResult = send(ConnectSocket, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		printf("Bytes Sent: %ld\n", iResult);

		printf("Stopping Point\n");


		return 0;
	}
};
