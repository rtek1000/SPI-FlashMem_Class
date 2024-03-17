//library
/**
  author: hmz06967
  @link: https://github.com/hmz06967/SPI-FlashMem
*/
/**
  modified by: rtek1000
  @link: https://github.com/rtek1000/SPI-FlashMem_Class
*/

//#define loop_test

#include "spi_flash.h"

SPIFlash Flash1;

/*  For use in a global variable (array bound)
    - Need to edit manually, see spi_flash.h:
    - FLASH_BYTE/SECTOR_BYTE/BLOCK_BYTE/PAGE_BYTE
    - PAGE_SIZE/SECTOR_SIZE/BLOCK_BYTE
 */
// Global variables
uint8_t read_data[PAGE_BYTE]; // Default SECTOR_BYTE: 4kB
uint8_t write_data[PAGE_BYTE];

uint16_t test_number, test_addr = 0;
char pData[50];

// functions prototype
void read_jedec();
void full_erase();
void read_flash_w(uint32_t size);

void setup() {
  Serial.begin(115200);
  Serial.println("merhaba");

  //writes 1024
  uint32_t len = 1024;
  uint8_t data[1] = { 0 };
  for (uint32_t i = 0; i < len; i++) {
    data[0] = (i & 0xff);
    Flash1.write_flash(i, data, 1);
  }

  //reset
  Serial.print(F("Begin flash-> "));
  if (Flash1.begin(W25Q32_IC) != FLASH_OK) {
    Serial.println(F("Error"));
    while (1)
      ;
  }
  Serial.println(F("OK"));

  delay(10);

#ifdef erase
  Flash1.full_erase();
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
  read_jedec();


#ifdef test
  //write enable
  //write_enable();

  //sector 0 erase
  Serial.println(F("***** ERASE SECTOR *****"));
  Flash1.erase_sector(0);

  //write
  Serial.println(F("***** WRITE *****"));
  write_data[0] = 212;
  Flash1.write_flash(0, write_data, 1);

  //read
  Serial.println(F("***** READ *****"));
  Flash1.read_flash(0, read_data, 1);
  Serial.println(read_data[0], HEX);

#endif

  Serial.println();

  Serial.println("Available operations:");
  Serial.println("'i' - chip deviceID");
  Serial.println("'r' - dump first 256 bytes on the chip");
  Serial.println("'w' - write flash number");
  Serial.println("'e' - erase entire flash chip");
  Serial.println("************************\n");
}


#define write

void loop() {
  delay(10);  // 1 saniye bekle

  if (Serial.available() > 0) {
    char read = Serial.read();
    switch (read) {
      case 'i':
        read_jedec();
        break;
      case 'r':
        {
          Serial.println("Reading..");
          read_flash_w(0x100);
          Serial.println();
        }
        break;
      case 'w':
        {
          test_addr = 0;
          Serial.println("Enter number: ");
          Flash1.erase_sector(0);
          while (read != '\n') {  // Dizi boyutuna ve veriye erişilebilirlik kontrolü
            read = Serial.read();
            if (read >= '0' && read < ':') {
              write_data[test_addr] = (uint8_t)((uint8_t)read - '0');  // Diziye aktar
              Serial.print(write_data[test_addr], HEX);
              test_addr++;  // Dizi indeksini artır
            }
            delay(10);  // Küçük bir gecikme
          }

          if (Flash1.write_flash(0, write_data, test_addr) != FLASH_OK) {
            Serial.println("->Err");
          }
          Serial.println("->OK");
          Serial.print("read:");
          read_flash_w(test_addr);
        }
        break;
      case 'e':
        full_erase();
        break;
      default:
        break;
    }
    Serial.flush();
  }

#ifdef loop_test

//write
#ifdef write
  write_data[0] = test_number;
  if (Flash1.write_flash(test_addr, write_data, 1) != FLASH_OK) {
    Serial.println("Err");
    while (1)
      ;
  }
#endif
  //sprintf(pData, "[%ld] Write-> %d\n",test_addr, write_data[0]);
  //Serial.print(pData);

  //read
  Flash1.read_flash(test_addr, read_data, 1);
  sprintf(pData, "[%ld] Read-> %d\r\n", test_addr, read_data[0]);
  Serial.print(pData);

  test_addr = ((test_addr + 1) & (PAGE_SIZE - 1));
  test_number = ((read_data[0] + 1) & 0xff);

  if (test_addr & 0xff) return;
#endif
}

void read_jedec() {
  Serial.print(F("JEDEC-> "));
  Flash1.read_register(0x9F, read_data, 3);
  Serial.print(read_data[0], HEX);
  Serial.print(" ");
  Serial.print(read_data[1], HEX);
  Serial.print(" ");
  Serial.println(read_data[2], HEX);
}

void full_erase() {
  //chip erase
  Serial.print(F("FULL ERASE-> "));
  uint32_t time1 = millis();
  Flash1.erase_chip();
  uint32_t time2 = millis();
  Serial.print((time2 - time1) / 1000);
  Serial.println(F(" sec."));
}

void read_flash_w(uint32_t size) {
  if (sizeof(read_data) < size) {
    return; // Not enough array bounds
  }

  Flash1.read_flash(0, read_data, size);
  for (uint16_t i = 0; i < size; i++) {
    Serial.print(read_data[i], HEX);
    Serial.print(" ");
    if (i % 32 == 0 && i) Serial.println();
  }
}