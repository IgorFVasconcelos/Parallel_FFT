#include "../headers/bitops.h"

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