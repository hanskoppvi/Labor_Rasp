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


int main(){
  

  ofstream PreviusIDTextfileWrite;
  ifstream PreviusIDTextfileRead;

  ifstream masterKeyTextFile;

  ostringstream IDString;
  ostringstream PreviusIDString;
  ostringstream masterKeyString;
  MFRC522 mfrc;

  int isUsed = 3;
  char temp[11];

  mfrc.PCD_Init();

  
  bcm2835_gpio_fsel(PIN_11, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(PIN_12, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(PIN_16, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(PIN_15, BCM2835_GPIO_FSEL_OUTP);

  bcm2835_gpio_write(REDLIGHT, LOW);
  bcm2835_gpio_write(GREENLIGHT, LOW);
  bcm2835_gpio_write(BUZZER, LOW);
  bcm2835_gpio_write(LOCK, CLOSE);

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




//--------------------APP----------------------------------//

  while(1)
  {
    // Open Saved ID
    PreviusIDTextfileRead.open(USERIDFILE);
      
      if (PreviusIDTextfileRead.is_open())
      {
        while (!PreviusIDTextfileRead.eof())
        {
          PreviusIDTextfileRead >> temp;
        }
    
      }
    PreviusIDTextfileRead.close();

    PreviusIDString << temp;

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

    //Compare with MasterKey
    if (masterKeyString.str() == IDString.str())
    {
      bcm2835_gpio_write(GREENLIGHT, HIGH);
      bcm2835_gpio_write(REDLIGHT, HIGH);
      printf(" YOU ARE IS MASTER MODE\n");
      if (isUsed == 3)
      {
        delay(1000);
        bcm2835_gpio_write(REDLIGHT, LOW);
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

        bcm2835_gpio_write(REDLIGHT, LOW);
        bcm2835_gpio_write(GREENLIGHT, HIGH);
        delay(100);
        bcm2835_gpio_write(LOCK, OPEN);
        isUsed = 0;
      }
      
    }

    if (isUsed == 1)
    {
      if (PreviusIDString.str() == IDString.str()) // Is read ID same as Saved ?
      {
        bcm2835_gpio_write(REDLIGHT, LOW);
        bcm2835_gpio_write(GREENLIGHT, HIGH);
        bcm2835_gpio_write(LOCK, OPEN);
        isUsed = 0;
      }
      else
      {
        bcm2835_gpio_write(BUZZER, HIGH);
        delay(500);
        bcm2835_gpio_write(BUZZER, LOW);
      } 
    }
    else
    if (IDString.str() != masterKeyString.str()) //
    {
      PreviusIDTextfileWrite.open (USERIDFILE); // If Empty write new ID
      PreviusIDTextfileWrite << IDString.str();
      PreviusIDTextfileWrite.close();

      bcm2835_gpio_write(REDLIGHT, HIGH);
      bcm2835_gpio_write(GREENLIGHT, LOW);
      bcm2835_gpio_write(LOCK, CLOSE);
      
      isUsed = 1;
    }
  
    // Clear Variable IDstring
    IDString.str("");
    IDString.clear();
    PreviusIDString.str("");
    PreviusIDString.clear();

    delay (500);
  }
  return 0;
}
