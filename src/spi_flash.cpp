#include "spi_flash.h"

SPIFlash::SPIFlash() {}

void SPIFlash::flash_set_cs(uint8_t CS_pin) {
  _FLASH_CS = CS_pin;
}

uint8_t SPIFlash::begin(uint32_t IcModelSize_Mbit, uint8_t CS_pin, uint32_t speedMaximum) {
  FLASH_BYTE = ((IcModelSize_Mbit / 8) * 1024 * 1024);  // BIT to BYTE: (IcModelSize_Mbit / 8)
  PAGE_SIZE = (FLASH_BYTE / PAGE_BYTE);             // For W25Q32: ((1) * 1024) / ((1) * 256) = 4MB bytes
  SECTOR_SIZE = (FLASH_BYTE / SECTOR_BYTE);         // For W25Q32: ((1) * 1024) / ((4) * 1024) = 256 sectors
  BLOCK_SIZE = (FLASH_BYTE / BLOCK_BYTE);           // For W25Q32: ((1) * 1024) / ((64) * 1024) = 64 blocks

  flash_set_cs(CS_pin);

  _speedMaximum = speedMaximum;

  return init_flash();
}

uint8_t SPIFlash::init_flash(void) {
  uint8_t status = FLASH_OK;
  uint8_t wr[1] = { 0x00 };

  pinMode(_FLASH_CS, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(_speedMaximum, MSBFIRST, SPI_MODE0));
  //write_enable();
  digitalWrite(_FLASH_CS, HIGH);
  SPI.transfer(Release_PD_cmd);  // Send POWER command
  read_device_id();
  status = write_register(Write_Status_Reg1_cmd, wr, 1);  //remove PROTECT
  wait_flash();
  return status;
}

void SPIFlash::read_register(uint8_t cmd, uint8_t *regData, uint8_t size) {
  digitalWrite(_FLASH_CS, LOW);
  SPI.transfer(cmd);  // Send JEDEC ID command
  for (uint8_t i = 0; i < size; i++) {
    regData[i] = SPI.transfer(0x00);
  }
  digitalWrite(_FLASH_CS, HIGH);
}

uint8_t SPIFlash::write_register(uint8_t cmd, uint8_t *regData, uint8_t size) {
  uint8_t status = FLASH_OK;
  wait_flash();
  digitalWrite(_FLASH_CS, LOW);
  SPI.transfer(cmd);  // Send WRITE ID command
  for (uint8_t i = 0; i < size; i++) {
    status = SPI.transfer(regData[i]);
  }
  digitalWrite(_FLASH_CS, HIGH);
  return status;
}

uint8_t SPIFlash::write_cmd(uint8_t cmd) {
  uint8_t status = FLASH_OK;
  digitalWrite(_FLASH_CS, LOW);
  status = SPI.transfer(cmd);  // send RESUME command
  digitalWrite(_FLASH_CS, HIGH);
  return status == FLASH_OK;
}

void SPIFlash::write_addr(uint32_t cmd, uint32_t address) {
  SPI.transfer(cmd);
#if SET_W25Q01 == 1 || SET_W25Q02 == 1
  // 1G-BIT/2G-BIT addr: 32-BIT
  SPI.transfer((address >> 24) & 0xff);  //32-BIT (MSB)
#endif
  SPI.transfer((address >> 16) & 0xff);  //24-BIT (MSB)
  SPI.transfer((address >> 8) & 0xff);
  SPI.transfer(address & 0xff);  // (LSB)
}

void SPIFlash::wait_flash() {
  uint8_t reg1[1], is_busy = 1;
  while (is_busy) {
    read_register(Read_Status_Reg1_cmd, reg1, 1);
    is_busy = ((reg1[0]) & 0x01);
    delay(1);
  }
}

void SPIFlash::write_enable() {
  write_cmd(Write_Enable_cmd);
  //Wait
  uint8_t reg1[1], is_enable = 0;
  while (!is_enable) {
    read_register(Read_Status_Reg1_cmd, reg1, 1);
    is_enable = ((reg1[0]) & 0x02);
    delay(1);
  }
}

uint8_t SPIFlash::erase_chip() {
  uint8_t status = 1;
  // uint8_t reg1[1];
  //enable
  write_enable();
  //erase
  status = write_cmd(Chip_Erase1_cmd);
  //waiting
  wait_flash();
  return status == FLASH_OK;
}

uint8_t SPIFlash::erase_sector(uint16_t sector) {
  if (sector > SECTOR_SIZE) {
    return FLASH_ERR_PARAM;
  }
  //enable
  write_enable();
  //resume
  write_cmd(Erase_Resume_cmd);
  //erase sector
  digitalWrite(_FLASH_CS, LOW);
  write_addr(Sector_Erase_cmd, sector * SECTOR_BYTE);
  digitalWrite(_FLASH_CS, HIGH);
  //waiting finish
  wait_flash();
  return FLASH_OK;
}

uint8_t SPIFlash::write_flash(uint32_t address, uint8_t *data, uint32_t size) {
  uint8_t status = FLASH_OK;
  int32_t page_byte = size;

  if (size > (PAGE_SIZE * PAGE_BYTE)) {
    return FLASH_ERR_PARAM;
  }
  //write map
  while (page_byte > 0) {
    //enable
    write_enable();
    size = ((page_byte > PAGE_BYTE) ? PAGE_BYTE : page_byte);
    //write page
    digitalWrite(_FLASH_CS, LOW);
    write_addr(Page_Prog_cmd, address);
    //write data
    for (uint8_t i = 0; i < (size); i++) {
      status = SPI.transfer(data[i]);
    }
    digitalWrite(_FLASH_CS, HIGH);
    address += PAGE_BYTE;
    page_byte -= PAGE_BYTE;
    //waiting finish
    wait_flash();
  }

  return status;
}

uint8_t SPIFlash::read_flash(uint32_t address, uint8_t *data, uint32_t size) {
  uint8_t status = FLASH_OK;
  digitalWrite(_FLASH_CS, LOW);
  write_addr(Read_Data_cmd, address);
  for (uint32_t i = 0; i < size; i++) {
    data[i] = SPI.transfer(0x00);
  }
  digitalWrite(_FLASH_CS, HIGH);
  return status;
}

uint16_t SPIFlash::read_device_id() {
  uint8_t read_data[3];
  read_register(JEDEC_ID_cmd, read_data, 3);
  return read_data[0] << 8 | read_data[1];
}

/*
    Because of the small package and the limitation on the number of pins, the W25Q32JV provide a software
    Reset instruction instead of a dedicated RESET pin. Once the Reset instruction is accepted, any on-going
    internal operations will be terminated and the device will return to its default power-on state and lose all the
    current volatile settings, such as Volatile Status Register bits, Write Enable Latch (WEL) status,
    Program/Erase Suspend status, Read parameter setting (P7-P0), Continuous Read Mode bit setting (M7-
    M0) and Wrap Bit setting (W6-W4)
 */
uint8_t SPIFlash::reset_flash() {
  uint8_t status = FLASH_OK;
  write_cmd(Enable_Reset_cmd);
  wait_flash();
  status = write_cmd(Reset_Device_cmd);
  wait_flash();
  return status == FLASH_OK;
}

// QUAD ENABLE, Status_Reg2:bit_1 (W25Q32JV - page 16)
/*
    Quad Enable (QE) – Volatile/Non-Volatile Writable
    The Quad Enable (QE) bit is a non-volatile read/write bit in the status register (S9) that enables Quad SPI
    operation. When the QE bit is set to a 0 state (factory default for part numbers with ordering options “IM” &
    ”JM”), the /HOLD are enabled, the device operates in Standard/Dual SPI modes. When the QE bit is set to
    a 1 (factory fixed default for part numbers with ordering options “IQ” & “JQ”), the Quad IO2 and IO3 pins
    are enabled, and /HOLD function is disabled, the device operates in Standard/Dual/Quad SPI modes
 */
uint8_t SPIFlash::enable_quad_spi() {
  uint8_t status = FLASH_OK;
  uint8_t reg2[1] = {0};
  read_register(Read_Status_Reg2_cmd, reg2, 1);
  BIT_SET(reg2[0], SR2_QUAD_ENABLE_bit);
  status = write_register(Write_Status_Reg2_cmd, reg2, 1);
  wait_flash();
  return status == FLASH_OK;
}

// QUAD ENABLE, Status_Reg2:bit_1 (W25Q32JV - page 16)
/*
    Quad Enable (QE) – Volatile/Non-Volatile Writable
    The Quad Enable (QE) bit is a non-volatile read/write bit in the status register (S9) that enables Quad SPI
    operation. When the QE bit is set to a 0 state (factory default for part numbers with ordering options “IM” &
    ”JM”), the /HOLD are enabled, the device operates in Standard/Dual SPI modes. When the QE bit is set to
    a 1 (factory fixed default for part numbers with ordering options “IQ” & “JQ”), the Quad IO2 and IO3 pins
    are enabled, and /HOLD function is disabled, the device operates in Standard/Dual/Quad SPI modes
 */
uint8_t SPIFlash::disable_quad_spi() {
  uint8_t status = FLASH_OK;
  uint8_t reg2[1] = {0};
  read_register(Read_Status_Reg2_cmd, reg2, 1);
  BIT_CLEAR(reg2[0], SR2_QUAD_ENABLE_bit);
  status = write_register(Write_Status_Reg2_cmd, reg2, 1);
  wait_flash();
  return status == FLASH_OK;
}

uint8_t SPIFlash::enable_qpi() {
  uint8_t status = FLASH_OK;
  status = write_cmd(0x38);
  wait_flash();
  return status == FLASH_OK;
}