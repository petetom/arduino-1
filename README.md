arduino
=======

Code used to control the functions of the Arduino board on the robot.  This code will collect the 2 one-wire 
temperature sensor data and the calculte the vaccuum motor RPM's from the optical sensor on the motor gear. 

This code will receive and send i2C data.  Data received will be used to light up the LED ring either red or green.
It will send back RPM or temperature data to the cRio.
