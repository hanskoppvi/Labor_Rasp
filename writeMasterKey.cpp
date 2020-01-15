#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "/home/hans/Desktop/RaspPro/rfid/RPi-RFID/MFRC522.h"
#include "/home/hans/Desktop/RaspPro/rfid/RPi-RFID/bcm2835.h"

#define MASTERKEYFILE "MasterKey.txt"
#define USERIDFILE    "UserID.txt"

#define PIN_11 RPI_GPIO_P1_11   //Signal to Relay
#define PIN_12 RPI_GPIO_P1_12   //Red Light
#define PIN_16 RPI_GPIO_P1_16   //Green Light
#define PIN_15 RPI_GPIO_P1_15   //Buzzer



using namespace std;


int main(){

  printf("WELCOME \n");
  
  ifstream masterKeyTextFileRead;       // Allows read the MasterKey 
  ofstream masterKeyTextFileWrite;       // Allows to write the MasterKey 
  ostringstream IDString;           // Here will be saved the ID read from the card in a readeable and workable form
  ostringstream masterKeyString;    // Here will be the MasterKey loaded

  MFRC522 mfrc;

  char ID[20];                      // A temporaly variable to transform the PreviusID to a workable form

  mfrc.PCD_Init();

  // All Pins are sent to Output mode
  bcm2835_gpio_fsel(PIN_11, BCM2835_GPIO_FSEL_OUTP);
  
  // process to load the master key
  masterKeyTextFileRead.open(MASTERKEYFILE);
  char temp[20];
  if (masterKeyTextFileRead.is_open())
  {
    while (!masterKeyTextFileRead.eof())
    {
      masterKeyTextFileRead >> temp;
    }
    
  } 
  masterKeyTextFileRead.close();

  masterKeyString << temp;
  
  int a = 1;


//--------------------APP----------------------------------//

 
    // Look for a card
    while(!mfrc.PICC_IsNewCardPresent());

    while(!mfrc.PICC_ReadCardSerial());

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
    
    masterKeyTextFileWrite.open (MASTERKEYFILE); 
    masterKeyTextFileWrite << IDString.str();
    masterKeyTextFileWrite.close();
      
    // Clear Variable IDstring and PrevuisID
    IDString.str("");
    IDString.clear();

    delay(500);
    printf("MasterKey overwritten \n");
    a++;
  

  
  return 0;

}
