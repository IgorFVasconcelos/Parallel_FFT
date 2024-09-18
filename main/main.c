#include <stdio.h>
#include <stdlib.h>
#include "../headers/fft.h"
#include "../headers/bitops.h"
#include <mpi.h>
#include <math.h>

//#define DEBUG

int main(int argc, char** argv){

    //MPI Startup
    MPI_Init(&argc, &argv);
    unsigned int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    unsigned int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    unsigned int dimension = sqrt(world_size);

    //Definindo os buffers
    size_t sig_size = pow(sqrt(world_size), 3);
    int signal_start_buffer[dimension];
    cd signal_proccessing_buffer[dimension];
    cd recv_buffer[dimension];
    //Buffers finais
    int signal[sig_size];
    cd transform_signal[sig_size];

    //Definindo os comunicadores de linhas e colunas
    MPI_Comm row_comunicator[dimension];
    MPI_Comm column_comunicator[dimension];

    unsigned int num_bits = log2(world_size);

    unsigned int group_column = keepLSB(rank, num_bits/2); 
    unsigned int group_row = (keepMSB(rank, num_bits/2) >> num_bits/2);

    //printf("Eu, o processo %u pertenco a linha %u e a coluna %u\n\n", rank, group_row, group_column);

    MPI_Comm_split(MPI_COMM_WORLD, group_column, rank, &column_comunicator[group_column]);

    MPI_Comm_split(MPI_COMM_WORLD, group_row, rank, &row_comunicator[group_row]);

    //Serial Component
    if(rank == 0){
        //Verify if the number of process is a power of 2
        float test;
        test = sqrt(world_size);
        test = log2(test);
        if(test != (int) test){
            printf("Number of process is not a power of 2");
            exit(-1);
        }

        //File logic
        //Suposing a signal of ints
        char buffer[16];
        char* path = "./data/raw.txt";
        FILE* f = fopen(path, "r");

        if(f == NULL){
            printf("File path not found\n\n");
            exit(-1);
        }

        //Copy signal from file to array

        memset_0(signal, sig_size);

        char* ptr = NULL;
        char zero = '0';

        for(int i = 0; i < sig_size; i++){
            ptr = fgets(buffer, 10, f);

            if(ptr == NULL){
                printf("Error, line not found");
                ptr = &zero;
            }
            signal[i] += atoi(ptr);
        }
        fclose(f);

        //Send the appropriate data for each proccess
        for (int dest = 1; dest < world_size; dest++){
          int start = dest*dimension;

          MPI_Send(&signal[start], dimension, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }

        for (int i = 0; i < dimension; i++){
          signal_proccessing_buffer[i] = (complex)signal[i];
        }

    }
    if (rank != 0){
      MPI_Recv(signal_start_buffer, dimension, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      
      for(int i = 0; i < dimension; i++){
        signal_proccessing_buffer[i] = (complex)signal_start_buffer[i];
    }

    }
    fft(signal_proccessing_buffer, (size_t) dimension);

    //Shift rows
    MPI_Alltoall(signal_proccessing_buffer, 1, MPI_DOUBLE_COMPLEX, recv_buffer, 1, MPI_DOUBLE_COMPLEX, row_comunicator[group_row]);
    fft(recv_buffer, (size_t) dimension);
    //Shift Columns
    MPI_Alltoall(recv_buffer, 1, MPI_DOUBLE_COMPLEX, signal_proccessing_buffer, 1, MPI_DOUBLE_COMPLEX, column_comunicator[group_column]);
    fft(signal_proccessing_buffer, (size_t) dimension);

    #ifdef DEBUG
    printf("%d: meu vetor =", rank);
    for (int i = 0; i < dimension; i++){
      printf(" %lf + %lfj,", creal(signal_proccessing_buffer[i]), cimag(signal_proccessing_buffer[i]));
    }
    printf("\n");
    #endif

    //Finally
    //Gathering all data
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(signal_proccessing_buffer, dimension, MPI_DOUBLE_COMPLEX, transform_signal, dimension, MPI_DOUBLE_COMPLEX, 0, MPI_COMM_WORLD);

    //Printing all data
    if(rank == 0){
      for(int i = 0; i < sig_size; i++){
        printf("%lf + %lfj\n", creal(transform_signal[i]), cimag(transform_signal[i]));
      }
    }
    MPI_Finalize();

    return 0;
}