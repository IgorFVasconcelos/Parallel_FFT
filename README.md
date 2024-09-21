### DESCRIPTION
An MPI FFT-3D implementation in C via the 3d transpose algorithm.
We take data from the file raw.txt with size n, which we supose to be int values and distribute it by
a 3-dimensional array with dimensions cbrt(n).
Each process receives a portion of the data the size of a dimension and execute the classic cooley-turkey fft.
Then we transpose all the data using all-to-all communication first in the horizontal axis, then in the vertical axis.
Finally we gather all the data in proccess 0 and print it.
In future versions would be ideal to put the info in a file.

### REQUIREMENTS
Only the MPI library.

For Linux run:

sudo apt-get install mpich

sudo apt-get install openmpi-bin libopenmpi-dev


For Windows:

Compile through WSL.

### COMPILING
I already left a Makefile prepared. So you just need to run the following in the project root folder:

make

mpicc -np <number of processors> ./main/main_program

The size of the signal is proportional to the size of the entry. The dimension is sqrt(p).

Example: if you want to run a 4096 size signal you'll compile like

mpicc -np 256 ./main/main_program

if needed use the --oversubscribe flag

mpicc --oversubscribe -np 256 ./main/main_program

### OPEN ISSUES
- Make the program accept any power of 2 input despite the number of available processes

- Make the size of input be a parameter of the compilation OR make it do the zero padding automatically 
