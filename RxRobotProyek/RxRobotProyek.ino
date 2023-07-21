#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//Radio Set
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

// Inisialisasi objek Servo
Servo myservo1;
Servo myservo2;

//Motor set
//Define enable pins of the Motors
const int enbA = 5;
const int enbB = 6;

//Define control pins of the Motors
//If the motors rotate in the opposite direction, you can change the positions of the following pin numbers
const int IN1 = 2;    //Right Motor (-)
const int IN2 = 4;    //Right Motor (+)
const int IN3 = 7;    //Left Motor (+)
const int IN4 = 14;    //Right Motor (-)

// Variables to store the previous servo positions
int prevServoX;
int prevServoY;

int data[3];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(enbA, OUTPUT);
  pinMode(enbB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  myservo1.attach(8);  // attaches the servo on pin 8 to the servo object
  myservo2.attach(3);  // attaches the servo on pin 3 to the servo object

  delay(10);

  myservo1.write(90);
  myservo2.write(90);

  radio.begin();
  if (!radio.begin()) {
    Serial.println(F("Radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (radio.available()) {
    radio.read(data, sizeof(data));

    int mode = data[0];
    //Pengecekan Data mode
    Serial.print("Mode: ");
    Serial.println(mode);
//    delay(100);

    if (mode == 0) {
      carMode();
      Serial.println("Mobil");
    } else {
      servoMode();
      Serial.println("Gripper");
    }
  } else {
    Serial.println("Radio tidak terbaca");
    delay(1000);
  }
}

void servoMode() {
  // Ubah nilai potensio menjadi sudut
  int servoX = map(data[2], -9, 10, 0, 180);
  int servoY = map(data[1], -9, 10, 0, 180);

  // Menggerakkan servo hanya jika ada perubahan sudut
  if (servoX != prevServoX) {
    myservo1.write(servoX);
    prevServoX = servoX;
  }

  if (servoY != prevServoY) {
    myservo2.write(servoY);
    prevServoY = servoY;
  }

//  // Tunggu sebentar sebelum membaca nilai potensio berikutnya
//  delay(200);
}

void carMode() {
  int RightSpd = map(data[1], -9, 10, 0, 254); //Send X axis data
  int LeftSpd = map(data[2], -9, 10, 0, 254);  //Send Y axis data

  if (data[1] > 0) {
    //forward
    maju(RightSpd, LeftSpd);
  } else if (data[1] < 3) {
    //backward
    mundur(RightSpd, LeftSpd);
  } else if (data[2] > 0) {
    //left
    kiri(RightSpd, LeftSpd);
  } else if (data[2] < 3) {
    //right
    kanan(RightSpd, LeftSpd);
  } else {
    //stop car
    analogWrite(enbA, 0);
    analogWrite(enbB, 0);
  }

  delay(100);
}

void maju(int spdA, int spdB) {
  analogWrite(enbA, spdA);
  analogWrite(enbB, spdB);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void mundur(int spdA, int spdB) {
  analogWrite(enbA, spdA);
  analogWrite(enbB, spdB);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void kanan(int spdA, int spdB) {
  analogWrite(enbA, spdA);
  analogWrite(enbB, spdB);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void kiri(int spdA, int spdB) {
  analogWrite(enbA, spdA);
  analogWrite(enbB, spdB);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
