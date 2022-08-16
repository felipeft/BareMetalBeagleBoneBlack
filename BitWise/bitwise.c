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

int main(){
    unsigned char valor;
    valor = 7;
    //valor = valor & ~valor;
    printBits(valor);

}