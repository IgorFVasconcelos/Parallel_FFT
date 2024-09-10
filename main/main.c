#include <stdio.h>
#include <stdlib.h>
#include "../headers/fft.h"
#include <mpi.h>

int main(int argc, char** argv){
    //File logic
    char buffer[10];
    char* path = "../data/teste.txt";
    FILE* f = fopen(path, "r");

    if(f == NULL){
        printf("File path not found\n\n");
        exit(-1);
    }

    //Temporário, quando compilar passar o tamanho do sinal
    //O tamanho Precisa necessariamente ser um cubo perfeito
    size_t sig_size = 4096;
    cd signal[sig_size];
    char* ptr = NULL;

    for(int i = 0; i < sig_size; i++){
        ptr = fgets(buffer, 10, f);
        if(ptr == NULL){
            break;
        }
        signal[i] += atoi(ptr);

    }

    //MPI logic



    //OpenMP logic

}