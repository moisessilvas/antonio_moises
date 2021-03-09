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

int main(){
    //leitura de uma linha específica
    char weight[28762];
    int linha;
    linha=2;
    lerLinha(weight, linha);

    //manipulação dessa linha
    int i, j;
    j = strlen(weight);
    char conv[j];
    for (int i = 0; i < j; i++) {
        if (weight[i] == '*') {
            for (int k = i; k < j; k++) {
                weight[k] = weight[k + 1]; //retira o índice *
            }
        }
        conv[i] = weight[i];
        double x = atof(conv); //converter em double o número recebido
    }
    puts(conv);
    return 0;
}