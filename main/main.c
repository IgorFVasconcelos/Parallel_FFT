#include <stdio.h>
#include <stdlib.h>
#include "../headers/fft.h"
#include <mpi.h>
#include <math.h>

void memset_0(int *buffer, size_t size){
    for(int i = 0; i < size; i++){
        buffer[i] = 0;
    }
    return;
}

unsigned int keepMSB(unsigned int num, unsigned int m){
  return num & (~((1 << m) - 1));
}

unsigned int keepLSB(unsigned int num, unsigned int m){
  return num & ((1 << m) - 1);
}

unsigned int isSimetrical(unsigned int num, unsigned int size){
  unsigned int ms_half = (num >> size/2);
  num = keepLSB(num, size/2);

  if (num == ms_half){
    return 1;
  }
  return 0;
}


int main(int argc, char** argv){
    //MPI Startup
    MPI_Init(&argc, &argv);

    unsigned int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    unsigned int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    unsigned int dimension = sqrt(world_size);

    MPI_Comm row_comunicator[dimension];

    MPI_Comm column_comunicator[dimension];

    unsigned int num_bits = log2(world_size);

    int group_column = keepLSB(rank, num_bits/2); 
    int group_row = (keepMSB(rank, num_bits/2) >> num_bits/2);

    printf("Eu, o processo %d pertenco a linha %d e a coluna %d\n\n", rank, group_row, group_column);

    MPI_Comm_split(MPI_COMM_WORLD, group_column, rank, &column_comunicator[group_column]);

    MPI_Comm_split(MPI_COMM_WORLD, group_row, rank, &row_comunicator[group_row]);

    //#define MPI_OK

    #ifdef MPI_OK
    //Serial Component
    if(rank == 0){
        //Verify if the number of process is a power of 2
        float test;
        test = sqrt(world_size);
        test = log2(test);
        if(test != (int) test){
            printf("Number of process is not a perfect cube");
            exit(-1);
        }

        //File logic
        char buffer[10];
        char* path = "../data/teste.txt";
        FILE* f = fopen(path, "r");

        if(f == NULL){
            printf("File path not found\n\n");
            exit(-1);
        }

        //Copy signal from file to array
        size_t sig_size = pow(world_size, 3);

        int signal[sig_size];
        memset_0(signal, sig_size);

        char* ptr = NULL;

        for(int i = 0; i < sig_size; i++){
            ptr = fgets(buffer, 10, f);

            if(ptr == NULL){
                printf("Erro, linha não encontrada");
                exit(-1);
            }
            signal[i] += atoi(ptr);
        }


    }
    #endif
    

    //OpenMP logic


    //Finally

    MPI_Finalize();

    return 0;
}