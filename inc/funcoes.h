#ifndef FUNCOES_H
#define FUNCOES_H

// Define constantes de tamanho
#define TAM_REGISTRO 112
#define TAM_DATAI 10
#define TAM_DATAF 10

// Define nome da base de dados
#define BASEDADOS "dados"
#define INDICE "indice"

// Defines do Buffer Pool
#define bufferRRN 29
#define frequencia 30
#define pageFaults (*bufferPool)[5][0]
#define pageHits (*bufferPool)[5][1]

int carregarArquivo(char *nome);
void funcionalidade2();
void funcionalidade3(const char *campo, const char *valor);
void funcionalidade4(int RRN);
int funcionalidade5(int RRN);
int funcionalidade6(int codEscola, char* dataI, char* dataF, char* nomeEscola, char* municipio, char* endereco);
int funcionalidade7(int RRN, int codEscola, char* dataI, char* dataF, char* nomeEscola, char* municipio, char* endereco);
int funcionalidade8() ;
void funcionalidade9();
int funcionalidade10(char *nomeArq, int ***buffer);
int funcionalidade11(int cod, int RRN, int RRNPag, int pagAnterior, int podeDescer, int ***buffer);
void funcionalidade12(int chaveDeBusca);
//int funcionalidade13(int RRN);

int ** bufferCreate();
void bufferClose(int *** bufferPool);
int * carregaRaiz(int *** bufferPool);
void escrevePagina(int * pagina);
void bufferLFU(int *novaPagina, int *** bufferPool);
int * carregaPagina(int RRN, int *** bufferPool);
void atualizaPagina(int * pag, int *** bufferPool);
int * novaPagina(int ***bufferPool);

#endif
