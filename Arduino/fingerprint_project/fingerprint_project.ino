#include <Adafruit_Fingerprint.h>

#include<EEPROM.h> 

//command for storing data

#include<LiquidCrystal.h> 

//lcd header file

LiquidCrystal lcd(8,9,10,11,12,13);

#include <SoftwareSerial.h>

SoftwareSerial fingerPrint(2, 3); 

//for tx/rx communication between arduino & r305 fingerprint sensor
 
#include <Wire.h>

#include "RTClib.h" //library file for DS3231 RTC Module

RTC_DS3231 rtc;
 
uint8_t id;

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerPrint);
 
#define register_back 14
#define delete_ok 15
#define forward 16
#define reverse 17
#define match 5
#define indFinger 7
#define buzzer 5
 
#define records 10 

// 10 for 10 user
 
int user1,user2,user3,user4,user5,user6,user7,user8,user9,user10;
 
DateTime now;
 
void setup()
{
delay(1000);
lcd.begin(16,2);
Serial.begin(9600);
pinMode(register_back, INPUT_PULLUP);
pinMode(forward, INPUT_PULLUP);
pinMode(reverse, INPUT_PULLUP);
pinMode(delete_ok, INPUT_PULLUP);
pinMode(match, INPUT_PULLUP);
pinMode(buzzer, OUTPUT);
pinMode(indFinger, OUTPUT);
digitalWrite(buzzer, LOW);
if(digitalRead(register_back) == 0)
{
digitalWrite(buzzer, HIGH);
delay(500);
digitalWrite(buzzer, LOW);
lcd.clear();
lcd.print(F("Please wait !"));
lcd.setCursor(0,1);
lcd.print(F("Downloding Data"));
 
Serial.println(F("Please wait"));
Serial.println(F("Downloding Data.."));
Serial.println();
 
Serial.print(F("S.No. "));
for(int i=0;i<records;i++)
{
digitalWrite(buzzer, HIGH);
delay(500);
digitalWrite(buzzer, LOW);
Serial.print(F(" User ID"));
Serial.print(i+1);
Serial.print(F(" "));
}
Serial.println();
int eepIndex=0;
for(int i=0;i<30;i++)
{
if(i+1<10)
Serial.print('0');
Serial.print(i+1);
Serial.print(F(" "));
eepIndex=(i*7);
download(eepIndex);
eepIndex=(i*7)+210;
download(eepIndex);
eepIndex=(i*7)+420;
download(eepIndex);
eepIndex=(i*7)+630;
download(eepIndex);
eepIndex=(i*7)+840;
download(eepIndex);
eepIndex=(i*7)+1050;
download(eepIndex);
eepIndex=(i*7)+1260;
download(eepIndex);
eepIndex=(i*7)+1470;
download(eepIndex);
eepIndex=(i*7)+1680;
download(eepIndex);
Serial.println();
}
}
if(digitalRead(delete_ok) == 0)
{
lcd.clear();
lcd.print(F("Please Wait"));
lcd.setCursor(0,1);
lcd.print(F("Reseting....."));
for(int i=1000;i<1005;i++)
EEPROM.write(i,0);
for(int i=0;i<841;i++)
EEPROM.write(i, 0xff);
lcd.clear();
lcd.print(F("System Reset"));
delay(1000);
}
 
lcd.clear();
lcd.print(F(" Fingerprint "));
lcd.setCursor(0,1);
lcd.print(F("Attendance System"));
delay(2000);
lcd.clear();
 
digitalWrite(buzzer, HIGH);
delay(500);
digitalWrite(buzzer, LOW);
for(int i=1000;i<1000+records;i++)
{
if(EEPROM.read(i) == 0xff)
EEPROM.write(i,0);
}
 
finger.begin(57600);
Serial.begin(9600);
lcd.clear();
lcd.print(F("Finding Module.."));
lcd.setCursor(0,1);
delay(2000);
if (finger.verifyPassword())
{
Serial.println(F("Found fingerprint sensor!"));
lcd.clear();
lcd.print(F(" Module Found"));
delay(2000);
}
else
{
Serial.println(F("Did not find fingerprint sensor :("));
lcd.clear();
lcd.print(F("Module Not Found"));
lcd.setCursor(0,1);
lcd.print(F("Check Connections"));
while (1);
}
 
if (! rtc.begin())
Serial.println(F("Couldn't find RTC"));
 
// rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 
if (rtc.lostPower())
{
Serial.println(F("RTC is NOT RUNNING!"));

// following line sets the RTC to the date & time this sketch was compiled

rtc.adjust(DateTime(2020, 10, 9, 16, 35, 0));

// This line sets the RTC with an explicit date & time, for example to set

// Oct 9, 2020 at 4.35pm you would call:

// rtc.adjust(DateTime(2020, 10, 9, 16, 353, 0));
}
lcd.setCursor(0,0);
lcd.print(F(" Press Match to "));
lcd.setCursor(0,1);
lcd.print(F(" Start System"));
delay(3000);
 
user1=EEPROM.read(1000);
user2=EEPROM.read(1001);
user3=EEPROM.read(1002);
user4=EEPROM.read(1003);
user5=EEPROM.read(1004);
lcd.clear();
digitalWrite(indFinger, HIGH);
 
}
 
void loop()
{
now = rtc.now();
lcd.setCursor(0,0);
lcd.print(F("Time: "));
lcd.print(now.hour(), DEC);
lcd.print(':');
lcd.print(now.minute(), DEC);
lcd.print(':');
lcd.print(now.second(), DEC);
lcd.print(F(" "));
lcd.setCursor(0,1);
lcd.print(F("Date: "));
lcd.print(now.day(), DEC);
lcd.print('/');
lcd.print(now.month(), DEC);
lcd.print('/');
lcd.print(now.year(), DEC);
lcd.print(F(" "));
delay(500);
int result=getFingerprintIDez();
if(result>0)
{
digitalWrite(indFinger, LOW);
digitalWrite(buzzer, HIGH);
delay(100);
digitalWrite(buzzer, LOW);
lcd.clear();
lcd.print(F("ID:"));
lcd.print(result);
lcd.setCursor(0,1);
lcd.print(F("Please Wait...."));
delay(1000);
attendance(result);
lcd.clear();
lcd.print(F("Attendance "));
lcd.setCursor(0,1);
lcd.print(F("Registered"));
delay(1000);
digitalWrite(indFinger, HIGH);
return;
}
checkKeys();
delay(300);
}
 
// dmyyhms - 7 bytes

void attendance(int id)
{
int user=0,eepLoc=0;
if(id == 1)
{
eepLoc=0;
user=user1++;
}
else if(id == 2)
{
eepLoc=210;
user=user2++;
}
else if(id == 3)
{
eepLoc=420;
user=user3++;
}
else if(id == 4)
{
eepLoc=630;
user=user4++;
}
else if(id == 5)
{
eepLoc=0;
user=user5++;
}
else if(id == 6)
{
eepLoc=840;
user=user5++;
}
else if(id == 7)
{
eepLoc=1050;
user=user7++;
}
else if(id == 8)
{
eepLoc=1260;
user=user8++;
}
else if(id == 9)
{
eepLoc=1470;
user=user9++;
}
else if(id == 10)
{
eepLoc=1680;
user=user8++;
}

else
return;
 
int eepIndex=(user*7)+eepLoc;
EEPROM.write(eepIndex++, now.hour());
EEPROM.write(eepIndex++, now.minute());
EEPROM.write(eepIndex++, now.second());
EEPROM.write(eepIndex++, now.day());
EEPROM.write(eepIndex++, now.month());
EEPROM.write(eepIndex++, now.year()>>8 );
EEPROM.write(eepIndex++, now.year());
 
EEPROM.write(1000,user1);
EEPROM.write(1001,user2);
EEPROM.write(1002,user3);
EEPROM.write(1003,user4);

// EEPROM.write(4,user5); // figth user
}
 
void checkKeys()
{
if(digitalRead(register_back) == 0)
{
lcd.clear();
lcd.print(F("Please Wait"));
delay(1000);
while(digitalRead(register_back) == 0);
Enroll();
}
 
else if(digitalRead(delete_ok) == 0)
{
lcd.clear();
lcd.print(F("Please Wait"));
delay(1000);
delet();
}
}
 
void Enroll()
{
int count=1;
lcd.clear();
lcd.print(F("Enter Finger ID:"));
 
while(1)
{
lcd.setCursor(0,1);
lcd.print(count);
if(digitalRead(forward) == 0)
{
count++;
if(count>records)
count=1;
delay(500);
}
 
else if(digitalRead(reverse) == 0)
{
count--;
if(count<1)
count=records;
delay(500);
}
else if(digitalRead(delete_ok) == 0)
{
id=count;
getFingerprintEnroll();
for(int i=0;i<records;i++)
{
if(EEPROM.read(i) != 0xff)
{
EEPROM.write(i, id);
break;
}
}
return;
}
 
else if(digitalRead(register_back) == 0)
{
return;
}
}
}
 
void delet()
{
int count=1;
lcd.clear();
lcd.print(F("Enter Finger ID"));
 
while(1)
{
lcd.setCursor(0,1);
lcd.print(count);
if(digitalRead(forward) == 0)
{
count++;
if(count>records)
count=1;
delay(500);
}
 
else if(digitalRead(reverse) == 0)
{
count--;
if(count<1)
count=records;
delay(500);
}
else if(digitalRead(delete_ok) == 0)
{
id=count;
deleteFingerprint(id);
for(int i=0;i<records;i++)
{
if(EEPROM.read(i) == id)
{
EEPROM.write(i, 0xff);
break;
}
}
return;
}
 
else if(digitalRead(register_back) == 0)
{
return;
}
}
}
 
uint8_t getFingerprintEnroll()
{
int p = -1;
lcd.clear();
lcd.print(F("finger ID:"));
lcd.print(id);
lcd.setCursor(0,1);
lcd.print(F("Place Finger"));
delay(2000);
while (p != FINGERPRINT_OK)
{
p = finger.getImage();
switch (p)
{
case FINGERPRINT_OK:
Serial.println(F("Image taken"));
lcd.clear();
lcd.print(F("Image taken"));
break;
case FINGERPRINT_NOFINGER:
Serial.println(F("No Finger"));
lcd.clear();
lcd.print(F("No Finger Found"));
break;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println(F("Communication error"));
lcd.clear();
lcd.print(F("Comm Error"));
break;
case FINGERPRINT_IMAGEFAIL:
Serial.println(F("Imaging error"));
lcd.clear();
lcd.print(F("Imaging Error"));
break;
default:
Serial.println(F("Unknown error"));
lcd.clear();
lcd.print(F("Unknown Error"));
break;
}
}
 
// OK success!
 
p = finger.image2Tz(1);
switch (p) {
case FINGERPRINT_OK:
Serial.println(F("Image converted"));
lcd.clear();
lcd.print(F("Image converted"));
break;
case FINGERPRINT_IMAGEMESS:
Serial.println(F("Image too messy"));
lcd.clear();
lcd.print(F("Image too messy"));
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println(F("Communication error"));
lcd.clear();
lcd.print(F("Comm Error"));
return p;
case FINGERPRINT_FEATUREFAIL:
Serial.println(F("Could not find fingerprint features"));
lcd.clear();
lcd.print(F("Feature Not Found"));
return p;
case FINGERPRINT_INVALIDIMAGE:
Serial.println(F("Could not find fingerprint features"));
lcd.clear();
lcd.print(F("Feature Not Found"));
return p;
default:
Serial.println(F("Unknown error"));
lcd.clear();
lcd.print(F("Unknown Error"));
return p;
}
 
Serial.println(F("Remove finger"));
lcd.clear();
lcd.print(F("Remove Finger"));
delay(2000);
p = 0;
while (p != FINGERPRINT_NOFINGER) {
p = finger.getImage();
}
Serial.print(F("ID ")); Serial.println(id);
p = -1;
Serial.println(F("Place same finger again"));
lcd.clear();
lcd.print(F("Place Finger"));
lcd.setCursor(0,1);
lcd.print(F(" Again"));
while (p != FINGERPRINT_OK) {
p = finger.getImage();
switch (p) {
case FINGERPRINT_OK:
Serial.println(F("Image taken"));
break;
case FINGERPRINT_NOFINGER:
Serial.print(F("."));
break;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println(F("Communication error"));
break;
case FINGERPRINT_IMAGEFAIL:
Serial.println(F("Imaging error"));
break;
default:
Serial.println(F("Unknown error"));
return;
}
}
 
// OK success!
 
p = finger.image2Tz(2);
switch (p) {
case FINGERPRINT_OK:
Serial.println(F("Image converted"));
break;
case FINGERPRINT_IMAGEMESS:
Serial.println(F("Image too messy"));
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println(F("Communication error"));
return p;
case FINGERPRINT_FEATUREFAIL:
Serial.println(F("Could not find fingerprint features"));
return p;
case FINGERPRINT_INVALIDIMAGE:
Serial.println(F("Could not find fingerprint features"));
return p;
default:
Serial.println(F("Unknown error"));
return p;
}
 
// OK converted!
Serial.print(F("Creating model for #")); Serial.println(id);
 
p = finger.createModel();
if (p == FINGERPRINT_OK) {
Serial.println(F("Prints matched!"));
} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
Serial.println(F("Communication error"));
return p;
} else if (p == FINGERPRINT_ENROLLMISMATCH) {
Serial.println(F("Fingerprints did not match"));
return p;
} else {
Serial.println(F("Unknown error"));
return p;
}
 
Serial.print(F("ID ")); Serial.println(id);
p = finger.storeModel(id);
if (p == FINGERPRINT_OK) {
Serial.println(F("Stored!"));
lcd.clear();
lcd.print(F(" Finger Stored!"));
delay(2000);
} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
Serial.println(F("Communication error"));
return p;
} else if (p == FINGERPRINT_BADLOCATION) {
Serial.println(F("Could not store in that location"));
return p;
} else if (p == FINGERPRINT_FLASHERR) {
Serial.println(F("Error writing to flash"));
return p;
}
else {
Serial.println(F("Unknown error"));
return p;
}
}
 
int getFingerprintIDez()
{
uint8_t p = finger.getImage();
 
if (p != FINGERPRINT_OK)
return -1;
 
p = finger.image2Tz();
if (p != FINGERPRINT_OK)
return -1;
 
p = finger.fingerFastSearch();
if (p != FINGERPRINT_OK)
{
lcd.clear();
lcd.print(F("Finger Not Found"));
lcd.setCursor(0,1);
lcd.print(F("Try Later"));
delay(2000);
return -1;
}
// found a match!

Serial.print(F("Found ID #"));
Serial.print(finger.fingerID);
return finger.fingerID;
}
 
uint8_t deleteFingerprint(uint8_t id)
{
uint8_t p = -1;
lcd.clear();
lcd.print(F("Please wait"));
p = finger.deleteModel(id);
if (p == FINGERPRINT_OK)
{
Serial.println(F("Deleted!"));
lcd.clear();
lcd.print(F("Finger Deleted"));
lcd.setCursor(0,1);
lcd.print(F("Successfully"));
delay(1000);
}
 
else
{
Serial.print(F("Something Wrong"));
lcd.clear();
lcd.print(F("Something Wrong"));
lcd.setCursor(0,1);
lcd.print(F("Try Again Later"));
delay(2000);
return p;
}
}
 
void download(int eepIndex)
{
 
if(EEPROM.read(eepIndex) != 0xff)
{
Serial.print(F("T->"));
if(EEPROM.read(eepIndex)<10)
Serial.print('0');
Serial.print(EEPROM.read(eepIndex++));
Serial.print(':');
if(EEPROM.read(eepIndex)<10)
Serial.print('0');
Serial.print(EEPROM.read(eepIndex++));
Serial.print(':');
if(EEPROM.read(eepIndex)<10)
Serial.print('0');
Serial.print(EEPROM.read(eepIndex++));
Serial.print(F(" D->"));
if(EEPROM.read(eepIndex)<10)
Serial.print('0');
Serial.print(EEPROM.read(eepIndex++));
Serial.print('/');
if(EEPROM.read(eepIndex)<10)
Serial.print('0');
Serial.print(EEPROM.read(eepIndex++));
Serial.print('/');
Serial.print(EEPROM.read(eepIndex++)<<8 | EEPROM.read(eepIndex++));
}
else
{
Serial.print(F("---------------------------"));
}
 
Serial.print(F(" "));
}
