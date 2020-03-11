#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "/home/hans/Desktop/RFID_PRO/Labor_Rasp/Read.h"

int main(){

  printf("WELCOME \n");
  init();
  loadMasterKey();

  while(isUsed == START)
  {
    
    // Look for a card
    if(!mfrc.PICC_IsNewCardPresent())
    continue;

    if(!mfrc.PICC_ReadCardSerial())
    continue;

    printCardID();

    if (masterKeyString.str() == IDString.str()) routineMasterCardRead();
    else errorSound();

    delay(100);
    
    IDString.str("");
    IDString.clear();
    
  }

//--------------------APP----------------------------------//
  
  while(1)
  {
    // Look for a card
    if(!mfrc.PICC_IsNewCardPresent())
    continue;

    if(!mfrc.PICC_ReadCardSerial())
    continue;

    openPreviusID();
    printCardID();

    
    //Compare with MasterKey
    if (masterKeyString.str() == IDString.str()){
      if (wasMasterKeyRead){
        routineClosingProgram();
        return 0;
      }

      wasMasterKeyRead = 1;

      enterAdminModus();
      
      if (isUsed)
        routineMasterCardRead();

    }

    // Is read ID same as Saved ?
    if (isUsed){

      if (PreviusIDString.str() == IDString.str())
        userUnlock();
      else 
        errorSound();

    }
    else{
      if (IDString.str() != masterKeyString.str()) // Avoids comflict if the read card was the MasterKey
        userLock();
    }

    clearTextVariables();

    delay(500);
    
  }
  
  return 0;
}
