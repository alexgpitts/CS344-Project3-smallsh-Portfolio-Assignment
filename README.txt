How to run my program for assignment 3

My program contains 2 files: smallsh.c and smallsh.h
You must have both of these files in the same folder when compiling

1) After you cd into the directory on the os1 server, run the following command:

	gcc --std=gnu99 -o smallsh smallsh.c

	This will compile the program and make a executable file called "smallsh"

2) To run the script, run the following command in bash

	./p3testscript 2>&1

	This will start the proccess of the shell script which will run all the tests on my smallsh

3) to run the shell by its self, run the following command in bash

	./smallsh
	
	This will start smallsh. Only way to purposefully exit is to type "exit" into prompt. 