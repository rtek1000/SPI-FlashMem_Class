//library
/**
  author: hmz06967
  @link: https://github.com/hmz06967/SPI-FlashMem
*/
/**
  modified by: rtek1000
  @link: https://github.com/rtek1000/SPI-FlashMem_Class
*/

#include "spi_flash.h"

SPIFlash Flash1;

uint8_t read_data[SECTOR_BYTE];
uint8_t write_data[SECTOR_BYTE];
uint16_t test_number, test_addr = 0;
char pData[50];

void setup() {

  Serial.begin(115200);
  Serial.println("merhaba");

  //writes 1024
  uint32_t  len = 1024;
  uint8_t data[1];

  for(uint32_t i=0;i<len;i++){
    data[0] = i & 0xff;

    Flash1.write_flash(i, data, 1);
  }

  //reset
  Serial.print(F("Begin flash-> "));
  if(Flash1.begin(W25Q32_IC) !=FLASH_OK){
    Serial.println(F("Error"));
    while(1);
  }
  Serial.println(F("OK"));

  delay(10);

#ifdef erase
  //chip erase
  Serial.print(F("FULL ERASE-> "));
  uint32_t  time1 = millis();
  Flash1.erase_chip();
  uint32_t  time2 = millis();
  Serial.print((time2 - time1) / 1000);
  Serial.println(F(" sec."));
#endif
  
  //read reg1
  Serial.print(F("REG[1]-> "));
  Flash1.read_register(0x05, read_data, 2);
  Serial.println(read_data[0]); 

  //read reg2
  Serial.print(F("REG[2]-> "));
  Flash1.read_register(0x35, read_data, 2);
  Serial.println(read_data[0]);
  
  //read jedec
  Serial.print(F("JEDEC-> "));
  Flash1.read_register(0x9F, read_data, 3);
  Serial.print(read_data[0], HEX);
  Serial.print(" ");
  Serial.print(read_data[1], HEX);
  Serial.print(" ");
  Serial.println(read_data[2], HEX);

#ifdef test
  //write enable
  //write_enable();

  //sector 0 erase
  Serial.println(F("***** ERASE SECTOR *****"));
  Flash1.erase_sector(0);

  //write
  Serial.println(F("***** WRITE *****"));
  Flash1.write_data[0] = 212;
  Flash1.write_flash(0, write_data, 1); 

  //read
  Serial.println(F("***** READ *****"));
  Flash1.read_flash(0, read_data, 1);
  Serial.println(read_data[0], HEX); 

#endif

  Serial.println();
}


void loop() {
  delay(10); // 1 saniye bekle
#ifdef loop_test
  //write
  write_data[0] = test_number;
  if(Flash1.write_flash(test_addr, write_data, 1) != FLASH_OK){
    Serial.println("Err");
    while(1);
  } 

  //sprintf(pData, "[%ld] Write-> %d\n",test_addr, write_data[0]);
  //Serial.print(pData);

  //read
  Flash1.read_flash(test_addr, read_data, 1);
  sprintf(pData, "[%ld] Read-> %d\n",test_addr, read_data[0]);
  Serial.print(pData);

  
  test_addr = ((test_addr+1) & (PAGE_SIZE - 1));
  test_number = ((read_data[0]+1) & 0xff);
#endif
}
