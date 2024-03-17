#ifndef _SPI_FLASH_
#define _SPI_FLASH_

#include <Arduino.h>
#include <SPI.h>

#define W25Q40_IC 4     // 4M-BIT
#define W25Q80_IC 8     // 8M-BIT
#define W25Q16_IC 16    // 16M-BIT
#define W25Q32_IC 32    // 32M-BIT
#define W25Q64_IC 64    // 64M-BIT
#define W25Q128_IC 128  // 128M-BIT
#define W25Q256_IC 256  // 256M-BIT
#define W25Q512_IC 512  // 512M-BIT
#define W25Q01_IC 1024  // 1G-BIT (1024M-BIT) 32-BIT addr
#define W25Q02_IC 2048  // 2G-BIT (2048M-BIT) 32-BIT addr

// #define W25Qxx_switch 4 // 4M-BIT
// #define W25Qxx_switch 8 // 8M-BIT
// #define W25Qxx_switch 16 // 16M-BIT
#define W25Qxx_switch 32  // 32M-BIT
// #define W25Qxx_switch 64 // 64M-BIT
// #define W25Qxx_switch 128 // 128M-BIT
// #define W25Qxx_switch 256 // 256M-BIT
// #define W25Qxx_switch 512 // 512M-BIT
// #define W25Qxx_switch 1024 // 1G-BIT (1024M-BIT) 32-BIT addr
// #define W25Qxx_switch 2048 // 2G-BIT (2048M-BIT) 32-BIT addr

/*  For use in a global variable (array bound)
    - FLASH_BYTE/SECTOR_BYTE/BLOCK_BYTE/PAGE_BYTE
    - PAGE_SIZE/SECTOR_SIZE/BLOCK_BYTE
 */

/* Here need to edit manually - BEGIN, */
/*
    Or uncomment a line above "W25Qxx_switch"
    - The memories W25Qxx also accept 32kB blocks see datasheet
    - - Default: 64kB
 */

#ifdef W25Qxx_switch

#if W25Qxx_switch == 4
#define FLASH_BYTE (uint32_t)((512)) * 1024)  // 512kB (for W25Q32: "(4)")
#else                               // #if W25Qxx_switch == 4
#define FLASH_BYTE (uint32_t)((W25Qxx_switch / 8)) * 1024 * 1024)  //4MB (for W25Q32: "(4)")
#endif                                                   // #if W25Qxx_switch == 4

#else                                   // #ifdef W25Qxx_switch
#define FLASH_BYTE (uint32_t)((4) * 1024 * 1024)  //4MB (for W25Q32: "(4)")
#endif                                  // #ifdef W25Qxx_switch

#define BLOCK_BYTE (uint32_t)((64) * 1024)  //64kb (The memories W25Qxx also accept 32kB blocks see datasheet)
// #define BLOCK_BYTE (uint32_t)((32) * 1024)  //64kb (The memories W25Qxx also accept 32kB blocks see datasheet)
#define SECTOR_BYTE (uint16_t)((4) * 1024)  //4kb
#define PAGE_BYTE (uint16_t)((1) * 256)     //256byte
/* Here need to edit manually - END */

#define PAGE_SIZE (FLASH_BYTE / PAGE_BYTE)  // for W25Q32: ((1) * 1024) //1024*256 = 4MB
#define SECTOR_SIZE (FLASH_BYTE / SECTOR_BYTE)
#define BLOCK_SIZE (FLASH_BYTE / BLOCK_BYTE)

#define FLASH_OK 0
#define FLASH_ERR 1
#define FLASH_ERR_PARAM 2

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a, b) ((a) &= ~(1ULL << (b)))

// (Volatile/Non-Volatile Writable)
#define SR1_WRITE_IN_PROGRESS_bit 0
#define SR1_WRITE_ENABLE_LATCH_bit 1
#define SR1_BLOCK_PROT_BIT0_bit 2
#define SR1_BLOCK_PROT_BIT1_bit 3
#define SR1_BLOCK_PROT_BIT2_bit 4
#define SR1_TOP_BOTTOM_PROT_bit 5
#define SR1_SECTOR_PROT_bit 6
#define SR1_STATUS_REG_PROT_bit 7

// (Volatile/Non-Volatile Writable)
#define SR2_STATUS_REG_LOCK_bit 0
#define SR2_QUAD_ENABLE_bit 1
#define SR2_RESERVED2_SR2_bit 2
#define SR2_SECURITY_REG_LOCK_BIT0_bit 3
#define SR2_SECURITY_REG_LOCK_BIT1_bit 4
#define SR2_SECURITY_REG_LOCK_BIT2_bit 5
#define SR2_COMPLEMENT_PROt_bit 6
#define SR2_SUSPEND_STATUS_bit 7

// (Volatile/Non-Volatile Writable)
#define SR3_RESERVED0_bit 0
#define SR3_RESERVED1_bit 1
#define SR3_WRITE_PROT_SEL_bit 2
#define SR3_RESERVED3_bit 3
#define SR3_RESERVED4_bit 4
#define SR3_OUTPUT_DRV_STRENGTH_BIT0_bit 5
#define SR3_OUTPUT_DRV_STRENGTH_BIT1_bit 6
#define SR3_RESERVED5_bit 7

// From datasheet "W25Q80, W25Q16, W25Q32" - Winbond - September 26, 2007, Rev. B
#define Write_Status_Reg1_cmd 0x01
#define Page_Prog_cmd 0x02
#define Read_Data_cmd 0x03
#define Write_Disable_cmd 0x04
#define Read_Status_Reg1_cmd 0x05
#define Write_Enable_cmd 0x06
#define Fast_Read_cmd 0x0B
#define Write_Status_Reg3_cmd 0x11  // For W25Q32JV
#define Read_Status_Reg3_cmd 0x15   // For W25Q32JV
#define Sector_Erase_cmd 0x20
#define Write_Status_Reg2_cmd 0x31  // For W25Q32JV
#define Quad_Input_Page_Prog_cmd 0x32
#define Read_Status_Reg2_cmd 0x35
#define Individual_Block_Lock_cmd 0x36     // For W25Q32JV
#define Individual_Sector_Lock_cmd 0x36    // For W25Q32JV
#define Individual_Block_Unlock_cmd 0x39   // For W25Q32JV
#define Individual_Sector_Unlock_cmd 0x39  // For W25Q32JV
#define Fast_Read_Dual_Output_cmd 0x3B
#define Read_Block_Lock_cmd 0x3D     // For W25Q32JV
#define Read_Sector_Lock_cmd 0x3D    // For W25Q32JV
#define Prog_Security_Reg_cmd 0x42   // For W25Q32JV
#define Erase_Security_Reg_cmd 0x44  // For W25Q32JV
#define Read_Security_Reg_cmd 0x48   // For W25Q32JV
#define Read_Unique_ID_cmd 0x4B      // For W25Q16 this feature is available upon special request
#define Write_Enable_VSReg_cmd 0x50  // For W25Q32JV
#define Block_Erase_32KB_cmd 0x52
#define Read_SFDP_Reg_cmd 0x5A  // For W25Q32JV - See note 1
#define Chip_Erase2_cmd 0x60
#define Enable_Reset_cmd 0x66  // For W25Q32JV
#define Fast_Read_Quad_Output_cmd 0x6B
#define Erase_Suspend_cmd 0x75
#define Set_Burst_Wrap_cmd 0x77  // For W25Q32JV
#define Erase_Resume_cmd 0x7A
#define Global_Block_Lock_cmd 0x7E   // For W25Q32JV
#define Global_Sector_Lock_cmd 0x7E  // For W25Q32JV
#define Read_Manufacturer_cmd 0x90
#define Device_ID_cmd 0x90
#define Read_Manufacturer_Dual_IO_cmd 0x92  // For W25Q32JV
#define Device_ID_Dual_IO_cmd 0x92          // For W25Q32JV
#define Read_Manufacturer_Quad_IO_cmd 0x94  // For W25Q32JV
#define Device_ID_Quad_IO_cmd 0x94          // For W25Q32JV
#define Global_Block_Unlock_cmd 0x98        // For W25Q32JV
#define Global_Sector_Unlock_cmd 0x98       // For W25Q32JV
#define Reset_Device_cmd 0x99               // For W25Q32JV
#define JEDEC_ID_cmd 0x9F
#define High_Performance_Mode_cmd 0xA3
#define Release_PD_cmd 0xAB         // This instruction is a multi-purpose instruction
#define Release_HPM_cmd 0xAB        // This instruction is a multi-purpose instruction
#define Release_Device_ID_cmd 0xAB  // This instruction is a multi-purpose instruction
#define Power_Down_cmd 0xB9
#define Fast_Read_Dual_IO_cmd 0xBB
#define Chip_Erase1_cmd 0xC7
#define Block_Erase_64KB_cmd 0xD8
#define Fast_Read_Quad_IO_cmd 0xEB
#define Mode_Bit_Reset1_cmd 0xFF
#define Mode_Bit_Reset2_cmd 0xFFFF
/*
    Note 1:
      The W25Q32JV features a 256-Byte Serial Flash Discoverable Parameter (SFDP) register that contains
      information about device configurations, available instructions and other features. The SFDP parameters
      are stored in one or more Parameter Identification (PID) tables. Currently only one PID table is specified,
      but more may be added in the future. The Read SFDP Register instruction is compatible with the SFDP
      standard initially established in 2010 for PC and other applications, as well as the JEDEC standard
      JESD216-serials that is published in 2011. Most Winbond SpiFlash Memories shipped after June 2011
      (date code 1124 and beyond) support the SFDP feature as specified in the applicable datasheet.
 */

class SPIFlash {
public:
  SPIFlash();

  uint8_t begin(uint32_t IcModelSize_Mbit = W25Q32_IC, // Default: 32Mbits
                uint32_t BlockBytes = BLOCK_BYTE, // Default: 64kB
                uint16_t SectorBytes = SECTOR_BYTE, // Default: 4kB
                uint16_t PageBytes = PAGE_BYTE, // Default: 256B
                uint8_t CS_pin = 5,
                uint32_t speedMaximum = 4000000);

  uint8_t init_flash(void);

  /*
      Because of the small package and the limitation on the number of pins, the W25Q32JV provide a software
      Reset instruction instead of a dedicated RESET pin. Once the Reset instruction is accepted, any on-going
      internal operations will be terminated and the device will return to its default power-on state and lose all the
      current volatile settings, such as Volatile Status Register bits, Write Enable Latch (WEL) status,
      Program/Erase Suspend status, Read parameter setting (P7-P0), Continuous Read Mode bit setting (M7-
      M0) and Wrap Bit setting (W6-W4)
   */
  uint8_t reset_flash(void);

  uint16_t read_device_id(void);
  void read_register(uint8_t cmd, uint8_t *regData, uint8_t size);
  uint8_t write_register(uint8_t cmd, uint8_t *regData, uint8_t size);
  uint8_t write_cmd(uint8_t cmd);

  void write_addr(uint32_t cmd, uint32_t address);
  void wait_flash(void);

  uint8_t erase_chip(void);
  uint8_t erase_sector(uint16_t sector);

  uint8_t write_flash(uint32_t address, uint8_t *data, uint32_t size);
  uint8_t read_flash(uint32_t address, uint8_t *data, uint32_t size);

  uint8_t enable_qpi(void);

  // QUAD ENABLE, Status_Reg2:bit_1 (W25Q32JV - page 16)
  /*
      Quad Enable (QE) – Volatile/Non-Volatile Writable
      The Quad Enable (QE) bit is a non-volatile read/write bit in the status register (S9) that enables Quad SPI
      operation. When the QE bit is set to a 0 state (factory default for part numbers with ordering options “IM” &
      ”JM”), the /HOLD are enabled, the device operates in Standard/Dual SPI modes. When the QE bit is set to
      a 1 (factory fixed default for part numbers with ordering options “IQ” & “JQ”), the Quad IO2 and IO3 pins
      are enabled, and /HOLD function is disabled, the device operates in Standard/Dual/Quad SPI modes
   */
  uint8_t enable_quad_spi();
  uint8_t disable_quad_spi();

  uint32_t BYTE_FLASH;  // Value calculated automatically when executing Flash1.begin(W25Qxx_IC)

  uint16_t SIZE_PAGE;    // Value calculated automatically when executing Flash1.begin(W25Q32_IC)
  uint16_t SIZE_SECTOR;  // Value calculated automatically when executing Flash1.begin(W25Q32_IC)
  uint16_t SIZE_BLOCK;   // Value calculated automatically when executing Flash1.begin(W25Q32_IC)

private:
  uint8_t _FLASH_CS;
  uint32_t _speedMaximum;
  bool manually_setup = true;

  void flash_set_cs(uint8_t CS_pin);
  void write_enable(void);
};

#endif  // #ifndef _SPI_FLASH_