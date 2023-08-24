#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

//Radio Set
RF24 radio(10, 9); // CE, CSN;
const byte address[6] = "00001 "; //Pastikan Address dibedakan dengan robot lain

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

int posX;
int posY;

int data[3];

int RightSpd;
int LeftSpd;


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
    //    delay(100);

    if (mode == 1) {
      Serial.print("Mobil \t");
      //Menjalankan Fungsi Penggerak mobil
      carMode();
      
      //Menetapkan posisi gripper terakhir
      myservo1.write(posX);
      myservo2.write(posY);
      delay(50);
      
    } else {
      berhenti(0, 0);
      Serial.print("Gripper \t");
      servoMode();
      delay(50);
      
    }

    
    Serial.print("data 1: ");
    Serial.print(data[1]);
    Serial.print("\t");
    Serial.print("data 2: ");
    Serial.println(data[2]);


  } else {
    Serial.println("Radio tidak terbaca");
    myservo1.write(posX);
    myservo2.write(posY);
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

  int servoX = data[2];
  int servoY = data[1];

  // Menggerakkan servo hanya jika ada perubahan sudut
  //  if (servoX != prevServoX) {
  if (servoX >= 4) {
    posX = posX + 10;
//    Serial.print("posX = ");
//    Serial.println(posX);
    delay(20);

  } else if (servoX <= -4) {
    posX = posX - 10;
//    Serial.print("posX = ");
//    Serial.println(posX);
    delay(20);
  }

  if (servoY > 4) {
    posY = posY + 4;
    delay(20);

  } else if (servoY < -4) {
    posY = posY - 10;
    delay(20);
  }

  if (posX > 180 ) {
    posX = 180;
  } else if (posX < 0) {
    posX = 0;
  } else if (posY > 180) {
    posY = 180;
  } else if (posY < 0) {
    posY = 0;
  }

  myservo1.write(posX);
  myservo2.write(posY);

  // Tunggu sebentar sebelum membaca nilai potensio berikutnya
  delay(50);
}

void carMode() {

  Serial.print("Kondisi: ");
  if (data[1] > 3) {
    //forward
    maju();
    Serial.print("MAJU \t");
  } else if (data[1] < -3) {
    //backward
    mundur();
    Serial.print("MUNDUR \t");
  } else if (data[2] > 3) {
    //left
    kiri();
    Serial.print("KIRI \t");
  } else if (data[2] < -3) {
    //right
    kanan();
    Serial.print("KANAN \t");
  } else {
    //stop car
    
    Serial.print("Netral \t");
    RightSpd = 0;
    LeftSpd = 0;
  }

  analogWrite(enbA, RightSpd); // Send PWM signal to motor A
  analogWrite(enbB, LeftSpd); // Send PWM signal to motor B

  delay(50);
}

void maju() {
  
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  RightSpd = map(data[1], 3, 10, 90, 255);
  LeftSpd = map(data[1], 3, 10, 90, 255);

}

void mundur() {
  //  analogWrite(enbA, spdA);
  //  analogWrite(enbB, spdB);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  RightSpd = map(data[1], -4, -9, 0, 255);
  LeftSpd = map(data[1], -4, -9, 0, 255);
}

void kanan() {

  int RLMap = map(data[2], -9, -3, 0, 255);
  RightSpd = RightSpd - RLMap;
  LeftSpd = LeftSpd + RLMap;
  //Membatasi Ukuran Nilai tetap di 0-255
  if (RightSpd < 0) {
    RightSpd = 0;
  }
  if (LeftSpd > 255) {
    LeftSpd = 255;
  }
}

void kiri() {

  int RLMap = map(data[2], 3, 9, 100, 255);
  RightSpd = RightSpd + RLMap;
  LeftSpd = LeftSpd - RLMap;
  //Membatasi Ukuran Nilai tetap di 0-255
  if (LeftSpd < 0) {
    LeftSpd = 0;
  }
  if (RightSpd > 255) {
    RightSpd = 255;
  }
}

void berhenti(int spdA, int spdB) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
