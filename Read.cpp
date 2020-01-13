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

#define MASTERKEYFILE "MasterKey.txt"
#define USERIDFILE    "UserID.txt"

void delay(int ms){

  usleep(ms*1000);

}

using namespace std;

// process to load the master key
void loadMasterKey(ostringstream masterKeyString)
{ 
  
}

int main(){

  printf("WELCOME \n");
  

  ofstream PreviusIDTextfileWrite;  // Allows to write the SavedID 
  ifstream PreviusIDTextfileRead;   // Allows to read the SavedID

  ifstream masterKeyTextFile;       // Allows to read the MasterKey   

  ostringstream IDString;           // Here will be saved the ID read from the card in a readeable and workable form
  ostringstream PreviusIDString;    // Here will be ID of the SavedID loaded to be read
  ostringstream masterKeyString;    // Here will be the MasterKey loaded

  MFRC522 mfrc;

  int isUsed = 3;                   // 3 means the program just started | 0 means not used | 1 means used
  int wasMasterKeyRead= 0;          // A variable to help to know is the previus read card was the master key for shutting down the program
  char ID[20];                      // A temporaly variable to transform the PreviusID to a workable form

  mfrc.PCD_Init();

  // All Pins are sent to Output mode
  bcm2835_gpio_fsel(PIN_11, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(PIN_12, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(PIN_16, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(PIN_15, BCM2835_GPIO_FSEL_OUTP);

  // All Pins are set to low
  bcm2835_gpio_write(REDLIGHT, LOW);
  bcm2835_gpio_write(GREENLIGHT, LOW);
  bcm2835_gpio_write(BUZZER, LOW);
  bcm2835_gpio_write(LOCK, CLOSE);

  

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
  


//--------------------APP----------------------------------//

  while(1)
  {
    

    // Look for a card
    if(!mfrc.PICC_IsNewCardPresent())
    continue;

    if(!mfrc.PICC_ReadCardSerial())
    continue;

    // Open Saved ID
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

    //Compare with MasterKey
    if (masterKeyString.str() == IDString.str())
    {
      if (wasMasterKeyRead == 1)
      {
        printf(" CLOSING PROGRAM\n");

        bcm2835_gpio_write(GREENLIGHT, HIGH);
        delay(1000);
        bcm2835_gpio_write(GREENLIGHT, LOW);
        bcm2835_gpio_write(REDLIGHT, HIGH);
        delay(1000);
        bcm2835_gpio_write(REDLIGHT, LOW);
        bcm2835_gpio_write(LOCK, CLOSE);
        
        return 0;
      }
      wasMasterKeyRead = 1;
      bcm2835_gpio_write(GREENLIGHT, HIGH);
      bcm2835_gpio_write(REDLIGHT, HIGH);
      printf(" YOU ARE IS MASTER MODE\n");
      if (isUsed == 3)
      {
        delay(1000);
        bcm2835_gpio_write(REDLIGHT, HIGH);
        bcm2835_gpio_write(GREENLIGHT, HIGH);
        bcm2835_gpio_write(LOCK, OPEN);
        isUsed = 0;

      }
      else
      {
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
        bcm2835_gpio_write(REDLIGHT, HIGH);
        bcm2835_gpio_write(GREENLIGHT, HIGH);
        delay(100);

        bcm2835_gpio_write(REDLIGHT, HIGH);
        bcm2835_gpio_write(GREENLIGHT, HIGH);
        delay(100);
        bcm2835_gpio_write(LOCK, OPEN);
        isUsed = 0;
      }
    }
    else
    {
      wasMasterKeyRead = 0;
    }

    // Is read ID same as Saved ?
    if (isUsed == 1)
    {
      if (PreviusIDString.str() == IDString.str()) // If they match 
      {
        bcm2835_gpio_write(REDLIGHT, LOW);
        bcm2835_gpio_write(GREENLIGHT, HIGH);
        bcm2835_gpio_write(LOCK, OPEN);
        isUsed = 0;
      }
      else // Error sound
      {
        bcm2835_gpio_write(BUZZER, HIGH);
        delay(50);
        bcm2835_gpio_write(BUZZER, LOW);
      } 
    }
    else
    if (IDString.str() != masterKeyString.str()) // Avoids comflict if the read card was the MasterKey
    {
      PreviusIDTextfileWrite.open (USERIDFILE); // If Empty write new ID
      PreviusIDTextfileWrite << IDString.str();
      PreviusIDTextfileWrite.close();

      bcm2835_gpio_write(REDLIGHT, HIGH);
      bcm2835_gpio_write(GREENLIGHT, LOW);
      bcm2835_gpio_write(LOCK, CLOSE);
      
      isUsed = 1;
    }
  
    // Clear Variable IDstring and PrevuisID
    IDString.str("");
    IDString.clear();
    PreviusIDString.str("");
    PreviusIDString.clear();

    delay(500);
  }
  return 0;
}
