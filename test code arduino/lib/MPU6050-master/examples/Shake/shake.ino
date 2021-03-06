// shake.ino - a shake detector using MPU6050
// https://github.com/maarten-pennings/MPU6050
#include <Wire.h>
#include <mpu6050.h>

MPU6050 mpu6050;

void setup()  {
  Serial.begin(115200);
  Serial.println("\n\nWelcome to the MPU6050 shake detector");
  Serial.println("Driver version " MPU6050_VERSION);
  Wire.begin();

  int error= mpu6050.begin();
  Serial.print("MPU6050: ");
  Serial.println(mpu6050.error_str(error));

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("When the device is shaken, its LED will blink");
}

void loop() {
  MPU6050_t data= mpu6050.get();
  if( data.accel.error==0 ) {
    // Compute magnitude of accelaration over all three axis
    float magnitude= sqrt(data.accel.x*data.accel.x + data.accel.y*data.accel.y + data.accel.z*data.accel.z); 
    // Turn on LED if acceleration exceeds 2g
    if( magnitude > 2*MPU6050_GRAVITY_EARTH ) {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("Shaken"); 
      delay(250); // Keep LED on for a while
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}