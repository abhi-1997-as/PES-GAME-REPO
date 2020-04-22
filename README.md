# PES-GAME-REPO

Project Description:
The project that is being developed is a piece of hardware that can interact with a software and allow a user to play a game similar to that of ‘Temple Run’, with their own physical movements. It was thought this would be important to develop especially in a situation like right now, where people are being advised to stay indoors, and this is a way for people to stay active even within their own homes. 

For this system to be used within a household the user would need to space around them to allow them to jump up, duck down and move left and right. The idea was to make the game more physically involving motivating the user to workout while playing. 
The Arduino board will be attached to the chest of the user monitoring the movement of the user. The user will have the phone displaying the interface of the game and will virtually move the character as they move in the real world. 

Requirements:
For this system to work it is likely multiple sensors of the Arduino Nano 33 BLE Sense will be used. This system will be developed using the 9-axis inertial and gesture sensors, as well as the microphone. 

The 9-axis inertial sensor will be used to identify the user’s orientation and their movements, it can do this as the 9-axis inertial sensor contains a:
-	3-axis gyroscope
-	3-axis accelerometer
-	3-axis magnetometer
The gyrometer will be used to determine the orientation of a user, and the accelerometer can be used to tell whether the user is moving left, right, up or down. 

The gesture sensor will be used for other features such as turning the virtual character within the game.

The microphone will be used to scare off in-game enemies. So the user will need to make a noise loud enough to be registered by the microphone for that action to be completed. 

The Bluetooth will be used to wirelessly send the sensor values in order to interface it with the game. 

The Arduino will be powered using batteries making the Arduino board independently powered up. 
