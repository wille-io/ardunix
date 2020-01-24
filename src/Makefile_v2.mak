# ardunix ftw
# WARNING! DO NOT OPEN THIS FILE IN QTCREATOR OR ONLY IN READ-ONLY MODE !! As qtcreator CAN change text formatation, which makes make go totally nuts
ARDUINO_DEV_PATH=C:/dev/Arduino/
ARDUINO_PATH=$(ARDUINO_DEV_PATH)hardware/arduino/cores/arduino/
CXX=avr-g++
CC=avr-gcc
AR=avr-ar
OBJCPY=avr-objcopy
MCU=atmega2560
MCU_DIR=mega
# xMCU=atmega328p
# xMCU_DIR=standard
BIN_PATH=../bin/
LIB_PATH=../lib/
INC_PATH=../inc/
CXXFLAGS=-fno-exceptions -c -Wall -Os -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=16000000L -DUSB_VID=null -DUSB_PID=null -DARDUINO=106

# list of libraries and their includes
LIB_ARDUINO=$(LIB_PATH)arduino.a
LIB_EEPROM=$(LIB_PATH)EEPROM.o
INC_ARDUINO=-I$(ARDUINO_DEV_PATH)hardware/arduino/cores/arduino -I$(ARDUINO_DEV_PATH)hardware/arduino/variants/$(MCU_DIR)
INC_EEPROM=-IC:/dev/Arduino/libraries/EEPROM/

# list of libraries and includes to include and compiler
LIBRARIES=$(LIB_ARDUINO) $(LIB_EEPROM)
INCLUDES=$(INC_ARDUINO) $(INC_EEPROM)

# list of modules
MOD_EEPROM=EEPROMRawBlockDeviceDriver.cpp
MOD_ARDUNIX_BASE=AbstractEventClass.cpp AbstractFileSystemAccessor.cpp AbstractPartitionAccessor.cpp AbstractRawBlockDevice.cpp AbstractRawCharDevice.cpp AbstractRawDevice.cpp API.cpp ArdunixHeader.cpp ArdunixFileSystemAccessor.cpp Console.cpp SDRawBlockDeviceAccessor.cpp ELFile.cpp Event.cpp EventController.cpp EventList.cpp FAT16FileSystemAccessor.cpp File.cpp FileSystemManager.cpp InternalFunctions.cpp Interpreter.cpp Kernel.cpp OpcodeManager.cpp RawBlockDeviceAccessor.cpp RawCharDeviceAccessor.cpp RawDeviceAccessor.cpp UARTRawCharDeviceDriver.cpp VFI.cpp VFIManager.cpp VirtualMachine.cpp

# list of modules to include and compile (ardunix.cpp must be the first object to compile)
MODULES=ardunix.cpp $(MOD_ARDUNIX_BASE) $(MOD_EEPROM)
OBJECTS=$(MODULES:%.cpp=$(BIN_PATH)%.o)


all: $(BIN_PATH)ardunix 

clean:
	@rm $(BIN_PATH)* -r

#	arduino uno:  no --relax (--gc,sections,--relax)    -cwiring = -carduino
install: $(BIN_PATH)ardunix	
	@avrdude -C$(ARDUINO_DEV_PATH)/hardware/tools/avr/etc/avrdude.conf -v -p$(MCU) -cwiring -P//./COM4 -b115200 -D -Uflash:w:$(BIN_PATH)ardunix.hex:i
	
# for Qt Creator
run: install

# -lc x2 = bad workaround
$(BIN_PATH)ardunix: $(OBJECTS)
	@$(AR) rcs $(BIN_PATH)ardunix.a $(OBJECTS)
	@$(CXX) -Os -Wl,--gc-sections,--relax -mmcu=$(MCU) -o $(BIN_PATH)ardunix.elf $(BIN_PATH)ardunix.o $(BIN_PATH)ardunix.a $(LIBRARIES) -lc -lm -lc
	@$(OBJCPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(BIN_PATH)ardunix.elf $(BIN_PATH)ardunix.eep
	@$(OBJCPY) -O ihex -R .eeprom $(BIN_PATH)ardunix.elf $(BIN_PATH)ardunix.hex

$(OBJECTS): $(BIN_PATH)%.o : %.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $<