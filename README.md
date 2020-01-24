![ardunix](gfx/adx.png)
# Ardunix
So, what is Ardunix?
This is yet another operating system for the Arduino - but it can run binaries from any device, without having to flash it!
It turnt into a school project in 2012, so very much time could be invested by 4 people. But this project ended up being graded with an *F*.
The teachers said they didn't understand anything we've done. But anyways, it was a working operating system that could run (emulate) binaries from any device you want, without having to flash new software on it.
I always wanted to have this os ready for a release, but I never got it into a shape that I was daring to publish. Now, it's a mess and still getting published to GitHub - 8 years later.

### adxfs
This source code contains a filesystem ("Ardunix filesystem", short: "adxfs"), specially made for microcontrollers with extremely few memory. An os is rather hungry and leaves few bytes free to read an sd card - which sends a minimum of 256 bytes of data on request. So to have this working the Ardunix filesystem puts only 16 bytes into 256 byte blocks. So reading files does not end up needing 256 free bytes to write into memory, but just 16 bytes and 240 bytes right into the void.

### Important
This is terribly written C++ (it was 8 years ago) and as easily readable and understandable as possible (keep in mind, this was a school project). 
I don't even know if this is the most recent source code, as back then there was no free private GitHub repos and I wrote most of the code anyway.
Ardunix might not run or not even compile, but you could give it a try. Compile & execute at your *OWN RISK*. 
Thanks for reading.
