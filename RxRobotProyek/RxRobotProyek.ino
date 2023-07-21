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

int RightSpd = 120;
int LeftSpd = 120;


void(* resetFunc) (void) = 0; // Deklarasi fungsi untuk mereset Arduino


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

    if (mode == 1) {
      carMode();
      Serial.println("Mobil");
      delay(50);
    } else {
      berhenti(0, 0);
      servoMode();
      Serial.println("Gripper");
      delay(50);
    }


  }

  else {
    Serial.println("Radio tidak terbaca");
    myservo1.write(0);
    myservo2.write(0);
    berhenti(0, 0);
    delay(2000);
    if (radio.available()) {
      return;
    }
    resetFunc(); // Memanggil fungsi untuk mereset Arduino
  }
}

void servoMode() {
  // Ubah nilai potensio menjadi sudut

  Serial.print("data 1: ");
  Serial.println(data[1]);
  Serial.println("data 2: ");
  Serial.println(data[2]);
  int servoX = data[2];
  int servoY = data[1];

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
  delay(100);
}

void carMode() {

  Serial.print("data 1: ");
  Serial.println(data[1]);
  Serial.println("data 2: ");
  Serial.println(data[2]);

  if (data[1] > 5) {
    //forward
    maju(RightSpd, RightSpd);
    Serial.println("MAJU");
  } else if (data[1] < -5) {
    //backward
    mundur(RightSpd, RightSpd);
    Serial.println("MUNDUR");
  } else if (data[2] > 3) {
    //left
    kiri(LeftSpd, LeftSpd);
    Serial.println("KIRI");
  } else if (data[2] < -3) {
    //right
    kanan(LeftSpd, LeftSpd);
    Serial.println("KANAN");
  } else {
    //stop car
    analogWrite(enbA, 0);
    analogWrite(enbB, 0);
  }

  delay(50);
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

void berhenti(int spdA, int spdB) {
  analogWrite(enbA, spdA);
  analogWrite(enbB, spdB);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
