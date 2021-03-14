#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lerLinha(char *pesos, int linha){
    FILE *file;
    if((file=fopen("weights.txt", "r"))==NULL){
        printf("não encontrado");
        exit(1);
    }
    int cont=0;
    while(cont!=linha){
        fscanf(file, "%s", pesos);
        cont++;
    }
    fclose(file);
    return 0;
}

void divideString(char *str, int n) 
{ 
    int str_size = strlen(str); 
    int i; 
    int part_size; 
  
    part_size = str_size / n; 
    for (i = 0; i< str_size; i++){ 
        if (i % part_size == 0) 
            printf("\n");  
            printf("%c", str[i]); 
    } 
} 

int main(){
    //leitura de uma linha específica
    char weight[28762];
    int linha;
    linha=2;
    lerLinha(weight, linha);

    //manipulação dessa linha
    int i, j;
    int a = 0;
    j = strlen(weight);

    //array que terá os pesos
    char conv[j];

    //intermediário que transformará a string recebida em double
    char conv_middle[j];

    //for que percorre a linha
    for (int i = 0; i < j; i++) {

        //condição de encontro de caracter especial
        if (weight[i] == '*') {
            //conta quantos * possui a string
            ++a;
            for (int k = i; k < j; k++) {
                //retira o índice *
                weight[k] = weight[k + 1];
                
            }
        }
        conv_middle[i] = weight[i];
    }
    char *str = conv_middle;
    divideString(str, a);

    //converter em double o número recebido
    double x;
    sscanf(str, "%lf", &x);
    
    //strcpy(conv,str);
    //puts(conv);
    return 0;
}
