#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funcoes.h"

int carregarArquivo(char *nomeArq){
    FILE *arquivo = fopen(nomeArq, "rb");
    FILE *dados = fopen(BASEDADOS, "wb+");

    // Confere se conseguiu abrir os arquivos
    if(arquivo == NULL || dados == NULL){
        return 0;
    }

    char leitura, palavra[100], nome[100], muni[100], end[100];
    int tamNome, tamMuni, tamEnd, tamDataI, tamDataF, cod, topo = -1;

    fputc('0', dados);                      // Status inicial como 0
    fwrite(&topo, sizeof(int), 1, dados);   // Topo da pilha, comeca com -1

    // Percorre todo o arquivo
    while(fscanf(arquivo, "%d", &cod) > 0){
        tamNome = 0; tamEnd = 0; tamMuni = 0; tamDataI = 0; tamDataF = 0;

        // Escreve o cod no arquivo
        fwrite(&cod, sizeof(int), 1, dados);
        fgetc(arquivo); // Consome ';'

        // Pega nome
        leitura = fgetc(arquivo);
        while(leitura != ';'){
            palavra[tamNome] = leitura;
            tamNome++;
            leitura = fgetc(arquivo);
        }
        // Se nome nao for nulo, guarda em nome
        if(tamNome > 0){
          strncpy(nome, palavra, tamNome);
        }

        // Pega municipio
        leitura = fgetc(arquivo);
        while(leitura != ';'){
            palavra[tamMuni] = leitura;
            tamMuni++;
            leitura = fgetc(arquivo);
        }
        // Se municipio nao for nulo, guarda em muni
        if(tamMuni > 0){
          strncpy(muni, palavra, tamMuni);
        }

        // Pega endereco
        leitura = fgetc(arquivo);
        while(leitura != ';'){
            palavra[tamEnd] = leitura;
            tamEnd++;
            leitura = fgetc(arquivo);
        }
        // Se endereco nao for nulo, guarda em end
        if(tamEnd > 0){
          strncpy(end, palavra, tamEnd);
        }

        // Pega data inicio
        leitura = fgetc(arquivo);
        while(leitura != ';'){
            palavra[tamDataI] = leitura;
            tamDataI++;
            leitura = fgetc(arquivo);
        }
        // Define o que sera escrito em data inicial
        if(tamDataI > 0){
          fwrite(palavra, TAM_DATAI, 1, dados);
        }else{
          fwrite("0000000000", TAM_DATAI, 1, dados);
        }

        // Pega data final
        leitura = fgetc(arquivo);
        while(leitura != '\n'){
            palavra[tamDataF] = leitura;
            tamDataF++;
            leitura = fgetc(arquivo);
        }
        // Define o que sera escrito em data final
        if(tamDataF > 0){
          fwrite(palavra, TAM_DATAF, 1, dados);
        }else{
          fwrite("0000000000", TAM_DATAF, 1, dados);
        }

        // Escreve os campos de tamanho variavel, caso seu tam seja > 0
        fwrite(&tamNome, sizeof(int), 1, dados);
        if(tamNome > 0){
          fwrite(nome, tamNome, 1, dados);
        }

        fwrite(&tamMuni, sizeof(int), 1, dados);
        if(tamMuni > 0){
          fwrite(muni, tamMuni, 1, dados);
        }

        fwrite(&tamEnd, sizeof(int), 1, dados);
        if(tamEnd > 0){
          fwrite(end, tamEnd, 1, dados);
        }

        // Preenche o RRN com vazio para ajustar tamanho de registro
        int i, total = TAM_REGISTRO - (tamNome + tamMuni + tamEnd + TAM_DATAF + TAM_DATAI + 4 + 4 + 4 + 4); // Cod + delimitadores
        for(i = 0; i<total; i++){
          fwrite(" ", 1, 1, dados);
        }

        // Evita que o buffer sobrecarregue
        fflush(dados);
    }

    fseek(dados, 0, SEEK_SET);  // Vai até o inicio
    fputc('1', dados);          // Se deu tudo certo, status vira 1

    // Fecha arquivos
    fclose(arquivo);
    fclose(dados);

    return 1;
}


//Funcao que printa o registro inteiro, utilizada nas funcionalidades 2, 3 e 4
void printaRegistro(FILE *f, int x) {
  char s[80];
  int i, j;
  int offset = ftell(f);

  //printa codigo da escola
  printf("%d ", x);

  //printa data inicial e data final
  for (i=0; i<2; i++) {
    fread(s, 1, 10, f);
    for (j=0; j<10; j++) {
      printf("%c", s[j]);
    }
    printf(" ");
  }

  //printa os 3 campos variaveis
  for(i=0; i<3; i++) {
    //printa tamanho do campo
    fread(&x, 4, 1, f);
    printf("%d ", x);

    //printa campo
    fread(s, 1, x, f);
    for (j=0; j<x; j++) {
      printf("%c", s[j]);
    }
    printf(" ");
  }
  printf("\n");

  fseek(f, offset, SEEK_SET);
}

//funcionalidade 2 = printa todos os dados de todos os RRNs
void funcionalidade2() {
  char s;  //char auxiliar para verificar integridade do arquivo
  int deleted;  //variavel para veriticar registros removidos
  int notFound = 1; //flag para avisar se o arquivo esta vazio
  FILE *f = fopen(BASEDADOS, "rb");

  //checando o status do arquivo
  fread(&s, 1, 1, f);
  if (s == '0') {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  fseek(f, 5, SEEK_SET);
  while(fread(&deleted, sizeof(int), 1, f) > 0) {
    if (deleted != -1) {  //registro nao removido
      notFound = 0; //Foi encontrado ao menos um registro
      printaRegistro(f, deleted); //Se o registro nao foi removido, "deleted" guarda o numero da escola
    }

    fseek(f, 108, SEEK_CUR);  //indo para proximo registro
  }

  if (notFound) {
    printf("Registro inexistente.\n");
  }

  //Fechando o arquivo
  fclose(f);
}

/*Funcao que compara duas strings.
tam = tamanho de s1
s2 esta entre aspas simples
Retorna 1 se elas forem iguais, 0 se forem diferentes */
int comparaStrings(const char *s1, int tam, const char *s2) {
  int i;

  if(tam == 0 && strlen(s2) > 0){
    return 0;
  }

  for (i=0; i<tam; i++) {
    if (s1[i] != s2[i]) {
      return 0;
    }
  }

  return 1;
}

//Funcao que compara o valor do campo de um registro com um valor buscado
//Retorna 1 se forem iguais e 0 se nao forem iguais
int comparaCampo(FILE *f, int campoBuscado, const char *valor, int x) {
  char s[80];
  int campoAtual;
  int offset = ftell(f);

  if (campoBuscado < 3) {
    //Bucando campos de tamanho fixo
    switch (campoBuscado) {
      case 0:
        return (x == atoi(valor));
      break;

      case 1:
        fread(s, 1, 10, f);
        fseek(f, offset, SEEK_SET);
        return comparaStrings(s, 10, valor);
      break;

      case 2:
        fseek(f, 10, SEEK_CUR);
        fread(s, 1, 10, f);
        fseek(f, offset, SEEK_SET);
        return comparaStrings(s, 10, valor);
      break;
    }
    fseek(f, offset, SEEK_SET);
    return 0;
  } else {
    //Buscando campos de tamanho variavel
    fseek(f, 20, SEEK_CUR);

    for (campoAtual = 3; campoAtual < campoBuscado; campoAtual++) {
      fread(&x, 4, 1, f);
      fseek(f, x, SEEK_CUR);
    }

    fread(&x, 4, 1, f);
    fread(s, 1, x, f);

    fseek(f, offset, SEEK_SET);
    return comparaStrings(s, x, valor);
  }
}

//recupera todos os dados de todos os registros com um certo criterio de busca
void funcionalidade3(const char *campo, const char *valor) {
  char s;  //char auxiliar para verificar integridade do arquivo e registros removidos
  int notFound = 1;
  int deleted;  //Variavel para verificar registros removidos
  int campoBuscado; //variavel que identifica o campo buscado
  FILE * f = fopen(BASEDADOS, "rb");

  //checando o status do arquivo
  fread(&s, 1, 1, f);
  if (s == '0') {
    printf("Falha no processamento do arquivo.\n");
    fclose(f);
    return;
  }

  //Setando status do arquivo para 0
  s = '0';
  fseek(f, 0, SEEK_SET);
  fwrite(&s, 1, 1, f);

  //switch para determinar o campoBuscado
  switch (campo[0]) {
    case 'c': //codEscola
      campoBuscado = 0;
    break;

    case 'd': //data
      if (campo[4] == 'I') {  //Inicio
        campoBuscado = 1;
      } else {  //Final
        campoBuscado = 2;
      }
    break;

    case 'n': //nomeEscola
      campoBuscado = 3;
    break;

    case 'm': //municipio
      campoBuscado = 4;
    break;

    case 'e': //endereco
      campoBuscado = 5;
    break;
  }

  fseek(f, 5, SEEK_SET);
  while(fread(&deleted, sizeof(int), 1, f) > 0) {
    if (deleted != -1) {
      if (comparaCampo(f, campoBuscado, valor, deleted)) {
        notFound = 0; //Foi encontrado ao menos um registro
        printaRegistro(f, deleted);
      }
    }

    fseek(f, 108, SEEK_CUR);  //indo para o proximo registro
  }

  if (notFound) {
    printf("Registro inexistente.\n");
  }

  //Setando status do arquivo para 1 e fechando o arquivo
  s = '1';
  fseek(f, 0, SEEK_SET);
  fwrite(&s, 1, 1, f);
  fclose(f);
}

void funcionalidade4(int RRN) {
  char s;  //char auxiliar para verificar integridade do arquivo e registros removidos
  int deleted;  //Variavel para verificar arquivos removidos
  FILE * f = fopen(BASEDADOS, "rb");

  //checando o status do arquivo
  fread(&s, 1, 1, f);
  if (s == '0') {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  //Setando status do arquivo para 0
  s = '0';
  fseek(f, 0, SEEK_SET);
  fwrite(&s, 1, 1, f);

  fseek(f, 5 + (112*RRN), SEEK_SET);
  if (fread(&deleted, sizeof(int), 1, f) != 0) {
    if (deleted != -1) {
      printaRegistro(f, deleted);
    } else {
      printf("Registro inexistente.\n");
    }
  } else {
    printf("Registro inexistente.\n");
  }

  fclose(f);
}

int funcionalidade5(int RRN){
    int top, minusOne = -1;
    char s;
    FILE *f = fopen(BASEDADOS, "rb+"); // Abre o arquivo para leitura e escrita

    fread(&s, sizeof(char), 1, f); // Verifica se o primeiro byte eh 0 ou 1

    if (s == '0') { // Se ele for 0 informa o problema, e retorna da funcao
        fclose(f);
        return 0;
    }
    fseek(f, RRN * TAM_REGISTRO + 5, SEEK_SET); // Se nao va ateh a RRN a ser removida e verifica se ela existe
    if(feof(f)){
        fclose(f);
        return 0;
    }

    fseek(f, RRN * TAM_REGISTRO + 5, SEEK_SET);
    fread(&minusOne, sizeof(int), 1, f);
    if(minusOne == -1){ // Verifica se ja foi removida
        fclose(f);
        return 0;
    }
    minusOne = -1;

    fseek(f, 0, SEEK_SET); // Para comecar a editar muda o byte de check para 0
    fputc('0', f);

    fread(&top, sizeof(int), 1, f); // Pego o atual topo da pilha
    fseek(f, RRN * TAM_REGISTRO + 5, SEEK_SET);
    fwrite(&minusOne, sizeof(int), 1, f); // Marca como apagado
    fwrite(&top, sizeof(int), 1, f); // salva o topo atual no espaco que estava o dado
    fseek(f, 1, SEEK_SET);
    fwrite(&RRN, sizeof(int), 1, f); // Guarda o novo topo como o apagado

    fseek(f, 0, SEEK_SET);
    fputc('1', f);// Terminando de editar marca o check como editado
    fclose(f);
    return 1;// Fecha o arquivo e sai do programa
}

int funcionalidade6(int codEscola, char* dataI, char* dataF, char* nomeEscola, char* municipio, char* endereco){
    int top, aux, new_top;
    char s;
    FILE *f = fopen(BASEDADOS, "rb+"); // Abre a arquivo para leitura e escrita

    fread(&s, sizeof(char), 1, f); // Verifica sua integridade

    if (s == '0') {
        fclose(f);
        return 0;
    }

    fseek(f, 0, SEEK_SET); // Marca como sendo alterado
    fputc('0', f);

    fread(&top, sizeof(int), 1, f); // Verifica qual o topo atual da pilha

    if(top != -1){
        fseek(f, top * TAM_REGISTRO + 5, SEEK_SET); // Se o topo for diferente de -1 (tem espaco p/ ser utilizado)
        fread(&aux, sizeof(int), 1, f);
        printf("top: %d\n", top);
        if(aux != -1){ // Verifica se o lugar apontado pelo topo esta realmente apagado
            fclose(f);
            return 0;
        }
        fread(&new_top, sizeof(int), 1, f); // Se estiver apagado salva o proximo elemento da pilha

        fseek(f, top * TAM_REGISTRO + 5, SEEK_SET);
    }else{ // Se o topo da pilha for -1 entao adiciona o novo elemento ao final do arquivo
        fseek(f, 0, SEEK_END);
        new_top = -1; // Garante que o -1 seja escrito caso nao tenha mais espaco livre
    }
    fwrite(&codEscola, sizeof(int), 1, f); // Escreve o codigo da escola

    if(strlen(dataI) > 1){ // E a data de inicio do ano letivo (ou 0s)
        fwrite(dataI, TAM_DATAI, 1, f);
    }else{
        fwrite("0000000000", TAM_DATAI, 1, f);
    }
    if(strlen(dataF) > 1){  // E a data de final do ano letivo (ou 0s)
        fwrite(dataF, TAM_DATAF, 1, f);
    }else{
        fwrite("0000000000", TAM_DATAF, 1, f);
    }
    aux = strlen(nomeEscola);
    fwrite(&aux, sizeof(int), 1, f);              // Guarda o tamanho do nome da escola
    fwrite(nomeEscola, strlen(nomeEscola), 1, f); // E o nome em seguida

    aux = strlen(municipio);
    fwrite(&aux, sizeof(int), 1, f);// Guarda o tamanho do municipio
    fwrite(municipio, strlen(municipio), 1, f);// E o municipio em seguida

    aux = strlen(endereco);
    fwrite(&aux, sizeof(int), 1, f);// Guarda o tamanho do endereco
    fwrite(endereco, strlen(endereco), 1, f);// E o endereco em seguida

    fseek(f, 1, SEEK_SET);
    fwrite(&new_top, sizeof(int), 1, f); // Guarda o proximo elemento da pilha como o novo topo

    fseek(f, 0, SEEK_SET);
    fputc('1', f); // Seto a integridade do arquivo ao termino da execucao
    fclose(f);
    return 1; // Fecha o arquivo e retorna
}

int funcionalidade7(int RRN, int codEscola, char* dataI, char* dataF, char* nomeEscola, char* municipio, char* endereco){
    int aux;
    char s;
    FILE *f = fopen(BASEDADOS, "rb+"); // Abre o arquivo para leitura e escrita

    fread(&s, sizeof(char), 1, f); // Verifica a integridade do arquivo

    if (s == '0') {
        fclose(f);
        return 0;
    }

    fseek(f, RRN * TAM_REGISTRO + 5, SEEK_SET); // Va ate o RRN a ser alterado e verifica se ele existe
    if(feof(f)){
        fclose(f);
        return 0;
    }

    fseek(f, 0, SEEK_SET); // Seta ele como nao integro pois iremos altera-los
    fputc('0', f);
    fseek(f, RRN * TAM_REGISTRO + 5, SEEK_SET);

    fwrite(&codEscola, sizeof(int), 1, f); // Sebrescreve o codigo da escola

    if(strlen(dataI) > 1){ // E a data de comeco
        fwrite(dataI, TAM_DATAI, 1, f);
    }else{
        fwrite("0000000000", TAM_DATAI, 1, f);
    }
    if(strlen(dataF) > 1){ // E a data de termino
        fwrite(dataF, TAM_DATAF, 1, f);
    }else{
        fwrite("0000000000", TAM_DATAF, 1, f);
    }
    aux = strlen(nomeEscola);
    fwrite(&aux, sizeof(int), 1, f); // E o tamanho do nome e o nome
    fwrite(nomeEscola, strlen(nomeEscola), 1, f);

    aux = strlen(municipio);
    fwrite(&aux, sizeof(int), 1, f); // E o tamanho do nome do municipio e o nome do municipio
    fwrite(municipio, strlen(municipio), 1, f);

    aux = strlen(endereco);
    fwrite(&aux, sizeof(int), 1, f); // E o tamanho do endereco e o endereco
    fwrite(endereco, strlen(endereco), 1, f);

    fseek(f, 0, SEEK_SET);
    fputc('1', f); // asserta sua integridade apos alterado
    fclose(f);
    return 1; // Fecha o arquivo e retorna
}

int funcionalidade8() {
    int minusOne = -1;
    char aux[TAM_REGISTRO], check;
    FILE *oldfile = fopen(BASEDADOS, "rb"); // Abre o arquivo a ser desfragmentado para leituro
    FILE *newfile = fopen("new", "wb"); // Abre o novo arquivo (onde estara os dados desfragmentados) para escrita

    fread(&check, sizeof(char), 1, oldfile);
    if(check == '0'){ // verifica o arquivo lido eh integro
        fclose(newfile);
        fclose(oldfile);
        return 0;
    }

    fputc('0', newfile); // Seta o novo arquivo como nao integro
    fseek(oldfile, 5, SEEK_SET); // Pula o valor do topo da pilha do arquivo antigo

    fwrite(&minusOne, sizeof(int), 1, newfile);

    while(fread(&minusOne, sizeof(int), 1, oldfile) > 0){ // Enquanto tiver informacao a ser lida no arquivo antigo
        fread(aux, TAM_REGISTRO - sizeof(int), 1, oldfile);
        if(minusOne != -1){ // Verifica se esse dado foi deletado
            fwrite(&minusOne, sizeof(int), 1, newfile); // Se nao copia ele para o novo arquivo
            fwrite(aux, TAM_REGISTRO - sizeof(int), 1, newfile);
        }
        // Senao ignora ele
    }

    fseek(newfile, 0, SEEK_SET); // Seta o novo arquivo como integro
    fputc('1', newfile);
    fclose(newfile);
    fclose(oldfile);
    rename("new", BASEDADOS); // Renomeia o nome para o nome do antigo, apagando ele
    return 1; // Retorna da funcao
}

//Printa o estado atual da pilha de removidos
void funcionalidade9() {
  int x;
  char s;
  FILE * f = fopen(BASEDADOS, "rb");

  //checando o status do arquivo
  fread(&s, 1, 1, f);
  if (s == '0') {
    printf("Falha no processamento do arquivo.\n");
    return;
  }

  fread(&x, 4, 1, f);
  if (x == -1){
      printf("Pilha vazia.\n");
      fclose(f);
      return;
  }

  while (x != -1) {
    printf("%d ", x);
    fseek(f, 5 + (112*x) + 4, SEEK_SET);
    fread(&x, 4, 1, f);
  }
  printf("\n");

  //Fechando o arquivo
  fclose(f);
}
