//testing
/*
   Based on the RFID library by Miguel Balboa - https://github.com/miguelbalboa/rfid

   Pin layout used:
   ----------------------------------
               MFRC522      Arduino
               Reader/PCD   Uno
   Signal      Pin          Pin
   ----------------------------------
   RST/Reset   RST          see below
   SPI SS      SDA(SS)      see below
   SPI MOSI    MOSI         11
   SPI MISO    MISO         12
   SPI SCK     SCK          13
*/

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN_1        9
#define SS_PIN_1         10

#define RST_PIN_2        8
#define SS_PIN_2         5

#define RST_PIN_3        4
#define SS_PIN_3         3

MFRC522 mfrc522_1(SS_PIN_1, RST_PIN_1);  // Create MFRC522 instance
MFRC522 mfrc522_2(SS_PIN_2, RST_PIN_2);  // Create MFRC522 instance
MFRC522 mfrc522_3(SS_PIN_3, RST_PIN_3);  // Create MFRC522 instance

MFRC522 readers[] = {mfrc522_1, mfrc522_2, mfrc522_3};

const unsigned long correctId1 = 72057354;
const unsigned long correctId2 = 71926282;
const unsigned long correctId3 = 72188426;

unsigned long correctIds[] = {correctId1, correctId2, correctId3};
unsigned long lastIds[3];
boolean correctlyDetected[] = {false, false, false};


boolean solved = false;

#define Solved_PIN         6
#define MAGNET_PIN         2
#define Failed_PIN         7

int delayTime = 2;
int failcounter = 0;
int maxfailcounter = 5;

void setup() {
  pinMode(Failed_PIN, OUTPUT);
  pinMode(Solved_PIN, OUTPUT);
  pinMode(MAGNET_PIN, OUTPUT);
  digitalWrite(MAGNET_PIN, HIGH);
  Serial.begin(9600);   // Initialize serial communications with the PC
  //while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  for (int i = 0; i < 3; i++) {
    readers[i].PCD_Init();
    readers[i].PCD_DumpVersionToSerial();
  }
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  if (solved)
  {
    return;
  }
  for (int i = 0; i < 3; i++)
  {
    checkReader(readers[i], i);
    delay(100);
  }
}


void checkReader(MFRC522 mfrc522, int id)
{
  unsigned long uid = getID(mfrc522);
  if (uid == -1)
  {
    Serial.print("No Card found on reader "); Serial.println(id);
    return;
  }
  bool anythingCorrect = false;
  bool newCard = false;
  if (uid != lastIds[id])
  {
    Serial.print("New card detected in Reader: "); Serial.print(id); Serial.print(" UID: "); Serial.println(uid);
    lastIds[id] = uid;
    newCard = true;
  }
  else
  {
    Serial.print("Card detected in Reader: "); Serial.print(id); Serial.print(" UID: "); Serial.print(uid); Serial.println(" is known already.");
  }

  if (uid == correctIds[id])
  {
    correctlyDetected[id] = true;
    if(newCard)
    {
      blinkSolvedLed();
    }
    Serial.print("Correct Id detected for reader "); Serial.println(id);
    anythingCorrect = true;
  }
  if (correctlyDetected[0] && correctlyDetected[1] && correctlyDetected[2])
  {
    Serial.println("Riddle solved.");
    solved = true;
    digitalWrite(MAGNET_PIN, LOW); //turn off magnet, open box
    while (true)
    {
      blinkSolvedLed();
    }
  }
  if (!anythingCorrect && newCard)
  {
    resetIds();
    failcounter++;
    Serial.print("Failed: "); Serial.println(failcounter);
    digitalWrite(Failed_PIN, HIGH);
    delay(delayTime * 1000 * min(failcounter, maxfailcounter));
    digitalWrite(Failed_PIN, LOW);
  }
}

void blinkSolvedLed()
{
  digitalWrite(Solved_PIN, HIGH);
  delay(500);
  digitalWrite(Solved_PIN, LOW);
  delay(500);
}

void resetIds()
{
  correctlyDetected[0] = false;
  correctlyDetected[1] = false;
  correctlyDetected[2] = false;
}

unsigned long getID(MFRC522 mfrc522) {
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);
  mfrc522.PICC_WakeupA(bufferATQA, &bufferSize);
  if ( ! mfrc522.PICC_ReadCardSerial()) { // no PICC placed, return
    return -1;
  }
  //Since a PICC placed get Serial and continue
  unsigned long hex_num;
  Serial.print("UID Raw 1:"); Serial.println(mfrc522.uid.uidByte[0]);
  Serial.print("UID Raw 2:"); Serial.println(mfrc522.uid.uidByte[1]);
  Serial.print("UID Raw 3:"); Serial.println(mfrc522.uid.uidByte[2]);
  Serial.print("UID Raw 4:"); Serial.println(mfrc522.uid.uidByte[3]);
  hex_num =  (unsigned long)mfrc522.uid.uidByte[0] << 24;
  hex_num += (unsigned long)mfrc522.uid.uidByte[1] << 16;
  hex_num += (unsigned long)mfrc522.uid.uidByte[2] <<  8;
  hex_num += (unsigned long)mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA(); // Stop reading
  return hex_num;
}
