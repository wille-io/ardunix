#ifndef ARDUNIXHEADER_H
#define ARDUNIXHEADER_H

// helper variables, which have to be statically and globally defined, becaude many classes uses these varibales

class ArdunixHeader
{
	public:
		static const char  nullchar;
		static const char  slashchar;
		static const char* slashstring;
		static const char* spacestring;
		static const char* delimiter;

		static const char* uart_string;
		static const char* eeprom_string;
		static const char* sdcard_string;
};

#endif // ARDUNIXHEADER_H
