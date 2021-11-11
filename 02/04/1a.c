#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define N   1000
#define K1  100
#define K2  120

long isNumber(const char* s);
int strtoi(const char *str, int opt);
void stampa_frequenza(int *arr, int size, int base, int n);

int main(int argc, char* argv[]){

    int n  = strtoi(argv[1], 0);
    int k1 = strtoi(argv[2], 1);
    int k2 = strtoi(argv[3], 2);
    printf("%s\n%s\n%s\n", argv[1], argv[2], argv[3]);
    printf("%d %d %d\n", n, k1, k2);

    int size_arr = abs(k2-k1);

    int *arr_freq = calloc(size_arr, sizeof(int));

    int i = 0, val = 0;
    unsigned state = 0;

    for(i= 0; i < n; i++){
        val = ((rand_r(&state))%(k2-k1))  + k1;
        arr_freq[val-k1]+=1;
    }

    stampa_frequenza(arr_freq, size_arr, k1, n); 


    free(arr_freq);
    return 0;
}

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}


int strtoi(const char *str, int opt){
    if(str != NULL && (*str != '\0') )
        return (int) isNumber(str);
    
    switch (opt)
    {
    case 0:
        return N;
        break;
    case 1:
        return K1;
        break;
    case 2:
        return K2;
        break;
    }
}

void stampa_frequenza(int *arr, int size, int base, int n){
    
    
    puts("Occorrenze:");
    
    int i;
    for(i = 0; i < size; i++){
        printf("%d : %d\n", base + i, arr[i]);
    }
}