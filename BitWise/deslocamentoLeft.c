#include <stdio.h>
#include <stdbool.h>
#include <math.h>

int setBits(int w, unsigned int i, unsigned int j, unsigned int value){
    if(j>31){
        return 0;
    }
    int num;
    for(unsigned int a=i; a<=j; a++){
        num = pow(2, a);
        printf("%d\n",num);
    }
}

int main(){
    int a, b, c, val;
    scanf("%d %d %d %d", &a, &b, &c, &val);
    int num;
    num = setBits(a,b,c,val);
    
}