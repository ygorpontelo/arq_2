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

/*
  Funcao que cria o indice com cabeçalho vazio
*/
void criaCabecalho(){
  FILE *indice = fopen(INDICE, "wb+");

  int num = -1;
  // Escreve cabeçalho
  fputc('0', indice);                   // Escreve status
  fwrite(&num, sizeof(int), 1, indice);   // Escreve noRaiz
  num = 0;
  fwrite(&num, sizeof(int), 1, indice);    // Escreve altura
  fwrite(&num, sizeof(int), 1, indice);   // Escreve ultimo RRN
  fseek(indice, 0, SEEK_SET);
  fputc('1', indice);
  fclose(indice);
}

/*
  Funcao para criar arq de dados e indice 
*/
int funcionalidade10(char *nomeArq, int ***buffer){
  carregarArquivo(nomeArq);

  FILE *arquivo = fopen(nomeArq, "rb");

  // Confere se conseguiu abrir os arquivos
  if(arquivo == NULL){
      return 0;
  }

  int cod, rrn = 0;

  // Percorre todo arquivo 
  while(fscanf(arquivo, "%d", &cod) > 0){
    funcionalidade11(cod, rrn, -1, -1, 1, buffer);
    rrn++;
  }

  return 1;
}

/*
  Funcao pra colocar um valor na arvore        
*/
int funcionalidade11(int cod, int RRN, int RRNPag, int ***buffer){
  int *pagina, *pagNova, *pagNova2, vetor; 

  // Se o RRN for da raiz
  if(RRNPag == -1){
    pagina = carregaRaiz(buffer);
  }else{
    pagina = carregaPagina(RRNPag, buffer);
  }

  // Acha posicao pra colocar no vetor  
  int i = 2, j, k;
  
  if(pagina[0] > 0){
    while(cod >= pagina[i] && i < 29){
      i+=3;  
    }
  }  
  
  printf("JEBA 1\n");
  // Se o vetor tiver cheio                
  if(pagina[0] > 8){
    printf("JEBA 2\n");
    // Se a chave for menor que a metade
    if(i <= 15){
      printf("JEBA 3\n");
      // Pagina divide na metade
      pagNova = novaPagina(buffer);
      pagNova[0] = 5;
      for(j = 1, k = 1; j < 16; j++, k++){
        if(j == i){
          pagNova[j] = cod;
          pagNova[j+1] = RRN;
          k-=2;
          j++;                    
        }else{
          pagNova[j] = pagina[k];
        }
      }
      
      // Cria a nova pagina
      pagNova2 = novaPagina(buffer);
      pagNova2[0] = 4; 
      for(i = 1; i < 11; i++){
        pagNova2[i] = pagina[i+18];
      }

      // Promove o sexto maior cod
      pagina[0] = 1;
      pagina[1] = pagina[16];
      pagina[2] = pagina[17];
      pagina[3] = pagina[18];

      atualizaPagina(pagina, buffer);
      atualizaPagina(pagNova, buffer);          
      atualizaPagina(pagNova2, buffer);
    }else{
      printf("JEBA 4\n");
      // Pagina divide na metade
      pagNova = novaPagina(buffer);
      pagNova[0] = 5;
      for(j = 1; j < 16; j++){
        pagNova[j] = pagina[j];
      }
      
      // Cria a nova pagina
      pagNova2 = novaPagina(buffer);
      pagNova2[0] = 4; 
      for(j = 1, k = 1; j < 11; j++, k++){
        if(i == j){
          pagNova2[j] = cod;
          pagNova2[j+1] = RRN;
          k-=2;
          j++;
        }else{
          pagNova2[j] = pagina[k+18];
        }
      }

      // Promove o sexto maior cod
      pagina[0] = 1;
      pagina[1] = pagina[16];
      pagina[2] = pagina[17];
      pagina[3] = pagina[18];

      atualizaPagina(pagina, buffer);          
      atualizaPagina(pagNova, buffer);
      atualizaPagina(pagNova2, buffer);
    }
  }else{
    printf("JEBA 5\n");
    if(pagina[i+2] != -1){
      funcionalidade11(cod, RRN, pagina[i+2], buffer);
    }else{
      // Coloca os valores corretos no vetor aux
      for(j = 1, k = 1; j < 29; j++, k++){
        if(i == j){
          vetor[j] = cod;
          vetor[j+1] = RRN;
          k-=2;   
          j++;     
        }else{
          vetor[j] = pagina[k];        
        }      
      }
      // vetor original fica igual a vetor aux    
      for(j = 0; j < 29; j++){
        pagina[j] = vetor[j];
      }
      // Atualiza a pagina que foi modificada
      atualizaPagina(pagina, buffer);
    }
  }
  printf("JEBA 6\n");

  return 1;
}

void funcionalidade12(int chaveDeBusca) {

}

/*
Funcao que cria o bufferPool e retorna para o usuario
Cria a raiz caso ela ainda nao exista
*/
int ** bufferCreate() {
  int ** bufferPool;
  char s;
  int i;
  int *raiz;
  FILE *f = fopen(INDICE, "wb");

  if (f == NULL) {
    criaCabecalho();
    f = fopen(INDICE, "wb");
  }

  //checando o status do arquivo
  fread(&s, 1, 1, f);
  if (s == '0') {
    printf("Falha no processamento do arquivo.\n");
    return NULL;
  }

  //alocando memoria do bufferPool
  bufferPool = (int **) malloc(sizeof(int*)*6);
  for (i=0; i<6; i++) {
    //Cada posicao da matriz armazena 124 bytes (31 posicoes de inteiros):
    //116 bytes do nó,
    //4 bytes do RRN do nó e
    //4 bytes para o contador para a política do bufferPool (LFU)
    bufferPool[i] = (int *) malloc(sizeof(int) * 31);
    bufferPool[i][bufferRRN] = -1;
  }

  //zerando pageFault e pageHit do buffer
  bufferPool[5][0] = 0;
  bufferPool[5][1] = 0;

  fread(&i, 4, 1, f); //lendo o RRN da raiz
  if (i != -1) {
    raiz = malloc(sizeof(int)*30);
    fseek(f, 13 + i*116, SEEK_SET);
    fread(raiz, 4, 29, f); //lendo a raiz

    raiz[bufferRRN] = i; //guardando RRN da raiz
  } else {
    //cria nova raiz
    raiz = novaPagina(&bufferPool);

    //atualiza RRN da raiz no cabecalho
    fseek(f, 1, SEEK_SET);
    fwrite(&(raiz[bufferRRN]), 4, 1, f); 
  }

  fclose(f);

  //colocando raiz no buffer (a raiz fica na linha 4 do bufferPool)
  for (i=0; i<30; i++) {
    bufferPool[4][i] = raiz[i];
  }
  bufferPool[4][frequencia] = 2147483647;  //2.147.483.647 = int_max

  free(raiz);
  return bufferPool;
}

void bufferClose(int ***bufferPool) {
  char s;
  int i;
  FILE *f = fopen(INDICE, "wb");

  printf("olca %d\n", (*bufferPool)[0][bufferRRN]);

  //Laco para escrever cada no do bufferPool
  for (i=0; i<5; i++) {
    printf("JEBA1\n");
    fseek(f, 13 + ((*bufferPool)[i][bufferRRN]) * 116, SEEK_SET);
    printf("JEBA2\n");
    fwrite((*bufferPool)[i], 4, 29, f);
  }

  //Voltando status do arquivo para 1
  s = '1';
  fseek(f, 0, SEEK_SET);
  fwrite(&s, 1, 1, f);
  fclose(f);

  //Anotando numero de PageFaults e PageHits
  f = fopen("buffer-info.text", "a");
  fprintf(f, "Page fault: %d; Page hit: %d.\n", pageFaults, pageHits);
  fclose(f);


  //liberando memoria do bufferPool
  for (i=0; i<6; i++) {
    free((*bufferPool)[i]);
  }
  free(*bufferPool);
}

/*
Funcao que busca um RRN no bufferPool
Retorna um vetor de inteiros (a linha da matriz) caso esteja no buffer
Retorna NULL caso nao esteja no buffer
*/
int * bufferGet(int RRN, int ***bufferPool) {
  int i;
  for (i=0; i<4; i++) {
    if ((*bufferPool)[i][29] == RRN) {
      pageHits++;
      (*bufferPool)[i][frequencia]++;  //aumenta taxa de uso da pagina buscada
      return (*bufferPool)[i];
    }
  }

  pageFaults++;
  return NULL;
}

//Funcao que retorna para o usuario a raiz da arvore em um vetor de inteiros
int * carregaRaiz(int ***bufferPool) {
  int i;

  int * raiz = malloc(sizeof(int)*30);
  for (i=0; i<29; i++) {
    raiz[i] = (*bufferPool)[4][i];
  }

  return raiz;
}


//Funcao que escreve a pagina enviada pelo usuario no arquivo
void escrevePagina(int *pagina) {
  FILE *f = fopen(INDICE, "wb");

  //vai ate o RRN da pagina e escreve
  fseek(f, 13 + pagina[bufferRRN]*116, SEEK_SET);
  fwrite(pagina, 4, 29, f);

  fclose(f);
}

/*
Funcao que aplica a politica LFU no buffer
Substitui a linha menos frequentemente utilizada do bufferPool pela novaPagina
*/
void bufferLFU(int *novaPagina, int ***bufferPool) {
  int i, j;
  int LFU = 0;
  for (i=0; i<4; i++) { //itera sobre todos os membros do bufferPool (exceto a raiz)

    //caso o espaco esteja vazio
    if ((*bufferPool)[i][bufferRRN] == -1) {
      for (j=0; j<30; j++) {
        (*bufferPool)[i][j] = novaPagina[j];
      }
      (*bufferPool)[i][frequencia] = 1;
      return;
    }

    //encontrando o menos frequentemente utilizado
    if ((*bufferPool)[LFU][frequencia] > (*bufferPool)[i][frequencia]) {
      LFU = i;
    }
  }

  escrevePagina((*bufferPool)[LFU]);

  for (i=0; i<30; i++) {
    (*bufferPool)[LFU][i] = novaPagina[i];
  }
  (*bufferPool)[LFU][frequencia] = 1;
}

/*
Funcao que retorna, em um vetor de inteiros, o no da arvore que possui o RRN passado pelo usuario
Aplica a politica de substituicao LFU (Last Frequently Used) para decidir qual RRN do bufferPool sera substituido
(Caso o no ja nao esteja no bufferpool)
*/
int* carregaPagina(int RRN, int ***bufferPool) {
  int i;
  int *pag = malloc(sizeof(int)*30);

  //ve se pagina esta no buffer
  int * linha = bufferGet(RRN, bufferPool);
  if (linha != NULL) {
    for (i=0; i<30; i++) {
      pag[i] = linha[i];
    }

    return pag;
  }

  //pagina nao esta no buffer, portanto deve-se abrir o arquivo
  FILE *f = fopen(INDICE, "rb");

  //buscando pagina no arquivo e guardando em "pag"
  fseek(f, 13 + RRN*116, SEEK_SET);
  fread(pag, 4, 29, f);
  pag[bufferRRN] = RRN;

  fclose(f);

  bufferLFU(pag, bufferPool); //politica de substituicao

  return pag;
}

/*
Funcao que escreve a pagina enviada pelo usuario no bufferPool, caso esteja no buffer
ou no arquivo, caso nao esteja no buffer
*/
void atualizaPagina(int *pag, int ***bufferPool) {
  int i;

  int * linha = bufferGet(pag[bufferRRN], bufferPool);
  if (linha != NULL) {  //caso a pagina ja esteja no buffer
    for (i=0; i<30; i++) {
      linha[i] = pag[i];
    }

    free(pag);
    return;
  }

  //Pag nao esta no buffer, portanto deve-se aplicar o LFU
  bufferLFU(pag, bufferPool);
  free(pag);
}

/*
Funcao que cria uma nova pagina com o ultimo RRN
Retorna a pagina como vetor para o usuario
Coloca a pagina no buffer pool caso ela nao seja a raiz sendo criada
*/
int* novaPagina(int ***bufferPool) {
  int i;
  int ultimoRRN;
  
  FILE *f = fopen(INDICE, "wb");

  fseek(f, 9, SEEK_SET);
  fread(&ultimoRRN, 4, 1, f);

  //cria a nova pagina vazia
  int * pag = malloc(sizeof(int)*30);
  pag[0] = 0;
  for (i=1; i<29; i++) {
    pag[i] = -1;
  }
  pag[bufferRRN] = ultimoRRN;

  if (ultimoRRN != 0) {  
    /*caso nao esteja criando a primeira pagina de todas (a primeira raiz), 
    pois ela deve ser colocada em uma posicao especial, nao submetida ao LFU*/
    bufferLFU(pag, bufferPool); //coloca a nova pagina no buffer pool
  }

  escrevePagina(pag);

  //atualiza o ultimo RRN
  ultimoRRN++;
  fseek(f, 9, SEEK_SET);
  fwrite(&ultimoRRN, 4, 1, f);

  fclose(f);
  return pag;
}