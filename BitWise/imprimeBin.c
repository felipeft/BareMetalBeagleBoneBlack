#include <stdio.h>

int nBin(int x){
    if(x!=0){
        nBin(x/2);
        printf("%d", x%2);
    }
}


int main(){
    int x;
    scanf("%d",&x);
    printf("%d\n",nBin(x));
}