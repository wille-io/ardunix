/*
#ifndef WINDOWS_CXX_AMD64_TO_ARDUNIX_SIMULATOR_H
#define WINDOWS_CXX_AMD64_TO_ARDUNIX_SIMULATOR_H

//#include "ardunix.h"
#include <string.h>
#include <string>
#include <iostream>
#include <malloc.h>

typedef unsigned char byte;

#define BUS_SIZED_PTR unsigned long long

class _EEPROM
{
public:
   byte read(byte& pos)
   {
      return 0;
   }
};

class _Serial
{
public:
    void begin(int& x){}

    void print(int& x)
    {
        std::cerr << x;
    }

    void print(char* x)
    {
        std::cerr << x;
    }

    void println(char* x)
    {
        print(x);
        print("\r\n");
    }

    void println(int x)
    {
        print(x);
        print("\r\n");
    }

    void end(){}

    byte read()
    {
        //char c;
        //std::cin.get(c);
        //return c;

       char c;
       std::cin >> c;
       return c;
    }

    void write(char c)
    {
        std::cerr << c;
    }

    byte available()
    {
        //std::streamsize size = std::cin.rdbuf()->in_avail();
        //std::cerr << "size: " << size << std::endl;
        //return size;
       return 0;
    }
};

//typedef bool boolean;
typedef unsigned char byte;
typedef byte uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;
typedef std::string String;
typedef char* __FlashStringHelper;

_Serial Serial;
_EEPROM EEPROM;

char* F(char* x)
{
    return x;
}

char* PSTR(char* x)
{
    return x;
}

int strcmp_P(char* x, char* y)
{
    return strcmp(x, y);
}

void delay(int& x)
{
   //Sleep(x);
}

#endif // WINDOWS_CXX_AMD64_TO_ARDUNIX_SIMULATOR_H
*/
