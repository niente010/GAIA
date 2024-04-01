#include <DHT11.h>
DHT11 dht11(6);

// Define pins for the ultrasonic sensors
const int TriggerPins[] = {8, 10, 12};  // Arduino pins connected to Ultrasonic Sensors' TRIG pins
const int EchoPins[] = {9, 11, 13};  // Arduino pins connected to Ultrasonic Sensors' ECHO pins
const int RelayPins[] = {2, 3, 4};   // Arduino pins connected to Relays' pins
const int DistanceThreshold = 10;    // centimeters
const int NumSensors = 3;            // Number of sensors/relays

// Define the pin for the fan
int relay_4 = 5;

// Variables will change:
float duration, distance;

// Temperature measurement interval (3000 milliseconds = 3 seconds)
unsigned long previousTempMillis = 0;
const long tempInterval = 3000; 

void setup() {  
  Serial.begin(9600);  // initialize serial port
  
  // Initialize pins
  for (int i = 0; i < NumSensors; i++) {
    pinMode(TriggerPins[i], OUTPUT);  // set arduino pin to output mode for TRIG
    pinMode(EchoPins[i], INPUT);   // set arduino pin to input mode for ECHO
    pinMode(RelayPins[i], OUTPUT); // set arduino pin to output mode for RELAY
    digitalWrite(RelayPins[i], HIGH); // Initialize relay to OFF state
  }
  
  pinMode(relay_4, OUTPUT); // Initialize relay_4 as an OUTPUT
  digitalWrite(relay_4, HIGH); // Initialize Fan to OFF state
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if it's time to read temperature
  if (currentMillis - previousTempMillis >= tempInterval) {
    previousTempMillis = currentMillis;
    temperature();
  }

  for (int i = 0; i < NumSensors; i++) {
    // Generate a 10-microsecond pulse to TRIG pin
    digitalWrite(TriggerPins[i], HIGH);
    delayMicroseconds(10);
    digitalWrite(TriggerPins[i], LOW);

    // Measure duration of pulse from ECHO pin
    duration = pulseIn(EchoPins[i], HIGH);
    // Calculate the distance
    distance = 0.017 * duration;

    // Check if the distance is below the threshold
    if (distance < DistanceThreshold) {
      digitalWrite(RelayPins[i], LOW); // Turn on Relay
      // Print the value to Serial Monitor
      Serial.print("Sensor ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(distance);
      Serial.print(" cm - Pump "); 
      Serial.print(i + 1);
      Serial.println(" activated");

      delay(1000); // Keep the relay on for one second

      digitalWrite(RelayPins[i], HIGH); // Turn off Relay
      delay(4000); // Wait for 3 more seconds before taking new measurements
    }
  }
  //delay(500);  // Wait for half a second before starting the next cycle
}

void temperature() {
  static bool isRelay4Active = false;
  int temperature = dht11.readTemperature();

  if (temperature != DHT11::ERROR_CHECKSUM && temperature != DHT11::ERROR_TIMEOUT) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    if (temperature >= 28 && !isRelay4Active) {
      digitalWrite(relay_4, LOW); // Activate the relay (turn the fan ON)
      isRelay4Active = true;
      Serial.println("Relay 4 ON due to high temperature.");
    } 
    else if (temperature <= 21 && isRelay4Active) {
      digitalWrite(relay_4, HIGH); // Deactivate the relay (turn the fan OFF)
      isRelay4Active = false;
      Serial.println("Relay 4 OFF due to temperature drop.");
    }
  } else {
    Serial.println("Failed to read temperature!");
  }
}
