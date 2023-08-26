/*
 * Evenness of Light Experiment
 * 
 * SD card circuit:
 *     SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - pin 10
 *  
 *  created November 20, 2020
 *  by Paula Cameron
 */

#include <SPI.h>
#include <SD.h>

// variables
File myFile;
float seconds = 0.0;

bool D = LOW;
bool C = LOW;
bool B = LOW;
bool A = LOW;

int muxVals[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int muxVals2[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const int chipSelect = 10;
const int S3 = 4; /* multiplexer to arduino connection (integer represents pins) connect through breadboard */
const int S2 = 3; // one wire from each multiplexer most connect to the same arduino pin (reason for connection through breadboard)
const int S1 = 2;
const int S0 = 1;

void setup() { /* Defining of input and output pins */

  Serial.println("Setup started...");
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  //pinMode(A2, INPUT);
  //pinMode(A3, INPUT);
  //pinMode(A4, INPUT);
  //pinMode(A5, INPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT); 
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  
  Serial.begin(9600);

  Serial.println("Initializing SD card...");

  digitalWrite(6, LOW); // green light
  digitalWrite(7, LOW); // red light

// Connect wires to arduino pins based on positions above

  if(!SD.begin(chipSelect)) {
    digitalWrite(7, HIGH);
    Serial.println("Initialization failed!");
    while(1);
  } 

  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);

  Serial.println("Initialization done.");

  seconds = 0.0;
}

void loop() {
  delay(30000);
  seconds = seconds + 30;
  myFile = SD.open("even.txt", FILE_WRITE);

  if(myFile) {
    Serial.println("Writing to file...");
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
  } else{
    Serial.println("Error opening file.");
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    while(1);
  }
  
  for(int i = 0; i < 16; i++) {
    D = bitRead(i, 3);/* bitRead converts values to binary */ /* */
    C = bitRead(i, 2);
    B = bitRead(i, 1);
    A = bitRead(i, 0);

    digitalWrite(S3, D);
    digitalWrite(S2, C);
    digitalWrite(S1, B);
    digitalWrite(S0, A);
    delay(100);
    seconds = seconds + 0.1;

    muxVals[i] = analogRead(A0); // Connection of multiplexer to arduino
    muxVals[i] = analogRead(A0);
    muxVals2[i] = analogRead(A1); 
    muxVals2[i] = analogRead(A1);
    //Serial.print(D); Serial.print(C); Serial.print(B); Serial.print(A); Serial.print(": ");
    //Serial.print(analogRead(A0)); Serial.print("\tab\tab"); Serial.print(analogRead(A1)); Serial.print(", ");
  }

  Serial.println();
  Serial.println();
  myFile.print(seconds); myFile.print(",");
  
  // int photoArray[24] = {muxVals[15], muxVals[14], muxVals[13], muxVals[12], muxVals[11], muxVals[10], muxVals[9], muxVals[8], muxVals[7], muxVals[6], muxVals[5], muxVals[4], muxVals[3], muxVals[2], muxVals[1], muxVals[0], analogRead(A2), analogRead(A3), analogRead(A4), analogRead(A5), muxVals2[0], muxVals2[1], muxVals2[2], muxVals2[3]};
  int photoArray[24] = {muxVals[11], muxVals[10], muxVals[9], muxVals[8], muxVals[7], muxVals[6], muxVals[5], muxVals[4], muxVals[3], muxVals[2], muxVals[1], muxVals[0], muxVals2[11], muxVals2[10], muxVals2[9], muxVals2[8], muxVals2[7], muxVals2[6], muxVals2[5], muxVals2[4], muxVals2[3], muxVals2[2], muxVals2[1], muxVals2[0]};
  // 16 inputs through multiplexer 1, 4 directly into arduino, 4 through multiplexer
  for(int i = 0; i < 24; i++) {
    Serial.print(photoArray[i]);
    myFile.print(photoArray[i]);
  
    Serial.print("\t");
    myFile.print(",");
    
    if((i+1) % 6 == 0) {
      Serial.println();
    }
  }

  myFile.println();
  myFile.close();
}
