// Basic OLED demo for accelerometer readings from Adafruit MPU6050

// ESP32 Guide: https://RandomNerdTutorials.com/esp32-mpu-6050-accelerometer-gyroscope-arduino/
// ESP8266 Guide: https://RandomNerdTutorials.com/esp8266-nodemcu-mpu-6050-accelerometer-gyroscope-arduino/
// Arduino Guide: https://RandomNerdTutorials.com/arduino-mpu-6050-accelerometer-gyroscope/

#include <Adafruit_MPU6050.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

const float threshold = 2.5; // Adjust this threshold for step detection sensitivity
const int bufferLength = 15; // Number of accelerometer readings in the buffer
float buffer[bufferLength];
int bufferIndex = 0;
int stepCount = 0;
bool stepDetected = false;

const unsigned long debounceDelay = 300; // Debounce delay in milliseconds
unsigned long lastStepTime = 0;



void setup() {
  Serial.begin(115200);
  // while (!Serial);
  Serial.println("MPU6050 OLED demo");

  if (!mpu.begin()) {
    Serial.println("Sensor init failed");
    while (1)
      yield();
  }
  Serial.println("Found a MPU-6050 sensor");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.display();
}
void updateOledDisplay() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("# de pasos: ");
  display.print(stepCount);
  display.display();
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  display.clearDisplay();
  display.setCursor(0, 0);
/*
  display.println("Accelerometer - m/s^2");
  display.print(a.acceleration.x - 1, 1);
  display.print(", ");
  display.print(a.acceleration.y + 0.5, 1);
  display.print(", ");
  display.print(a.acceleration.z - 9.2, 1);
  display.println("");*/
  float accelerationMagnitude = sqrt((a.acceleration.x - 1) * (a.acceleration.x - 1) +
                                     (a.acceleration.y + 0.5) * (a.acceleration.y + 0.5) +
                                     (a.acceleration.z - 9.2) * (a.acceleration.z - 9.2));

  buffer[bufferIndex] = accelerationMagnitude;
  bufferIndex = (bufferIndex + 1) % bufferLength;

  // Detect a step if the current magnitude is greater than the average of the buffer by the threshold
  float avgMagnitude = 0;
  for (int i = 0; i < bufferLength; i++) {
    avgMagnitude += buffer[i];
  }
  avgMagnitude /= bufferLength;

  unsigned long currentMillis = millis();

  if (accelerationMagnitude > (avgMagnitude + threshold)) {
    if (!stepDetected && (currentMillis - lastStepTime) > debounceDelay) {
      stepCount++;
      stepDetected = true;
      lastStepTime = currentMillis;
      updateOledDisplay();
      
    }
  } else {
    stepDetected = false;
  }

  delay(100);
}