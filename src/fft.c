#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include "../headers/fft.h"

//Função que gerencia o butterfly
int mpi_reverse(int num, int logn){
    int res = 0;
    for (int i = 0; i < logn; i++){
        if(num & (1 << i)){
            res |= 1 << (logn - 1 - i); 
        }
    }
    return res;
}

//FFT
void fft(cd* signal, size_t size){

    int logn = log2(size);

    //Fazendo as trocas da butterfly
    for (int i = 0; i < size; i++){
        int rev = mpi_reverse(i, logn);

        if (i < rev) {
            //swap
            cd temp = signal[i];
            signal[i] = signal[rev];
            signal[rev] = temp;
        }
    }

    //FFT propriamente dita
    for(int len = 2; len <= size; size <<= 1){

        double ang = 2 * PI / len;
        cd wlen = cos(ang) + sin(ang) * I;

        for (int i = 0; i < size; i += len) {
            cd w = 1.0;
            for(int j = 0; j< len / 2; j++){
                cd u = signal[i+j];
                cd v = signal[i+j+len/2] * w;
                signal[i+j] = u + v;
                signal[i+j+len/2] = u - v;
                w *= wlen;
            }
        }
    }
}