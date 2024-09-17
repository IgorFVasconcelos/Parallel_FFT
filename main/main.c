#include <stdio.h>
#include <stdlib.h>
#include "../headers/fft.h"
#include "../headers/bitops.h"
#include <mpi.h>
#include <math.h>

int main(int argc, char** argv){

    //MPI Startup
    MPI_Init(&argc, &argv);
    unsigned int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    unsigned int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    unsigned int dimension = sqrt(world_size);

    //Definindo os buffers
    int signal_start_buffer[world_size];
    cd signal_proccessing_buffer[world_size];
    cd recv_buffer[world_size];

    //Definindo os comunicadores de linhas e colunas
    MPI_Comm row_comunicator[dimension];
    MPI_Comm column_comunicator[dimension];

    unsigned int num_bits = log2(world_size);

    unsigned int group_column = keepLSB(rank, num_bits/2); 
    unsigned int group_row = (keepMSB(rank, num_bits/2) >> num_bits/2);

    printf("Eu, o processo %u pertenco a linha %u e a coluna %u\n\n", rank, group_row, group_column);

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
        char* path = "/data/raw.txt";
        FILE* f = fopen(path, "r");

        if(f == NULL){
            printf("File path not found\n\n");
            exit(-1);
        }

        //Copy signal from file to array
        size_t sig_size = pow(sqrt(world_size), 3);

        int signal[sig_size];
        memset_0(signal, sig_size);

        char* ptr = NULL;
        char zero = '0';

        for(int i = 0; i < sig_size; i++){
            ptr = fgets(buffer, 10, f);

            if(ptr == NULL){
                printf("Erro, linha não encontrada");
                ptr = &zero;
            }
            signal[i] += atoi(ptr);
        }
        free(ptr);
        free(zero);

        //Send the appropriate data for each proccess
        for (int dest = 1; dest < world_size; dest++){
          int start = dest*world_size;
          MPI_Send(signal[start], world_size, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }

        for (int i = 0; i < world_size; i++){
          signal_proccessing_buffer[i] = (complex)signal[i];
        }

    }

    if (rank != 0){
      MPI_Recv(signal_start_buffer, world_size, MPI_INT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      
      for(int i = 0; i < world_size; i++){
        signal_proccessing_buffer[i] = (complex)signal_start_buffer[i];
      }

    }

    //Process locally
    fft(signal_proccessing_buffer, world_size);
    //Shift rows
    MPI_Alltoall(signal_proccessing_buffer, world_size, MPI_DOUBLE_COMPLEX, recv_buffer, world_size, MPI_DOUBLE_COMPLEX, row_comunicator[group_row]);
    fft(recv_buffer, world_size);
    //Shift Columns
    MPI_Alltoall(recv_buffer, world_size, MPI_DOUBLE_COMPLEX, signal_proccessing_buffer, world_size, MPI_DOUBLE_COMPLEX, column_comunicator[group_column]);
    fft(signal_proccessing_buffer, world_size);

    //OpenMP logic


    //Finally

    MPI_Finalize();

    return 0;
}