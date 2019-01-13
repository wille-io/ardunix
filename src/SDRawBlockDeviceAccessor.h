#pragma once

#include "ardunix.h"
#include "AbstractRawBlockDevice.h"



/** Set SCK to max rate of F_CPU/2. See Sd2Card::setSckRate(). */
uint8_t const SPI_FULL_SPEED = 0;
/** Set SCK rate to F_CPU/4. See Sd2Card::setSckRate(). */
uint8_t const SPI_HALF_SPEED = 1;
/** Set SCK rate to F_CPU/8. Sd2Card::setSckRate(). */
uint8_t const SPI_QUARTER_SPEED = 2;



// SD card commands
/** GO_IDLE_STATE - init card in spi mode if CS low */
uint8_t const CMD0 = 0X00;
/** SEND_IF_COND - verify SD Memory Card interface operating condition.*/
uint8_t const CMD8 = 0X08;
/** SEND_CSD - read the Card Specific Data (CSD register) */
uint8_t const CMD9 = 0X09;
/** SEND_CID - read the card identification information (CID register) */
uint8_t const CMD10 = 0X0A;
/** SEND_STATUS - read the card status register */
uint8_t const CMD13 = 0X0D;
/** READ_BLOCK - read a single data block from the card */
uint8_t const CMD17 = 0X11;
/** WRITE_BLOCK - write a single data block to the card */
uint8_t const CMD24 = 0X18;
/** WRITE_MULTIPLE_BLOCK - write blocks of data until a STOP_TRANSMISSION */
uint8_t const CMD25 = 0X19;
/** ERASE_WR_BLK_START - sets the address of the first block to be erased */
uint8_t const CMD32 = 0X20;
/** ERASE_WR_BLK_END - sets the address of the last block of the continuous
	 range to be erased*/
uint8_t const CMD33 = 0X21;
/** ERASE - erase all previously selected blocks */
uint8_t const CMD38 = 0X26;
/** APP_CMD - escape for application specific command */
uint8_t const CMD55 = 0X37;
/** READ_OCR - read the OCR register of a card */
uint8_t const CMD58 = 0X3A;
/** SET_WR_BLK_ERASE_COUNT - Set the number of write blocks to be
	  pre-erased before writing */
uint8_t const ACMD23 = 0X17;
/** SD_SEND_OP_COMD - Sends host capacity support information and
	 activates the card's initialization process */
uint8_t const ACMD41 = 0X29;
//------------------------------------------------------------------------------
/** status for card in the ready state */
uint8_t const R1_READY_STATE = 0X00;
/** status for card in the idle state */
uint8_t const R1_IDLE_STATE = 0X01;
/** status bit for illegal command */
uint8_t const R1_ILLEGAL_COMMAND = 0X04;
/** start data token for read or write single block*/
uint8_t const DATA_START_BLOCK = 0XFE;
/** stop token for write multiple blocks*/
uint8_t const STOP_TRAN_TOKEN = 0XFD;
/** start data token for write multiple blocks*/
uint8_t const WRITE_MULTIPLE_TOKEN = 0XFC;
/** mask for data response tokens after a write block operation */
uint8_t const DATA_RES_MASK = 0X1F;
/** write data accepted token */
uint8_t const DATA_RES_ACCEPTED = 0X05;



// Two Wire (aka I2C) ports
uint8_t const SDA_PIN = 20;
uint8_t const SCL_PIN = 21;

// SPI port
uint8_t const SS_PIN = 53;
uint8_t const MOSI_PIN = 51;
uint8_t const MISO_PIN = 50;
uint8_t const SCK_PIN = 52;




// define software SPI pins so Mega can use unmodified GPS Shield
/** SPI chip select pin */
uint8_t const SD_CHIP_SELECT_PIN = 53;
/** SPI Master Out Slave In pin */
uint8_t const SPI_MOSI_PIN = 51;
/** SPI Master In Slave Out pin */
uint8_t const SPI_MISO_PIN = 50;
/** SPI Clock pin */
uint8_t const SPI_SCK_PIN = 52;


/** init timeout ms */
#warning TIMEOUT CAHNGED
uint16_t const SD_INIT_TIMEOUT = 2000;
/** erase timeout ms */
uint16_t const SD_ERASE_TIMEOUT = 10000;
/** read timeout ms */
uint16_t const SD_READ_TIMEOUT = 300;
/** write time out ms */
uint16_t const SD_WRITE_TIMEOUT = 600;

/** timeout error for command CMD0 */
uint8_t const SD_CARD_ERROR_CMD0 = 0X1;
/** CMD8 was not accepted - not a valid SD card*/
uint8_t const SD_CARD_ERROR_CMD8 = 0X2;
/** card returned an error response for CMD17 (read block) */
uint8_t const SD_CARD_ERROR_CMD17 = 0X3;
/** card returned an error response for CMD24 (write block) */
uint8_t const SD_CARD_ERROR_CMD24 = 0X4;
/**  WRITE_MULTIPLE_BLOCKS command failed */
uint8_t const SD_CARD_ERROR_CMD25 = 0X05;
/** card returned an error response for CMD58 (read OCR) */
uint8_t const SD_CARD_ERROR_CMD58 = 0X06;
/** SET_WR_BLK_ERASE_COUNT failed */
uint8_t const SD_CARD_ERROR_ACMD23 = 0X07;
/** card's ACMD41 initialization process timeout */
uint8_t const SD_CARD_ERROR_ACMD41 = 0X08;
/** card returned a bad CSR version field */
uint8_t const SD_CARD_ERROR_BAD_CSD = 0X09;
/** erase block group command failed */
uint8_t const SD_CARD_ERROR_ERASE = 0X0A;
/** card not capable of single block erase */
uint8_t const SD_CARD_ERROR_ERASE_SINGLE_BLOCK = 0X0B;
/** Erase sequence timed out */
uint8_t const SD_CARD_ERROR_ERASE_TIMEOUT = 0X0C;
/** card returned an error token instead of read data */
uint8_t const SD_CARD_ERROR_READ = 0X0D;
/** read CID or CSD failed */
uint8_t const SD_CARD_ERROR_READ_REG = 0X0E;
/** timeout while waiting for start of read data */
uint8_t const SD_CARD_ERROR_READ_TIMEOUT = 0X0F;
/** card did not accept STOP_TRAN_TOKEN */
uint8_t const SD_CARD_ERROR_STOP_TRAN = 0X10;
/** card returned an error token as a response to a write operation */
uint8_t const SD_CARD_ERROR_WRITE = 0X11;
/** attempt to write protected block zero */
uint8_t const SD_CARD_ERROR_WRITE_BLOCK_ZERO = 0X12;
/** card did not go ready for a multiple block write */
uint8_t const SD_CARD_ERROR_WRITE_MULTIPLE = 0X13;
/** card returned an error to a CMD13 status check after a write */
uint8_t const SD_CARD_ERROR_WRITE_PROGRAMMING = 0X14;
/** timeout occurred during write programming */
uint8_t const SD_CARD_ERROR_WRITE_TIMEOUT = 0X15;
/** incorrect rate selected */
uint8_t const SD_CARD_ERROR_SCK_RATE = 0X16;


// card types
/** Standard capacity V1 SD card */
uint8_t const SD_CARD_TYPE_SD1 = 1;
/** Standard capacity V2 SD card */
uint8_t const SD_CARD_TYPE_SD2 = 2;
/** High Capacity SD card */
uint8_t const SD_CARD_TYPE_SDHC = 3;



class SDRawBlockDeviceAccessor : public AbstractRawBlockDevice
{
	public:
		SDRawBlockDeviceAccessor();

		ubyte1 init();
		ubyte1 getByte(ubyte8& pos);
		void setByte(int& pos, byte& val);


		// sd ops

		void spiSend(ubyte1 b); // send a byte to the card
		ubyte1 spiRec(); // receive a byte from the card
		void chipSelectHigh();
		void chipSelectLow();
		ubyte1 _init(ubyte1 sckRateID, ubyte1 chipSelectPin);
		ubyte1 cardCommand(ubyte1 cmd, ubyte4 arg);
		ubyte1 setSckRate(ubyte1 sckRateID);
		void readEnd();
		ubyte1 waitNotBusy(ubyte2 timeoutMillis);
		uint8_t readData(uint32_t block,
										  uint16_t offset, uint16_t count, uint8_t* dst);
		uint8_t waitStartBlock();

		// end of sd ops

		uint32_t block_;
		uint8_t chipSelectPin_;
		uint8_t errorCode_;
		uint8_t inBlock_;
		uint16_t offset_;
		uint8_t partialBlockRead_;
		uint8_t status_;
		uint8_t type_;



		/** Return the card type: SD V1, SD V2 or SDHC */
		uint8_t type(void) const {return type_;}
		void type(uint8_t value) {type_ = value;}
		uint8_t cardAcmd(uint8_t cmd, uint32_t arg)
		{
			 cardCommand(CMD55, 0);
			 return cardCommand(cmd, arg);
		}




		// wrong: this class should not cache as we then must cache the full 256 bytes and this is not good..
		/*byte cache[16]; */
		/* data stored which was read from sd card (WARNING: sd cards
		 * standard mode send 256 bytes, so this class ignores
		 * 240 bytes. We recommend to use AdxFS as it is optimized with
		 * 16 byte reading */
};
