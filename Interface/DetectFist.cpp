// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <winsock2.h>
#include  <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")
#define PORT 8080

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.
class DetectFist : public myo::DeviceListener {
public:
	DetectFist()
		: onArm(false), isUnlocked(false), currentPose()
	{
	}

	// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
	void onUnpair(myo::Myo* myo, uint64_t timestamp)
	{
		// We've lost a Myo.
		// Let's clean up some leftover state.
		onArm = false;
		isUnlocked = false;
	}

	// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
	// making a fist, or not making a fist anymore.
	void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
	{
		currentPose = pose;

		if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
			// Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
			// Myo becoming locked.
			myo->unlock(myo::Myo::unlockHold);

			// Notify the Myo that the pose has resulted in an action, in this case changing
			// the text on the screen. The Myo will vibrate.
			myo->notifyUserAction();
		}
		else {
			// Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
			// are being performed, but lock after inactivity.
			myo->unlock(myo::Myo::unlockTimed);
		}
	}
	// onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
	void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection, float rotation,
		myo::WarmupState warmupState)
	{
		onArm = true;
		whichArm = arm;
	}
	
	// onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
	void onArmUnsync(myo::Myo* myo, uint64_t timestamp)
	{
		onArm = false;
	}
	void onLock(myo::Myo* myo, uint64_t timestamp)
	{
		isUnlocked = false;
	}

	bool DetectPose(){
		//std::cout << currentPose << std::endl;
		std::string currPose = currentPose.toString();

		if (currPose == "fist") {
			

			std::cout << "FIST DETECTED" << std::endl;
			return true;
		}


		

	}

	int sendSignal(std::string input) {

		WSADATA wsaData;

		//Init Winsock Library
		int iResult;
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

		iResult = getaddrinfo("192.168.0.254", "5070", &hints, &result);
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


		std::string sendbuf = input;

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
	// We define this function to print the current values that were updated by the on...() functions above.
	
	// These values are set by onArmSync() and onArmUnsync() above.
	bool onArm;
	myo::Arm whichArm;

	// This is set by onUnlocked() and onLocked() above.
	bool isUnlocked;

	// These values are set by onOrientationData() and onPose() above.
	myo::Pose currentPose;
};

int main(int argc, char** argv)
{
	// We catch any exceptions that might occur below -- see the catch statement for more details.
	try {
		
		myo::Hub hub("com.example.hello-myo");

		std::cout << "Attempting to find a Myo..." << std::endl;

		
		myo::Myo* myo = hub.waitForMyo(10000);

		
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}

		
		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

		DetectFist collector;

		hub.addListener(&collector);

		while (1) {
			hub.run(1000 / 20);
			bool check = false;
			check = collector.DetectPose();
			if (check == true) {
				std::string input = "Fist Detected!)";
				collector.sendSignal(input);
				break;
			}
		}

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
}
