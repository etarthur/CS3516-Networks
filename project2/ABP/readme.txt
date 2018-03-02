To run first compile using the makefile by simply typing:
	make all
Then run the program by doing 
./ABPproject2
and a series of questions will pop-up. Simply enter the response to each question hit enter after each, and the 
program will run. NOTE: Bi-directional communication was not implemented, always keep off. The time between messages
from layer 5 should be above 500, otherwise it will enter the temporary buffer, which can only hold 100 messages then it
breaks, this should never happen!

IF IT DOESN'T COMPILE: 
-make sure have the right OS selected in the header
-I had to make one change to the project2.c file, where I use <sys/time.h> as opposed to <time.h>, so if
that doesn't work simply remove <sys/time.h>

Also not sure why but somtimes the first few runs will cause some bad packets, like repeats or something, and once a few runs
have been done, my implementation will work with 100% success, not sure why this happens. So if you get a bad run try 3 or 4 more 
and they will all be good. 

