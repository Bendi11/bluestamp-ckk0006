#include <Servo.h>

//The digital pin number that the base servo is connected to.
//We name it 'azimuth' to be more clear, but this is just the lowest servo on the arm.
static const unsigned PIN_AZIMUTH = 4;

//We have two servo motors in the actual arm that control the height of the arm,
//which we'll call the 'elevation' servos.
//The base elevation servo is the one just above the base plate.
static const unsigned PIN_BASE_ELEVATION = 5;

//The wrist elevation servo is located behind the main hand, and controls the angle that the hand is at.
static const unsigned PIN_WRIST_ELEVATION = 6;

//The claw servo is on the hand of the arm, and can open and close the two claws when angles are written to it.
static const unsigned PIN_CLAW = 7;

//We name the analog input pins that each joystick axis is connected to here,
//so that it's more clear than just writing A0 everywhere we mean to say the left joystick x axis
static const unsigned
    PIN_LEFT_JOYSTICK_X = A0,
    PIN_LEFT_JOYSTICK_Y = A1,
    PIN_RIGHT_JOYSTICK_X = A2,
    PIN_RIGHT_JOYSTICK_Y = A3;

//We have four servo motors to control, and we use the `Servo` library to control each.
//This library provides the Servo **class**, which allows us to easily write and read angles from servos that
//are connected to the Arduino.
Servo azimuth;
Servo base_elevation;
Servo wrist_elevation;
Servo claw;

/// Process a raw joystick value to get a change in angle between -5 and 5.
/// Scales the input reading and applies a deadzone to the joystick.
/// A **deadzone** is a small circle in the center of a joystick where movements will not affect the angle of the arm.
int scale_joystick(int adc_reading) {
    // The value `adc_reading` will be from 0 to 1024, so we must scale this by 100 to put it in the range of 0 to 10.
    // After this, we subtract 5 to shift the entire range from [0, 10] to [-5, 5]
    adc_reading = adc_reading / 100 - 5;
    
    // Apply a deadzone to the reading - if the value is in the range [-1, 1] then we set it to 0 to ignore the reading.
    if(adc_reading >= -1 && adc_reading <= 1) {
        adc_reading = 0;
    }
    
    // Return the scaled and processed joystick value to the user
    return adc_reading;
}

void setup() {
    Serial.begin(9600);
    
    //Let the Arduino know what pins each servo is connected to by calling the attach() method.
    azimuth.attach(PIN_AZIMUTH);
    base_elevation.attach(PIN_BASE_ELEVATION);
    wrist_elevation.attach(PIN_WRIST_ELEVATION);
    claw.attach(PIN_CLAW);
}

void loop() {
    //First, let's read all four joystick values from the pins that they're connected to.
    //This reads the raw voltage delivered to each pin from 0 to 1024, where 0 is zero volts and 1024 is the
    //supply voltage of the arduino (usually 5V).
    // We must process this value to get a small shift to apply to the angle of each servo
    int left_x  = analogRead(PIN_LEFT_JOYSTICK_X);
    int left_y  = analogRead(PIN_LEFT_JOYSTICK_Y);
    int right_x = analogRead(PIN_RIGHT_JOYSTICK_X);
    int right_y = analogRead(PIN_RIGHT_JOYSTICK_Y);
    
    //Now take the raw analog readings, scale them and apply a deadzone, and store the result to four variables
    //that will be used to change the angle of each servo.
    int angle_azimuth = scale_joystick(left_x);
    int angle_elevation_base = scale_joystick(left_y);
    int angle_elevation_wrist = scale_joystick(right_y);
    int angle_claw = scale_joystick(right_x);
    
    //Change the angle of all four servos by adding the small angle change we got from `scale_joystick` to the current angle of the servo.
    //The method servo.read() will return an angle between 0 and 180, and servo.write(angle) will change the servo's angle to what is in the smooth braces.
    azimuth.write(azimuth.read() + angle_azimuth);
    base_elevation.write(base_elevation.read() + angle_elevation_base);
    wrist_elevation.write(wrist_elevation.read() + angle_elevation_wrist);
    claw.write(claw.read() + angle_claw);
    
    //Now we wait 30 milliseconds so that the loop repeats about 30 times per second
    delay(30);
}
