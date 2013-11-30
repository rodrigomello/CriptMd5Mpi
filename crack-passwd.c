#define _GNU_SOURCE
#include <string.h>
#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TAM_HASH 256
#define TAM_SENHA 4
#define N_ITENS 64

char buffer[456976][TAM_SENHA+1];
char hash[TAM_HASH];
int inicio = 0, final = 0, produziu = 0, vazio = 0;
char senha[TAM_SENHA + 1];

/* Obtém o hash a partir de uma senha e coloca o resultado em hash.
   O vetor hash deve ter pelo menos 14 elementos. */
void calcula_hash_senha(const char *senha, char *hash);
/*Gerador de senha*/
void incrementa_senha(char *senha);
/*Testador de senha*/
void testa_senha(const char *hash_alvo, const char *senha);

//Thread Produtor
void* produtor() {
  int i, aux;
  char pass[TAM_SENHA + 1];
  strcpy(pass, senha);

  for (i = 0; i < 456976; i++) {
    final = (final + 1);
    incrementa_senha(pass);
    strcpy(buffer[final], pass);
    if(i<10){
      printf("Produtor, senha = %s\n", pass); 
    }
  }
  produziu = 1;
  printf("Produção encerrada.\n");     
  return NULL;
}

//Thread Consumidor
void* consumidor() {
  int aux;
  char pass[TAM_SENHA + 1];
  printf("inicio: %d\n", inicio);
  printf("final: %d\n", final);
  final = 456976;
  while (!(vazio == 1)) {
    inicio = (inicio + 1);
    strcpy(pass, buffer[inicio]); /* Item é consumido */
    
    //printf("Consumidor, senha = %s\n", pass);
    
    if(inicio == final){
      vazio = 1;
    }else{
      vazio = 0;
    }
    testa_senha(hash, pass);
  }
  printf("Consumo encerrado.\n");     
  return NULL;
}

//Main
int main(int argc, char *argv[]) {
  int i;
  strcpy(hash, argv[1]);

  /* Limpa o buffer */
  for (i = 0; i < N_ITENS; i++)
    strcpy(buffer[i], "\n");
  //Criando a primeira senha
  for (i = 0; i < TAM_SENHA; i++) {
    senha[i] = 'a';
  }
  senha[TAM_SENHA] = '\0';
  final=final+1;
  strcpy(buffer[final], senha);

  produtor();
  consumidor();

  return 0;
}

/*Implementação das Funções*/
void testa_senha(const char *hash_alvo, const char *senha) {
  char hash_calculado[TAM_HASH + 1];

  calcula_hash_senha(senha, hash_calculado);
  if (!strcmp(hash_alvo, hash_calculado)) {
    printf("Achou! %s\n", senha);
    exit(0);
  }
}

void incrementa_senha(char *senha) {
  int i;

  i = TAM_SENHA -1;
  while (i >= 0) {
    if (senha[i] != 'z') {
      senha[i]++;
      i = -2;
    } else {
      senha[i] = 'a';
      i--;
    }
  }
}

//Abstração da funçaõ que calcula o hash
void calcula_hash_senha(const char *senha, char *hash) {
  struct crypt_data data;
  data.initialized = 0;
  strcpy(hash, crypt_r(senha, "aa", &data));
}
