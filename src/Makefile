# ardunix ftw (my really first makefile)
# WARNING! DO NOT OPEN THIS FILE IN QTCREATOR OR ONLY IN READ-ONLY MODE !! As qtcreator CAN change text formatation, which makes make go totally apeshit

CXX=avr-g++
CC=avr-gcc
MCU=atmega2560
MCU_DIR=mega
# xMCU=atmega328p
# xMCU_DIR=standard
# ARDUNIX_SDK_PATH=C:/dev/ardunix_sdk/
# ARDUINO_DEV_PATH=C:/dev/Arduino/
ARDUNIX_SDK_PATH=../adxsdk/
ARDUINO_DEV_PATH=/usr/share/arduino/
INCLUDES=-I$(ARDUINO_DEV_PATH)hardware/arduino/cores/arduino -I$(ARDUINO_DEV_PATH)hardware/arduino/variants/$(MCU_DIR) $(EEPROM) -I$(ARDUNIX_SDK_PATH)/include
ARDUINO_PATH=$(ARDUINO_DEV_PATH)hardware/arduino/cores/arduino/
BIN_PATH=../bin/
#CFLAGS=-c -Wall -Os -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=16000000L -DUSB_VID=null -DUSB_PID=null -DARDUINO=106
CFLAGS=-c -Wall -O2 -ffunction-sections -fdata-sections -mmcu=$(MCU) -DF_CPU=16000000L -DUSB_VID=null -DUSB_PID=null -DARDUINO=106
CXXFLAGS=-fno-exceptions $(CFLAGS) 
EEPROM=-I$(ARDUINO_DEV_PATH)/libraries/EEPROM/
# -IC:/dev/Arduino/libraries/EEPROM/
# -I$(ARDUINO_DEV_PATH)/libraries/EEPROM/

all: arduino_deps
	@avr-ar rcs $(BIN_PATH)core.a $(BIN_PATH)malloc.o $(BIN_PATH)realloc.o $(BIN_PATH)WInterrupts.o $(BIN_PATH)wiring.o $(BIN_PATH)wiring_analog.o $(BIN_PATH)wiring_digital.o $(BIN_PATH)wiring_pulse.o $(BIN_PATH)wiring_shift.o $(BIN_PATH)CDC.o $(BIN_PATH)HardwareSerial.o $(BIN_PATH)HID.o $(BIN_PATH)IPAddress.o $(BIN_PATH)main.o $(BIN_PATH)new.o $(BIN_PATH)Print.o $(BIN_PATH)Stream.o $(BIN_PATH)Tone.o $(BIN_PATH)USBCore.o $(BIN_PATH)WMath.o $(BIN_PATH)WString.o
	
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)AbstractEventClass.o ./AbstractEventClass.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)AbstractFileSystemAccessor.o ./AbstractFileSystemAccessor.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)AbstractPartitionAccessor.o ./AbstractPartitionAccessor.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)AbstractRawBlockDevice.o ./AbstractRawBlockDevice.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)AbstractRawCharDevice.o ./AbstractRawCharDevice.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)AbstractRawDevice.o ./AbstractRawDevice.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)API.o ./API.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)ArdunixHeader.o ./ArdunixHeader.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)ArdunixFileSystemAccessor.o ./ArdunixFileSystemAccessor.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)Console.o ./Console.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)EEPROMRawBlockDeviceDriver.o ./EEPROMRawBlockDeviceDriver.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)SDRawBlockDeviceAccessor.o ./SDRawBlockDeviceAccessor.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)ELFile.o ./ELFile.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)Event.o ./Event.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)EventController.o ./EventController.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)EventList.o ./EventList.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)FAT16FileSystemAccessor.o ./FAT16FileSystemAccessor.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)File.o ./File.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)FileSystemManager.o ./FileSystemManager.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)InternalFunctions.o ./InternalFunctions.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)Interpreter.o ./Interpreter.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)Kernel.o ./Kernel.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)OpcodeManager.o ./OpcodeManager.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)RawBlockDeviceAccessor.o ./RawBlockDeviceAccessor.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)RawCharDeviceAccessor.o ./RawCharDeviceAccessor.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)RawDeviceAccessor.o ./RawDeviceAccessor.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)UARTRawCharDeviceDriver.o ./UARTRawCharDeviceDriver.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)VFI.o ./VFI.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)VFIManager.o ./VFIManager.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)VirtualMachine.o ./VirtualMachine.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)VMManager.o ./VMManager.cpp
	
	@avr-ar rcs $(BIN_PATH)ardunix.a ../bin/AbstractEventClass.o ../bin/AbstractFileSystemAccessor.o ../bin/AbstractPartitionAccessor.o ../bin/AbstractRawBlockDevice.o ../bin/AbstractRawCharDevice.o ../bin/AbstractRawDevice.o ../bin/API.o ../bin/ardunix.o ../bin/ArdunixHeader.o ../bin/ArdunixFileSystemAccessor.o ../bin/CDC.o ../bin/Console.o ../bin/EEPROMRawBlockDeviceDriver.o ../bin/SDRawBlockDeviceAccessor.o ../bin/ELFile.o ../bin/Event.o ../bin/EventController.o ../bin/EventList.o ../bin/FAT16FileSystemAccessor.o ../bin/File.o ../bin/FileSystemManager.o ../bin/HID.o ../bin/VMManager.o ../bin/InternalFunctions.o ../bin/Interpreter.o ../bin/IPAddress.o ../bin/Kernel.o ../bin/main.o ../bin/malloc.o ../bin/new.o ../bin/OpcodeManager.o ../bin/Print.o ../bin/RawBlockDeviceAccessor.o ../bin/RawCharDeviceAccessor.o ../bin/RawDeviceAccessor.o ../bin/realloc.o ../bin/Stream.o ../bin/Tone.o ../bin/UARTRawCharDeviceDriver.o ../bin/USBCore.o ../bin/VFI.o ../bin/VFIManager.o ../bin/VirtualMachine.o ../bin/WInterrupts.o ../bin/wiring.o ../bin/wiring_analog.o ../bin/wiring_digital.o ../bin/wiring_pulse.o ../bin/wiring_shift.o ../bin/WMath.o ../bin/WString.o
	
	@avr-gcc -Os -Wl,--gc-sections,--relax -mmcu=$(MCU) -o $(BIN_PATH)ardunix.elf $(BIN_PATH)ardunix.o $(BIN_PATH)ardunix.a $(BIN_PATH)EEPROM.o $(BIN_PATH)core.a -L$(BIN_PATH) -lm
	
	@avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 $(BIN_PATH)ardunix.elf $(BIN_PATH)ardunix.eep
 
	@avr-objcopy -O ihex -R .eeprom $(BIN_PATH)ardunix.elf $(BIN_PATH)ardunix.hex
	
	
clean:
	rm $(BIN_PATH)/*.*
	
	#	install: all 	- 	but without is faster !
install: 
	#	arduino uno:  no --relax (--gc,sections,--relax)    -cwiring = -carduino
	# @avrdude -C$(ARDUINO_DEV_PATH)/hardware/tools/avr/etc/avrdude.conf -v -p$(MCU) -cwiring -P//./COM4 -b115200 -D -Uflash:w:$(BIN_PATH)ardunix.hex:i
	@avrdude -v -p$(MCU) -cwiring -P/dev/ttyACM3 -b115200 -D -Uflash:w:$(BIN_PATH)ardunix.hex:i
	
	
run: install
	echo "k"
	
	
arduino_deps:
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)ardunix.o ./ardunix.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)EEPROM.o $(ARDUINO_DEV_PATH)libraries/EEPROM/EEPROM.cpp
		
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_PATH)malloc.o $(ARDUINO_PATH)avr-libc/malloc.c
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_PATH)realloc.o $(ARDUINO_PATH)avr-libc/realloc.c
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_PATH)WInterrupts.o $(ARDUINO_PATH)WInterrupts.c
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_PATH)wiring.o $(ARDUINO_PATH)wiring.c
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_PATH)wiring_analog.o $(ARDUINO_PATH)wiring_analog.c
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_PATH)wiring_digital.o $(ARDUINO_PATH)wiring_digital.c
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_PATH)wiring_pulse.o $(ARDUINO_PATH)wiring_pulse.c
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN_PATH)wiring_shift.o $(ARDUINO_PATH)wiring_shift.c
	
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)CDC.o $(ARDUINO_PATH)CDC.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)HardwareSerial.o $(ARDUINO_PATH)HardwareSerial.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)HID.o $(ARDUINO_PATH)HID.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)IPAddress.o $(ARDUINO_PATH)IPAddress.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)main.o $(ARDUINO_PATH)main.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)new.o $(ARDUINO_PATH)new.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)Print.o $(ARDUINO_PATH)Print.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)Stream.o $(ARDUINO_PATH)Stream.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)Tone.o $(ARDUINO_PATH)Tone.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)USBCore.o $(ARDUINO_PATH)USBCore.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)WMath.o $(ARDUINO_PATH)WMath.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN_PATH)WString.o $(ARDUINO_PATH)WString.cpp
