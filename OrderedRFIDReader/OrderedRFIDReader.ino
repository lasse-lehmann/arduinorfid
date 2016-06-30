
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

#define RST_PIN_1         9          
#define SS_PIN_1          10         

#define RST_PIN_2        8          
#define SS_PIN_2         5         

#define RST_PIN_3        4         
#define SS_PIN_3         3        

MFRC522 mfrc522_1(SS_PIN_1, RST_PIN_1);  // Create MFRC522 instance
MFRC522 mfrc522_2(SS_PIN_2, RST_PIN_2);  // Create MFRC522 instance
MFRC522 mfrc522_3(SS_PIN_3, RST_PIN_3);  // Create MFRC522 instance

const unsigned long correctId1 = 72057354;
const unsigned long correctId2 = 71926282;
const unsigned long correctId3 = 72188426;

boolean id1Detected = false;
boolean id2Detected = false;
boolean id3Detected = false;

boolean solved = false;

#define Solved_PIN         6
#define Failed_PIN         7

int delayTime = 5;
int failcounter = 0;
int maxfailcounter = 10;

void setup() {
  pinMode(Failed_PIN, OUTPUT);
  pinMode(Solved_PIN, OUTPUT);
  Serial.begin(9600);   // Initialize serial communications with the PC
  //while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522_1.PCD_Init();   // Init MFRC522
  mfrc522_1.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  mfrc522_2.PCD_Init();   // Init MFRC522
  mfrc522_2.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  mfrc522_3.PCD_Init();   // Init MFRC522
  mfrc522_3.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void loop() {
  if (solved)
  {
    return;
  }
  checkReader(mfrc522_1, 1);
  checkReader(mfrc522_2, 2);
  checkReader(mfrc522_3, 3);
  delay(20);
}

void checkReader(MFRC522 mfrc522, int id) {
  if (mfrc522.PICC_IsNewCardPresent()) {
    unsigned long uid = getID(mfrc522);
    if (uid != -1) {
      Serial.print("Card detected in Reader: "); Serial.print(id); Serial.print(" UID: "); Serial.println(uid);
      if (uid == correctId1 || uid == correctId2 || uid == correctId3)
      {
        Serial.println("Correct Id detected");
        if (uid == correctId1 && id == 1)
        {
          id1Detected = true;
        }
        if (uid == correctId2 && id == 2)
        {
          id2Detected = true;
        }
        if (uid == correctId3 && id == 3)
        {
          id3Detected = true;
        }
        if (id1Detected && id2Detected && id3Detected)
        {
          Serial.println("Riddle solved.");
          solved = true;
          while (true)
          {
            digitalWrite(Solved_PIN, HIGH);
            delay(500);
            digitalWrite(Solved_PIN, LOW);
            delay(500);
          }
        }
      }
      else
      {
        resetIds();
        failcounter++;
        Serial.print("Failed: "); Serial.println(failcounter);
        digitalWrite(Failed_PIN, HIGH);
        delay(delayTime * 1000 * min(failcounter, maxfailcounter));
        digitalWrite(Failed_PIN, LOW);
      }
    }
  }
}

void resetIds()
{
  id1Detected = false;
  id2Detected = false;
  id3Detected = false;
}

unsigned long getID(MFRC522 mfrc522) {
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
