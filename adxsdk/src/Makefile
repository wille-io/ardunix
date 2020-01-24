# compile ardunix sdk
# WARNING! DO NOT OPEN THIS FILE IN QTCREATOR OR ONLY IN READ-ONLY MODE !! As qtcreator CAN change text formatation, which makes make go totally nuts

CXX=avr-g++
CC=avr-gcc
OC=avr-objcopy
OD=avr-objdump
MCU=atmega328p
MCU_DIR=mega
# xMCU=atmega328p
# xMCU_DIR=standard
CFLAGS=-mmcu=$(MCU) -DF_CPU=16000000L
BIN_PATH=../lib/

all: 
	@$(CC) -c -mmcu=avr4 adxApi.c -o $(BIN_PATH)libArdunixApi.o
	
clean:
	rm $(BIN_PATH)/*.*
