#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "/home/hans/Desktop/RaspPro/rfid/RPi-RFID/MFRC522.h"
#include "/home/hans/Desktop/RaspPro/rfid/RPi-RFID/bcm2835.h"

#define PIN_11 RPI_GPIO_P1_11
#define PIN_12 RPI_GPIO_P1_13
#define MASTERKEYFILE "MasterKey.txt"
#define USERIDFILE "UserID"

void delay(int ms){

  usleep(ms*1000);

}

using namespace std;


int main(){
  
  ofstream userID;
  ifstream masterKeyTextFile;
  ostringstream IDString;
  ostringstream masterKeyString;
  MFRC522 mfrc;

  mfrc.PCD_Init();
  bcm2835_gpio_fsel(PIN_11, BCM2835_GPIO_FSEL_OUTP);

  masterKeyTextFile.open(MASTERKEYFILE);
  char output[11];
  if (masterKeyTextFile.is_open())
  {
    while (!masterKeyTextFile.eof())
    {
      masterKeyTextFile >> output;
    }
    
  } 
  masterKeyTextFile.close();

  masterKeyString << output;



  

  while(1){

    // Look for a card
    if(!mfrc.PICC_IsNewCardPresent())
    continue;

    if( !mfrc.PICC_ReadCardSerial())
    continue;

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

    // Writes down the ID from the card
    userID.open (USERIDFILE);
    userID << IDString.str();
    userID.close();

    
    if (masterKeyString.str() == IDString.str())
    {
      bcm2835_gpio_write(PIN_11, HIGH);
       bcm2835_gpio_write(PIN_12, HIGH);
    }
    else
    {
      bcm2835_gpio_write(PIN_11, LOW);
       bcm2835_gpio_write(PIN_12, LOW);
    }
    
    
   

    // Clear Variable IDstring
    IDString.str("");
    IDString.clear();

    
    delay(500);
  }
  return 0;
}
