#include <DHT11.h>         
#include <Servo.h>         // Servo library to control servo motor


#define DHTPIN D3               // DHT11 data pin connected to D3
#define MOISTURE_PIN A0         // Analog pin for moisture sensor
#define IR_SENSOR_PIN D0        // Digital pin for IR sensor

#define SERVO_PIN D1            // Servo motor connected to D1 pin
#define RELAY_PIN D7            // Relay module connected to D7
#define BUZZER_PIN D2           // Buzzer for alerts
#define INTRUDER_LED_PIN D4     // LED to turn ON when motion detected

// DHT11 setup
DHT11 dht(DHTPIN);
Servo myServo;

int moistureThreshold = 30;       // Moisture below this means dry
int criticalMoistureLevel = 30;   // Moisture below this is VERY dry
int tempWarningLevel = 35;         // Temperature above this is too hot

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(INTRUDER_LED_PIN, OUTPUT);
  pinMode(MOISTURE_PIN, INPUT);

  delay(1000);
  Serial.println("NodeMCU ESP8266 - Plant Guardian System");
}

void loop() {
  int temperature = dht.readTemperature();
  int humidity = dht.readHumidity();
  int soilMoisture = analogRead(MOISTURE_PIN);
  int motionDetected = digitalRead(IR_SENSOR_PIN);  // LOW = motion

  // --- TEMP & HUMIDITY ---
  Serial.print(" Temperature (C): ");
  Serial.println(temperature);
  Serial.print(" Humidity (%): ");
  Serial.println(humidity);

  if (temperature > tempWarningLevel) {
    Serial.println(" WARNING: High temperature! ");
  }

  if (humidity < 30) {
    Serial.println(" WARNING: Low humidity! ");
  }

  // --- MOISTURE CHECK ---
  Serial.print(" Soil Moisture Level: ");
  Serial.println(soilMoisture);

  if (soilMoisture < criticalMoistureLevel) {
    Serial.println(" ALERT: Soil is critically dry!");
    tone(BUZZER_PIN, 2000);
    delay(500);
    noTone(BUZZER_PIN);
  }

  if (soilMoisture < moistureThreshold) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println(" Soil is dry! Starting watering...");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println(" Soil is moist. No watering needed.");
  }

  // --- MOTION DETECTION ---
  if (motionDetected == LOW) {
    myServo.write(90);
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(INTRUDER_LED_PIN, HIGH);
    Serial.println(" Motion detected! Intruder alert.");
    delay(1000);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    myServo.write(0);
    digitalWrite(INTRUDER_LED_PIN, LOW);
    Serial.println(" No motion detected.");
  }

  delay(200);
}
