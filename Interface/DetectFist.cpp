// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <algorithm>
#include  <iostream>
#include <stdio.h>
#include "clientSide.h"

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
		: onArm(false), isUnlocked(false), roll_w(0), pitch_w(0), yaw_w(0), currentPose(), prev_pitch(100), prev_roll(300), prev_yaw(200)
	{
	}

	// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
	void onUnpair(myo::Myo* myo, uint64_t timestamp)
	{
		// We've lost a Myo.
		// Let's clean up some leftover state.
		roll_w = 0;
		pitch_w = 0;
		yaw_w = 0;
		prev_roll = 0;
		prev_yaw = 0;
		prev_pitch = 0;
		onArm = false;
		isUnlocked = false;
	}

	void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
	{
		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::max;
		using std::min;
		// Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
		float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
			1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
		float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
		// Convert the floating point angles in radians to a scale from 0 to 18.
		roll_w = static_cast<int>((roll + (float)M_PI) / (M_PI * 2.0f) * 18);
		pitch_w = static_cast<int>((pitch + (float)M_PI / 2.0f) / M_PI * 18);
		yaw_w = static_cast<int>((yaw + (float)M_PI) / (M_PI * 2.0f) * 18);
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
	void detectPose(){
		clientSide* client = new clientSide();
		myo::Pose pose;
		//Print the current Pose
		//std::cout << currentPose << std::endl;
		std::string currPose = currentPose.toString();
		std::string finalString = "";
		//If the pose has changed, send the signal activating the command.
		if (previousPose != currentPose) {
			std::cout << currentPose << std::endl;
			//Adding Parentheses
			finalString += currPose;
			finalString += ")";
			std::cout << finalString << std::endl;
			int result = client->sendSignal(finalString);
			std::cout << "called sendSignal, Result:" << result << std::endl;
			previousPose = currentPose;
		}
	
	}

	void detectMovement() {
		clientSide* client = new clientSide();
		std::string finalString = "";
		if (prev_yaw != yaw_w && prev_pitch != pitch_w && prev_roll != roll_w) {
			//This is a really stupid way of doing this!!!
			finalString += std::to_string(yaw_w);
			finalString += ",";
			finalString += std::to_string(pitch_w);
			finalString += ",";
			finalString += std::to_string(roll_w);
			finalString += "}";
			std::cout << finalString << std::endl;
			int result = client->sendSignal(finalString);
			std::cout << "called sendSignal, Result:" << result << std::endl;
			prev_yaw = yaw_w;
			prev_pitch = pitch_w;
			prev_roll = roll_w;
		}
	}

	
	// We define this function to print the current values that were updated by the on...() functions above.
	
	// These values are set by onArmSync() and onArmUnsync() above.
	bool onArm;
	myo::Arm whichArm;

	// This is set by onUnlocked() and onLocked() above.
	bool isUnlocked;

	// These values are set by onOrientationData() and onPose() above.
	myo::Pose currentPose;
	myo::Pose previousPose;
	std::string postRecord;
	int roll_w, pitch_w, yaw_w;
	int prev_roll, prev_pitch, prev_yaw;
};

int main(int argc, char** argv)
{	
	/*
	DetectFist collector;
	collector.detectMovement();
	std::cout << "Signal Sent" << std::endl;
	
	while (true) {

	}
	*/
	
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
			hub.run(1000 / 10);
			collector.detectPose();

		}
		

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
	

}
