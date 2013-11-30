#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <crypt.h>
#include <stdlib.h>

#define TAM_HASH 256
#define TAM_SENHA 4

/* Obtém o hash a partir de uma senha e coloca o resultado em hash.
   O vetor hash deve ter pelo menos 14 elementos. */
void calcula_hash_senha(const char *senha, char *hash);

void incrementa_senha(char *senha);
void testa_senha(const char *hash_alvo, const char *senha);

int main(int argc, char *argv[]) {
  int i;
  char senha[TAM_SENHA + 1];

  if (argc != 2) {
    printf("Uso: %s <hash>", argv[0]);
    return 1;
  }

  for (i = 0; i < TAM_SENHA; i++) {
    senha[i] = 'a';
  }
  senha[TAM_SENHA] = '\0';

  while (1) {
    testa_senha(argv[1], senha);
    incrementa_senha(senha);
  }
  return 0;
}

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

  i = TAM_SENHA - 1;
  while (i >= 0) {
    if (senha[i] != 'z') {
      senha[i]++;
      i = -2;
    } else {
      senha[i] = 'a';
      i--;
    }
  }
  if (i == -1) {
    printf("Não achou!\n");
    exit(1);
  }
}


void calcula_hash_senha(const char *senha, char *hash) {
  struct crypt_data data;
  data.initialized = 0;
  strcpy(hash, crypt_r(senha, "aa", &data));
}