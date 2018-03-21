#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

class clientSide {

public:
		int sendSignal(std::string input);
};
