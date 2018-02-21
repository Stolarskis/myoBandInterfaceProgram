# myoBandInterfaceProgram


## Purpose of Program

* Detect Spatical and Gesture data from a Myo Band
* Send that data to a Staubli robotic arm TX-40

## Current Functionality

* Detect Fist: Detect if a fist has been made
  * To do this I am using [Thalmic Labs' SDK](https://developer.thalmic.com/docs/api_reference/platform/the-sdk.html) and their
  sample [Hello Myo](https://developer.thalmic.com/docs/api_reference/platform/hello-myo_8cpp-example.html) program. 
* (In Progress) To send the data I am using Winsock socket library by Microsoft. Currently still in the process of debugging, once its finished
however, the client should be able to send the message "(fist)". This would be sent to the Server on the Staubli Arm's Cabinet, where it would
then issue the command to close the claw. 

