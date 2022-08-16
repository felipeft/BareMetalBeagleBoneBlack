#include <stdio.h>
#include <stdbool.h>

int qtd=0;
int qtd1=0;

int countBits(bool w,unsigned int value){
    int x;
    for(int i = 31; i>=0; i--){
        x = value >> i;
        if(x & 1){
            if(w == 1){
                qtd++;
            }
        }else{
            if(w == 0){
                qtd++;
            }
        }
        
    }
}

int main(){
    bool search;
    int num;
    scanf("%d %d", &search, &num);
    countBits(search, num);
    printf("%d\n", qtd);
}