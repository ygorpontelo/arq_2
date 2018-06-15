/*
Integrantes (Grupo 11):
Lucas Mateus Martins Araújo e Castro (10295388)
Miguel de Mattos Gardini (10295728)
Vitor Rossi Speranza (10262523)
Ygor Franco de Camargo Pontelo (10295631)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <funcoes.h>

int main(int argc, char * const argv[]){
    if(argc < 2){
        printf("Programa precisa de argumentos para execucao!\n");
        return 0;
    }

    char nomeArquivo[20];
    int funcionalidade = atoi(argv[1]), RRN;

    switch(funcionalidade){
        case 1:
            strcpy(nomeArquivo, argv[2]);
            if(strlen(nomeArquivo) > 0 && carregarArquivo(nomeArquivo)){
                printf("Arquivo Carregado.\n");
            }else{
                printf("Falha no carregamento do arquivo.\n");
            }
        break;

        case 2:
            funcionalidade2();
        break;

        case 3:
            funcionalidade3(argv[2], argv[3]);
        break;

        case 4:
            RRN = atoi(argv[2]);
            funcionalidade4(RRN);
        break;

        case 5:
            RRN = atoi(argv[2]);
            if(funcionalidade5(RRN)){
                printf("Registro removido com sucesso.\n");
            }else{
                printf("Registro inexistente.\n");
            }
        break;

        case 6:
            if(funcionalidade6(atoi(argv[2]), argv[3], argv[4], argv[5], argv[6], argv[7])){
                printf("Registro inserido com sucesso.\n");
            }else{
                printf("Falha no processamento do arquivo.\n");
            }
        break;

        case 7:
            RRN = atoi(argv[2]);
            if(funcionalidade7(RRN, atoi(argv[3]), argv[4], argv[5], argv[6], argv[7], argv[8])){
                printf("Registro alterado com sucesso.\n");
            }else{
                printf("Registro inexistente.\n");
            }
        break;

        case 8:
            if(funcionalidade8()){
                printf("Arquivo de dados compactado com sucesso.\n");
            }else{
                printf("Falha no processamento do arquivo.\n");
            }

        break;

        case 9:
            funcionalidade9();
        break;

        case 10:
            strcpy(nomeArquivo, argv[2]);
            if(strlen(nomeArquivo) > 0 && funcionalidade10(nomeArquivo)){
                printf("Arquivo Carregado.\n");
            }else{
                printf("Falha no carregamento do arquivo.\n");
            }
        break;

        case 12:
            RRN = atoi(argv[2]);
            funcionalidade12(RRN);
        break;

        default:
            printf("Funcionalidade inexistente!\n");
        break;
    }

    return 0;
}
