# Lead-Cloud

Cloud-based LED Matrix Solution.

Will consist of a remote system to update local raspberry systems on what to display. Will have a number of tasks (i.e. Visual jobs) running at the same time, taking turns displaying.

## Project structure

### Job Scheduler

Calls job display update function.
Swaps to jobs.

### Visual job
Representation of job displaying to the matrix.

### Utils
Useful implementations.

### LedDriver
Led Driver Daemon and device driver implementation.

Currently has only the behaviour to drive the matrix, doesn't implement the device driver nor the daemon atm.

## Project Compilation

Compile LedDriver directory with 
	gppCompiler -g -o driveMatrix main.cpp gpio.cpp MatrixDriver.cpp Drawer.cpp -Wall -Wextra

## Project Use

Currently only the Led driver part works.

Compile it and run the program in a Raspberry Pi 4.





