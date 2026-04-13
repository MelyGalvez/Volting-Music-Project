// Include required libraries
#include <Wire.h>                      // I2C communication
#include <Adafruit_Sensor.h>          // Base library for Adafruit sensors
#include <Adafruit_BNO055.h>          // Library for the BNO055 motion sensor
#include <utility/imumaths.h>         // Math utilities for BNO055 (quaternions, etc.)

// Initialize the BNO055 sensor
Adafruit_BNO055 bno = Adafruit_BNO055(55);

// Define LED pins
const int BLUE_LED = 10;
const int GREEN_LED = 11;
const int RED_LED = 12;

// Define pins for the two ultrasonic sensors
const int trigPin1 = 6;
const int echoPin1 = 7;
const int trigPin2 = 9;
const int echoPin2 = 8;

void setup(void) {
  Serial.begin(115200);  // Initialize serial communication at 115200 baud

  // Configure LEDs as outputs
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH); // Blue LED ON during initialization

  // Configure ultrasonic sensors (trig as output, echo as input)
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  // Initialize BNO055 sensor
  if (!bno.begin()) {
    Serial.println("Error: BNO055 sensor not detected.");
    digitalWrite(RED_LED, HIGH);  // Red LED ON if failure
    digitalWrite(BLUE_LED, LOW);
    while (1); // Infinite loop on error
  }

  delay(10);
  bno.setExtCrystalUse(true);   // Use external crystal for better accuracy
  digitalWrite(GREEN_LED, HIGH); // Green LED ON when everything is OK
  digitalWrite(BLUE_LED, LOW);
}

void loop(void) {
  // Retrieve different measurements from the BNO055 sensor
  sensors_event_t orientationData, linearAccelData, accelData, gyroData, magData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);        // Orientation
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL); // Linear acceleration
  bno.getEvent(&accelData, Adafruit_BNO055::VECTOR_ACCELEROMETER);     // Raw acceleration
  bno.getEvent(&gyroData, Adafruit_BNO055::VECTOR_GYROSCOPE);          // Angular velocity
  bno.getEvent(&magData, Adafruit_BNO055::VECTOR_MAGNETOMETER);        // Magnetic field

  imu::Quaternion quat = bno.getQuat(); // Get quaternion (full orientation)
  int8_t temp = bno.getTemp();          // Sensor internal temperature

  // Measure distances using ultrasonic sensors
  long distance1 = measureDistance(trigPin1, echoPin1);
  long distance2 = measureDistance(trigPin2, echoPin2);

  // Send data in readable format (each line starts with an identifier)
  Serial.print("O: "); // Orientation
  Serial.print(orientationData.orientation.x); Serial.print(", ");
  Serial.print(orientationData.orientation.y); Serial.print(", ");
  Serial.println(orientationData.orientation.z);

  Serial.print("L: "); // Linear acceleration
  Serial.print(linearAccelData.acceleration.x); Serial.print(", ");
  Serial.print(linearAccelData.acceleration.y); Serial.print(", ");
  Serial.println(linearAccelData.acceleration.z);

  Serial.print("A: "); // Raw acceleration
  Serial.print(accelData.acceleration.x); Serial.print(", ");
  Serial.print(accelData.acceleration.y); Serial.print(", ");
  Serial.println(accelData.acceleration.z);

  Serial.print("G: "); // Gyroscope
  Serial.print(gyroData.gyro.x); Serial.print(", ");
  Serial.print(gyroData.gyro.y); Serial.print(", ");
  Serial.println(gyroData.gyro.z);

  Serial.print("M: "); // Magnetometer
  Serial.print(magData.magnetic.x); Serial.print(", ");
  Serial.print(magData.magnetic.y); Serial.print(", ");
  Serial.println(magData.magnetic.z);

  Serial.print("Q: "); // Quaternion
  Serial.print(quat.w(), 4); Serial.print(", ");
  Serial.print(quat.x(), 4); Serial.print(", ");
  Serial.print(quat.y(), 4); Serial.print(", ");
  Serial.println(quat.z(), 4);

  Serial.print("T: "); // Temperature
  Serial.println(temp);

  Serial.print("D1: "); // Distance sensor 1
  Serial.println(distance1);

  Serial.print("D2: "); // Distance sensor 2
  Serial.println(distance2);

  Serial.println("---"); // Separator for easier parsing

  delay(100); // Delay to avoid overloading the serial connection
}

// Function to measure distance using an ultrasonic sensor
long measureDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);          // Send a 10 µs pulse
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH, 30000); // Measure return time, 30 ms timeout
  long distance = (duration / 2) / 29.1;      // Convert to cm
  return constrain(distance, 0, 200);         // Limit distance between 0 and 200 cm
}