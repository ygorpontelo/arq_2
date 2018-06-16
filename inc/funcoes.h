#ifndef FUNCOES_H
#define FUNCOES_H

// Define constantes de tamanho
#define TAM_REGISTRO 112
#define TAM_DATAI 10
#define TAM_DATAF 10

// Define nome da base de dados
#define BASEDADOS "dados"
#define INDICE "indice"

int carregarArquivo(char *nome);
void funcionalidade2();
void funcionalidade3(const char *campo, const char *valor);
void funcionalidade4(int RRN);
int funcionalidade5(int RRN);
int funcionalidade6(int codEscola, char* dataI, char* dataF, char* nomeEscola, char* municipio, char* endereco);
int funcionalidade7(int RRN, int codEscola, char* dataI, char* dataF, char* nomeEscola, char* municipio, char* endereco);
int funcionalidade8() ;
void funcionalidade9();
int funcionalidade13(int RRN);
void funcionalidade12(int chaveDeBusca);
int ** bufferCreate();
void bufferClose(int ** bufferPool);

#endif
