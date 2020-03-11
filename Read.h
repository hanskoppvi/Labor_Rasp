#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "/home/hans/Desktop/RaspPro/rfid/RPi-RFID/MFRC522.h"
#include "/home/hans/Desktop/RaspPro/rfid/RPi-RFID/bcm2835.h"

#define PIN_11 RPI_GPIO_P1_11   //Signal to Relay
#define PIN_12 RPI_GPIO_P1_12   //Red Light
#define PIN_16 RPI_GPIO_P1_16   //Green Light
#define PIN_15 RPI_GPIO_P1_15   //Buzzer

#define LOCK        RPI_GPIO_P1_11   //Signal to Relay
#define REDLIGHT    RPI_GPIO_P1_12   //Red Light
#define GREENLIGHT  RPI_GPIO_P1_16   //Green Light
#define BUZZER      RPI_GPIO_P1_15   //Buzzer

#define OPEN  0x01
#define CLOSE 0x00

#define TRUE 1
#define FALSE 0
#define START 3

#define MASTERKEYFILE "/root/MasterKey.txt"
#define USERIDFILE    "/root/UserID.txt"


ofstream PreviusIDTextfileWrite;  // Allows to write the SavedID 
ifstream PreviusIDTextfileRead;   // Allows to read the SavedID

ifstream masterKeyTextFile;       // Allows to read the MasterKey   

ostringstream IDString;           // Here will be saved the ID read from the card in a readeable and workable form
ostringstream PreviusIDString;    // Here will be ID of the SavedID loaded to be read
ostringstream masterKeyString;    // Here will be the MasterKey loaded

MFRC522 mfrc;

int isUsed = START;                   // 3 means the program just started | 0 means not used | 1 means used
int wasMasterKeyRead= 0;          // A variable to help to know is the previus read card was the master key for shutting down the program
char ID[20];                      // A temporaly variable to transform the PreviusID to a workable form

void delay(int ms){

  usleep(ms*1000);

}

using namespace std;

void init(){

  mfrc.PCD_Init();

  // All Pins are sent to Output mode
  bcm2835_gpio_fsel(PIN_11, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(PIN_12, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(PIN_16, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(PIN_15, BCM2835_GPIO_FSEL_OUTP);

  // All Pins are set to low
  bcm2835_gpio_write(REDLIGHT, HIGH);
  bcm2835_gpio_write(GREENLIGHT, HIGH);
  bcm2835_gpio_write(BUZZER, LOW);
  bcm2835_gpio_write(LOCK, CLOSE);
  
}

// ------ HARDWARE FUNCTIONS--------//
void turnON(int light){
  bcm2835_gpio_write(light, HIGH);
}
void turnOFF(int light){
  bcm2835_gpio_write(light, LOW);
}

void userLock(){

  wasMasterKeyRead = 0;

  PreviusIDTextfileWrite.open (USERIDFILE); // If Empty write new ID
  PreviusIDTextfileWrite << IDString.str();
  PreviusIDTextfileWrite.close();

  turnON(REDLIGHT);
  turnOFF(GREENLIGHT);
  bcm2835_gpio_write(LOCK, CLOSE);
      
  isUsed = TRUE;
}

void userUnlock(){

  turnOFF(REDLIGHT);
  turnON(GREENLIGHT);
  bcm2835_gpio_write(LOCK, OPEN);
  isUsed = FALSE;
}

// ------ MAIN FUNCTIONS ----//

void loadMasterKey(){ 
   masterKeyTextFile.open(MASTERKEYFILE);
  char temp[20];
  if (masterKeyTextFile.is_open())
  {
    while (!masterKeyTextFile.eof())
    {
      masterKeyTextFile >> temp;
    }  
  } 
  masterKeyTextFile.close();
  masterKeyString << temp;
}

void printCardID(){
  // Print UID
    for(int i = 0; i < mfrc.uid.size; ++i)
    {
      if(mfrc.uid.uidByte[i] < 0x10){
	      printf(" 0");
        printf("%X",mfrc.uid.uidByte[i]);
        }
      else{
	      printf(" ");
	      printf("%X", mfrc.uid.uidByte[i]);
      }
      
      IDString << (int)mfrc.uid.uidByte[i]; //Saves ID in a readable form
    }
    printf("\n");
}

void openPreviusID(){ // Open Saved ID
  PreviusIDTextfileRead.open(USERIDFILE);
      
  if (PreviusIDTextfileRead.is_open())
    {
      while (!PreviusIDTextfileRead.eof())
        {
          PreviusIDTextfileRead >> ID;
        }
   }
  PreviusIDTextfileRead.close();
  PreviusIDString << ID;

}

void clearTextVariables(){ // Clear Variable IDstring and PrevuisID
    
    IDString.str("");
    IDString.clear();
    PreviusIDString.str("");
    PreviusIDString.clear();

}

// ------ ROUTINES --------//

void routineMasterCardRead(){
  bcm2835_gpio_write(REDLIGHT, LOW);
  bcm2835_gpio_write(GREENLIGHT, LOW);
  delay(100);
  bcm2835_gpio_write(REDLIGHT, HIGH);
  bcm2835_gpio_write(GREENLIGHT, HIGH);
  delay(100);
  bcm2835_gpio_write(REDLIGHT, LOW);
  bcm2835_gpio_write(GREENLIGHT, LOW);
  delay(100);
  bcm2835_gpio_write(REDLIGHT, HIGH);
  bcm2835_gpio_write(GREENLIGHT, HIGH);
  delay(100);
  bcm2835_gpio_write(REDLIGHT, LOW);
  bcm2835_gpio_write(GREENLIGHT, LOW);
  delay(100);
  bcm2835_gpio_write(REDLIGHT, LOW);
  bcm2835_gpio_write(GREENLIGHT, HIGH);
  delay(500);
  bcm2835_gpio_write(LOCK, OPEN);
  isUsed = 0;
}

void routineClosingProgram(){
  printf(" CLOSING PROGRAM\n");

  bcm2835_gpio_write(GREENLIGHT, HIGH);
  delay(1000);
  bcm2835_gpio_write(GREENLIGHT, LOW);
  bcm2835_gpio_write(REDLIGHT, HIGH);
  delay(1000);

  userLock();
  bcm2835_gpio_write(REDLIGHT, LOW);
}

void errorSound(){
  bcm2835_gpio_write(BUZZER, HIGH);
  delay(50);
  bcm2835_gpio_write(BUZZER, LOW);
}

void enterAdminModus(){

  printf(" YOU ARE IS MASTER MODE\n");
  turnON(GREENLIGHT);
  turnON(REDLIGHT);

}



