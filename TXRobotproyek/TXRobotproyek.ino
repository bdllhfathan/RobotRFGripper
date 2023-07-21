#include <elapsedMillis.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24l01.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

RF24 radio(8, 9);
Adafruit_MPU6050 mpu;

elapsedMillis readModeMillis;
//
//elapsedMillis writeMpuSensorMillis;
//
//unsigned long writeMpuSensorInterval = 500;
unsigned long readModeInterval = 1000; // Interval waktu baca mode, 1000ms (1 detik)

const byte address[6] = "00001";

const int buttonPin1 = 2; // Nomor pin pushbutton
int buttonState1 = 1; // variabel untuk membaca status pushbutton
bool isRobotMode = true;

// Variabel MPU
int nilaiX, nilaiY;

// Variabel array untuk RF
int data[3];

void setMpu() {
  while (!Serial)
    delay(10); // Akan menunggu sampai serial console terbuka

  Serial.println("Adafruit MPU6050 test!");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
    case MPU6050_RANGE_2_G:
      Serial.println("+-2G");
      break;
    case MPU6050_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case MPU6050_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case MPU6050_RANGE_16_G:
      Serial.println("+-16G");
      break;
  }

  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
    case MPU6050_RANGE_250_DEG:
      Serial.println("+- 250 deg/s");
      break;
    case MPU6050_RANGE_500_DEG:
      Serial.println("+- 500 deg/s");
      break;
    case MPU6050_RANGE_1000_DEG:
      Serial.println("+- 1000 deg/s");
      break;
    case MPU6050_RANGE_2000_DEG:
      Serial.println("+- 2000 deg/s");
      break;
  }

  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
    case MPU6050_BAND_260_HZ:
      Serial.println("260 Hz");
      break;
    case MPU6050_BAND_184_HZ:
      Serial.println("184 Hz");
      break;
    case MPU6050_BAND_94_HZ:
      Serial.println("94 Hz");
      break;
    case MPU6050_BAND_44_HZ:
      Serial.println("44 Hz");
      break;
    case MPU6050_BAND_21_HZ:
      Serial.println("21 Hz");
      break;
    case MPU6050_BAND_10_HZ:
      Serial.println("10 Hz");
      break;
    case MPU6050_BAND_5_HZ:
      Serial.println("5 Hz");
      break;
  }

  Serial.println("");
  delay(100);
}

void setRF() {
  radio.begin();

  if (!radio.begin()) {
    Serial.println(F("Radio hardware is not responding!!"));
    while (1) {} // Berhenti di dalam loop tak terbatas
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(buttonPin1, INPUT_PULLUP);

  setMpu();

  delay(100);

  setRF();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Membaca nilai sensor MPU
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Pembacaan mode
  if (readModeMillis >= readModeInterval) {
    robotMode();
    readModeMillis = 0;
  }

  nilaiX = a.acceleration.x;
  nilaiY = a.acceleration.y;

  if (isRobotMode) {
    Serial.println("Gripper");
    data[0] = 0; // Mode gripper

    data[1] = map(nilaiX, -6, 8, 0, 180);
    data[2] = map(nilaiY, -6, 8, 0, 180);
    Serial.print("data 1: ");
    Serial.println(data[1]);
    Serial.println("data 2: ");
    Serial.println(data[2]);

  } else {
    Serial.println("Mobil");
    data[0] = 1; // Mode kendali mobil
    data[1] = nilaiX;
    data[2] = nilaiY;

    Serial.println(data[1]);
    Serial.println("data 2: ");
    Serial.println(data[2]);
  }

  // Kirim data melalui radio
  radio.write(&data, sizeof(data));
  delay(20);
}

void robotMode() {
  buttonState1 = digitalRead(buttonPin1);

  if (digitalRead(buttonPin1) == LOW) {
    isRobotMode = !isRobotMode;
    delay(1000);
    return;
  }
}
