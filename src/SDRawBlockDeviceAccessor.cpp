#include "SDRawBlockDeviceAccessor.h"
#include "EventController.h"

SDRawBlockDeviceAccessor::SDRawBlockDeviceAccessor() :
	AbstractRawBlockDevice((int)ardunix::filesystem::devicetype::sdcard, ArdunixHeader::sdcard_string)
{
	errorCode_ = 0;
	inBlock_ = 0;
	partialBlockRead_ = 0;
	type_ = 0;
	block_ = 0;
	chipSelectPin_ = 0;
	offset_ = 0;
	status_ = 0;



	 pinMode(10, OUTPUT);     // "change this to 53 on a mega", doc. << bullshit! 10 works perfectly on the mega

	this->_init(SPI_HALF_SPEED, 4); // SD_CHIP_SELECT_PIN);



	// TEST READ !!

	/*uint8_t* test = (uint8_t*)malloc(20);
	memset(test, 0, 20); // set 0

	this->readData(260, 0, 16, test);

	Serial.print("ICH HABE GELESEN !! ");
	Serial.println((char*)test);*/
}

void SDRawBlockDeviceAccessor::spiSend(uint8_t b)
{
	SPDR = b;
	while (!(SPSR & (1 << SPIF)));
}

ubyte1 SDRawBlockDeviceAccessor::spiRec()
{
	spiSend(0XFF);
	return SPDR;
}

ubyte1 SDRawBlockDeviceAccessor::cardCommand(ubyte1 cmd, ubyte4 arg)
{
	// end read if in partialBlockRead mode
	readEnd();

	// select card
	chipSelectLow();

	// wait up to 300 ms if busy
	waitNotBusy(300);

	// send command
	spiSend(cmd | 0x40);

	// send argument
	for (int8_t s = 24; s >= 0; s -= 8)
		spiSend(arg >> s);

	// send CRC
	uint8_t crc = 0XFF;
	if (cmd == CMD0) crc = 0X95;  // correct crc for CMD0 with arg 0
	if (cmd == CMD8) crc = 0X87;  // correct crc for CMD8 with arg 0X1AA
	spiSend(crc);

	// wait for response
	for (uint8_t i = 0; ((status_ = spiRec()) & 0X80) && i != 0XFF; i++);

	return status_;
}

void SDRawBlockDeviceAccessor::chipSelectHigh(void)
{
	digitalWrite(chipSelectPin_, HIGH);
}

void SDRawBlockDeviceAccessor::chipSelectLow(void)
{
	digitalWrite(chipSelectPin_, LOW);
}

ubyte1 SDRawBlockDeviceAccessor::_init(ubyte1 sckRateID, ubyte1 chipSelectPin)
{
	eventController.fireEvent(ardunix::events::etStdOutFlash, (void*)F(" * SDRBDA: initializing SD Card ... "));


	errorCode_ = inBlock_ = partialBlockRead_ = type_ = 0;
	chipSelectPin_ = chipSelectPin;
	// 16-bit init start time allows over a minute
	uint16_t t0 = (uint16_t)millis();
	uint32_t arg;

	// set pin modes
	pinMode(chipSelectPin_, OUTPUT);
	chipSelectHigh();
	pinMode(SPI_MISO_PIN, INPUT);
	pinMode(SPI_MOSI_PIN, OUTPUT);
	pinMode(SPI_SCK_PIN, OUTPUT);

	// SS must be in output mode even it is not chip select
	pinMode(SS_PIN, OUTPUT);
	digitalWrite(SS_PIN, HIGH); // disable any SPI device using hardware SS pin
	// Enable SPI, Master, clock rate f_osc/128
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
	// clear double speed
	SPSR &= ~(1 << SPI2X);

	// must supply min of 74 clock cycles with CS high.
	for (uint8_t i = 0; i < 10; i++)
		spiSend(0XFF);
	chipSelectLow();

	// command to go idle in SPI mode
	while ((status_ = cardCommand(CMD0, 0)) != R1_IDLE_STATE)
	{
		if (((uint16_t)millis() - t0) > SD_INIT_TIMEOUT)
		{
			Serial.print("SD_CARD_ERROR_CMD0");
			//error(SD_CARD_ERROR_CMD0);
			goto fail;
		}
	}

	// check SD version
	if ((cardCommand(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND))
	{
		type(SD_CARD_TYPE_SD1);
	}
	else
	{
		// only need last byte of r7 response
		for (uint8_t i = 0; i < 4; i++) status_ = spiRec();
		if (status_ != 0XAA)
		{
			Serial.print("SD_CARD_ERROR_CMD8");
			//error(SD_CARD_ERROR_CMD8);
			goto fail;
		}
		type(SD_CARD_TYPE_SD2);
	}
	// initialize card and send host supports SDHC if SD2
	arg = type() == SD_CARD_TYPE_SD2 ? 0X40000000 : 0;

	while ((status_ = cardAcmd(ACMD41, arg)) != R1_READY_STATE)
	{
		// check for timeout
		if (((uint16_t)millis() - t0) > SD_INIT_TIMEOUT)
		{
			Serial.println("SD_CARD_ERROR_ACMD41");
			//error(SD_CARD_ERROR_ACMD41);
			goto fail;
		}
	}

	// if SD2 read OCR register to check for SDHC card
	if (type() == SD_CARD_TYPE_SD2) {
		if (cardCommand(CMD58, 0))
		{
			Serial.print("SD_CARD_ERROR_CMD58");
			//error(SD_CARD_ERROR_CMD58);
			goto fail;
		}
		if ((spiRec() & 0XC0) == 0XC0) type(SD_CARD_TYPE_SDHC);
		// discard rest of ocr - contains allowed voltage range
		for (uint8_t i = 0; i < 3; i++) spiRec();
	}
	chipSelectHigh();

	eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("[done]"));

	return setSckRate(sckRateID);

fail:
	chipSelectHigh();
	eventController.fireEvent(ardunix::events::etStdOutFlashLine, (void*)F("[error!]"));
	return false;
}

ubyte1 SDRawBlockDeviceAccessor::setSckRate(ubyte1 sckRateID)
{
	if (sckRateID > 6) {
		//error(SD_CARD_ERROR_SCK_RATE);
		return false;
	}

	// see avr processor datasheet for SPI register bit definitions
	if ((sckRateID & 1) || sckRateID == 6) {
		SPSR &= ~(1 << SPI2X);
	} else {
		SPSR |= (1 << SPI2X);
	}
	SPCR &= ~((1 <<SPR1) | (1 << SPR0));
	SPCR |= (sckRateID & 4 ? (1 << SPR1) : 0)
			  | (sckRateID & 2 ? (1 << SPR0) : 0);
	return true;
}

void SDRawBlockDeviceAccessor::readEnd()
{
	if (inBlock_)
	{
		// skip data and crc
		// optimize skip for hardware
		SPDR = 0XFF;
		while (offset_++ < 513) {
			while (!(SPSR & (1 << SPIF)));
			SPDR = 0XFF;
		}
		// wait for last crc byte

		while (!(SPSR & (1 << SPIF)));

		chipSelectHigh();
		inBlock_ = 0;
	}
}

ubyte1 SDRawBlockDeviceAccessor::waitNotBusy(ubyte2 timeoutMillis)
{
	ubyte2 t0 = millis();

	do
	{
		if (spiRec() == 0XFF) return true;
	}
	while (((ubyte2)millis() - t0) < timeoutMillis);

	return false;
}

uint8_t SDRawBlockDeviceAccessor::readData(uint32_t block,
														 uint16_t offset, uint16_t count, uint8_t* dst)
{
	uint16_t n;
	if (count == 0) return true;
	if ((count + offset) > 512)
	{
		goto fail;
	}

	if (!inBlock_ || block != block_ || offset < offset_) {
		block_ = block;
		// use address if not SDHC card
		if (type()!= SD_CARD_TYPE_SDHC) block <<= 9;
		if (cardCommand(CMD17, block)) {
			Serial.println("SD_CARD_ERROR_CMD17");
			//error(SD_CARD_ERROR_CMD17);
			goto fail;
		}
		if (!waitStartBlock()) {
			goto fail;
		}
		offset_ = 0;
		inBlock_ = 1;
	}

	// start first spi transfer
	SPDR = 0XFF;

	// skip data before offset
	for (;offset_ < offset; offset_++) {
		while (!(SPSR & (1 << SPIF)));
		SPDR = 0XFF;
	}
	// transfer data
	n = count - 1;
	for (uint16_t i = 0; i < n; i++) {
		while (!(SPSR & (1 << SPIF)));
		dst[i] = SPDR;
		SPDR = 0XFF;
	}
	// wait for last byte
	while (!(SPSR & (1 << SPIF)));
	dst[n] = SPDR;

	offset_ += count;
	if (!partialBlockRead_ || offset_ >= 512) {
		// read rest of data, checksum and set chip select high
		readEnd();
	}
	return true;

fail:
	chipSelectHigh();
	return false;
}

uint8_t SDRawBlockDeviceAccessor::waitStartBlock()
{
	uint16_t t0 = millis();

	while ((status_ = spiRec()) == 0XFF)
	{
		if (((uint16_t)millis() - t0) > SD_READ_TIMEOUT)
		{
			Serial.println("SD_CARD_ERROR_READ_TIMEOUT");
			//error(SD_CARD_ERROR_READ_TIMEOUT);
			goto fail;
		}
	}

	if (status_ != DATA_START_BLOCK)
	{
		Serial.println("SD_CARD_ERROR_READ");
		//error(SD_CARD_ERROR_READ);
		goto fail;
	}
	return true;

fail:
	chipSelectHigh();
	return false;
}












ubyte1 SDRawBlockDeviceAccessor::init()
{
	return 0; // not omplemented yet
}

ubyte1 SDRawBlockDeviceAccessor::getByte(ubyte8& pos) // pos = 263.680
{
	// TODO: replace 512 with devBlockSize
	ubyte8 rest = pos % 512;
	ubyte8 block = (pos - rest) / 512;

	//Serial.print("Block: "); Serial.println((long)block, 16);
	//Serial.print("Rest: "); Serial.println((long)rest, 16);

	ubyte1 cache = 'S'; // as testing "invalid value"
	this->readData((uint32_t)block, (uint16_t)rest, 1, (uint8_t*)&cache); // read from 'block', 'offset' + 40 bytes, read only 1 byte, read to 'cache'

	return cache;
}

void SDRawBlockDeviceAccessor::setByte(int& pos, byte& val)
{
	return;
}
