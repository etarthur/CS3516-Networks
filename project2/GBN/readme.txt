To compile simply enter:
	make all
in a linux terminal, then to run enter:
	./GBNproject2
and following the onscreen prompts enter parameter values for the simulation. 

If on windows, change the OS in the header file to work properly.

IF IT DOESN'T COMPILE: 
-make sure have the right OS selected in the header
-I had to make one change to the project2.c file, where I use <sys/time.h> as opposed to <time.h>, so if
that doesn't work simply remove <sys/time.h>

This implementation doesn't work 100%, by that I mean it will always deliver the correct packets but it may
take a lot of packets only to send a few, this is an issue with my timers which I believe aren't correctly setup, but
it works enough that it can get by. It actually seems to behave better with out of order packets turned on.

BUFFER:
The buffer is intialized as null structs, so every acknum is set to -1 as a way of determing if an element
has been intialized or not because when a packet is put it in its acknum is 0. This is a way to get around 
creating a buffer struct which I probably should have done instead.