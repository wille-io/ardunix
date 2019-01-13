/* contains the two main functions that are executed by the microcontroller at startup
 * to support usage on windows machine (testing purpose), this project contains a compatibility
 * header which simulates the avr microcontroller. to activate windows compatibility, undefine
 * 'ARDUINO' and add the corresponding compatibility header to your processor and operating system
 *
 * funfact! you are unable to dev a sd card driver in a ardunix vm. no matter what. not even if
 * you add read/write events to the event controller for the pins. i guarantee it.
*/

#warning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#warning   if a VFI is going to be removed, VFI removes the node name string also,
#warning     which means that there is a chance to delete ArdunixHeader::slashstring for example.
#warning   if slashstring is overwritten, ardunix will (probably) crash hard.. just saying
#warning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include "ardunix.h"
#ifndef ARDUINO
#include "windows_cxx_amd64_to_ardunix_simulator.h"
#endif
#include "Kernel.h"


/*void* ardunix_malloc(size_t size)
{
	void* ptr = malloc(size);
	if (Serial)
	{
		Serial.print("malloc: ");
		Serial.println((int)ptr);
	}
	return ptr;
}
#define malloc(x) ardunix_malloc(x) */



Kernel* kernel; // has to be defined globally, becasue other classes use the kernel, too

void setup() // first called by the microcontroller
{
	kernel = new Kernel;
}

void loop() // executed after setup() and everytime this function returns
{
	//delay(1);

	if (!kernel)
		return;

	kernel->tick(); // like it's an FPS :)
}

#ifndef ARDUINO
int main()
{
   setup();

   while (true)
      loop();
}
#endif
