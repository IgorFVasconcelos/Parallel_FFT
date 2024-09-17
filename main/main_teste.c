#include <stdio.h>
#include <stdlib.h>
#include "../headers/fft.h"
#include "../headers/bitops.h"
//#include <mpi.h>
#include <math.h>

int main(void){
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
    size_t sig_size = 4096;//pow(sqrt(4), 3);
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
    // for(int i; i<sig_size; i++){
    //     printf("%d\n", signal[i]);
    // }

    complex double complex_signal[sig_size];
    for (int i = 0; i<sig_size; i++){
        complex_signal[i] = (double complex) signal[i];
    }
    
    fft(complex_signal, 4096);

    for(int i = 0; i < sig_size; i++){
        printf("%lf + %lfj\n", creal(complex_signal[i]), cimag(complex_signal[i]));
    }

    return 0;
}