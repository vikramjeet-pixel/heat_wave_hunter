/*
  Smart Car Control System
  Features:
  - Obstacle detection using ultrasonic sensors
  - Temperature monitoring with heat sensors
  - Gas detection for safety
  - Water spray system
*/

// Pin definitions
// Ultrasonic sensor pins
const int trigPin1 = 2;
const int echoPin1 = 3;
const int trigPin2 = 4;
const int echoPin2 = 5;

// Motor control pins
const int leftMotorPin1 = 6;
const int leftMotorPin2 = 7;
const int rightMotorPin1 = 8;
const int rightMotorPin2 = 9;
const int motorEnableA = 10;
const int motorEnableB = 11;

// Sensor pins
const int heatSensorPin = A0; // Analog pin for temperature sensor (e.g., LM35)
const int gasSensorPin = A1;  // Analog pin for gas sensor (e.g., MQ-2)

// Water spray system
const int waterPumpPin = 12;

// Thresholds
const float maxTemperature = 50.0; // in Celsius
const int gasThreshold = 400;      // Adjust based on your sensor
const int safeDistance = 20;       // in centimeters

// Variables
float frontDistance = 0;
float sideDistance = 0;
float temperature = 0;
int gasLevel = 0;

void setup()
{
    // Initialize serial communication
    Serial.begin(9600);

    // Initialize ultrasonic sensor pins
    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);

    // Initialize motor control pins
    pinMode(leftMotorPin1, OUTPUT);
    pinMode(leftMotorPin2, OUTPUT);
    pinMode(rightMotorPin1, OUTPUT);
    pinMode(rightMotorPin2, OUTPUT);
    pinMode(motorEnableA, OUTPUT);
    pinMode(motorEnableB, OUTPUT);

    // Initialize water pump pin
    pinMode(waterPumpPin, OUTPUT);

    // Set initial motor speed
    analogWrite(motorEnableA, 200); // Speed range: 0-255
    analogWrite(motorEnableB, 200);

    Serial.println("Smart Car Control System Initialized");
}

void loop()
{
    // Read sensor data
    readUltrasonicSensors();
    readTemperature();
    readGasLevel();

    // Print sensor readings for debugging
    printSensorReadings();

    // Check for hazards and take appropriate actions
    if (checkHazards())
    {
        stopCar();
    }
    else
    {
        // Normal operation - navigate based on ultrasonic readings
        navigateCar();
    }

    // Small delay to stabilize readings
    delay(100);
}

void readUltrasonicSensors()
{
    // Read front ultrasonic sensor
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    frontDistance = pulseIn(echoPin1, HIGH) * 0.034 / 2;

    // Read side ultrasonic sensor
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    sideDistance = pulseIn(echoPin2, HIGH) * 0.034 / 2;
}

void readTemperature()
{
    // Read temperature from LM35 sensor
    int sensorValue = analogRead(heatSensorPin);
    // Convert analog reading to temperature in Celsius
    // For LM35: 10mV per degree, and Arduino maps 0-5V to 0-1023
    temperature = (sensorValue * 5.0 * 100.0) / 1024.0;
}

void readGasLevel()
{
    // Read gas level from MQ-2 sensor
    gasLevel = analogRead(gasSensorPin);
}

bool checkHazards()
{
    bool hazardDetected = false;

    // Check temperature
    if (temperature > maxTemperature)
    {
        Serial.println("WARNING: High temperature detected!");
        activateWaterSpray();
        hazardDetected = true;
    }

    // Check gas level
    if (gasLevel > gasThreshold)
    {
        Serial.println("WARNING: Harmful gas detected!");
        hazardDetected = true;
    }

    return hazardDetected;
}

void activateWaterSpray()
{
    Serial.println("Activating water spray system");
    digitalWrite(waterPumpPin, HIGH);
    delay(2000); // Spray water for 2 seconds
    digitalWrite(waterPumpPin, LOW);
}

void navigateCar()
{
    // Simple obstacle avoidance logic
    if (frontDistance < safeDistance)
    {
        // Obstacle ahead, check side distance
        if (sideDistance > safeDistance)
        {
            // Turn right if there's space
            turnRight();
        }
        else
        {
            // Back up and turn if both directions have obstacles
            moveBackward();
            delay(500);
            turnRight();
            delay(700);
        }
    }
    else
    {
        // No obstacles, move forward
        moveForward();
    }
}

void moveForward()
{
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
    Serial.println("Moving forward");
}

void moveBackward()
{
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
    Serial.println("Moving backward");
}

void turnRight()
{
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
    Serial.println("Turning right");
}

void turnLeft()
{
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
    Serial.println("Turning left");
}

void stopCar()
{
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
    Serial.println("Car stopped");
}

void printSensorReadings()
{
    Serial.print("Front Distance: ");
    Serial.print(frontDistance);
    Serial.print(" cm | Side Distance: ");
    Serial.print(sideDistance);
    Serial.print(" cm | Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C | Gas Level: ");
    Serial.println(gasLevel);
}