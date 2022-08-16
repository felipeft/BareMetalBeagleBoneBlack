#include <stdio.h>

void printBits(unsigned char valor){
    unsigned char i;
    unsigned mascara = (1<<7);
    printf("%d = 0B",valor);

    for(i = 1; i<=8; i++){
        putchar(valor & mascara ? '1':'0');
        valor<<=1;
    }
    putchar('\n');
}

int BitSet(unsigned int x){
    unsigned int count = 0;
    while(x){
        count += x & 1;
        x >>= 1;
    }
    return count;
}

int main(){
    int valor=5;
    printf("%d\n", BitSet(valor));
}