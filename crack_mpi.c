/*
Compilar:  mpicc crack_mpi.c -o mpi.app -lcrypt
Rodar:  mpiexec -n <numero de processos> ./mpi.app <hash md5>
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <crypt.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>


#define TAM_HASH 256
#define TAM_SENHA 4
#define TAM_POSSIBILIDADES (int)pow(26, TAM_SENHA)

/* Obtém o hash a partir de uma senha e coloca o resultado em hash.
   O vetor hash deve ter pelo menos 14 elementos. */
void calcula_hash_senha(const char *senha, char *hash);

void incrementa_senha(char *senha);
void testa_senha(const char *hash_alvo, const char *senha);

int main(int argc, char *argv[]) {
  int i;
  char senha[TAM_SENHA + 1];
  char **buffer, **buffer_local;
  int my_rank, comm_sz;
  int n_local;
  MPI_Comm comm = MPI_COMM_WORLD;
  if (argc != 2) {
    printf("Uso: %s <hash>", argv[0]);
    return 1;
  }
  MPI_Init(NULL,NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if(my_rank == 0){
    //aloca a quantidade de possibilidades no buffer
    buffer = (char **) calloc( TAM_POSSIBILIDADES, sizeof(char *));
    
    //aloca o primeiro vetor e o preenche com a primeira senha
    buffer[0] = (char *) calloc(TAM_SENHA + 1, sizeof(char *));
    for(i = 0; i< TAM_SENHA; i++){
      buffer[0][i] = 'a';
    }

    buffer[0][TAM_SENHA] = '\0';
    
    strcpy(senha, buffer[0]);

    //aloca os outros vetores enquanto preenche com o restante das senhas
    for (i = 1; i < TAM_POSSIBILIDADES + (TAM_POSSIBILIDADES % comm_sz); i++){
      buffer[i] = (char *) calloc(TAM_SENHA + 1 , sizeof(char));
      incrementa_senha(senha);
      strcpy(buffer[i], senha);
    }
    /*
    for (i = 0; i < TAM_POSSIBILIDADES; i++){
      if(i%1000 == 0){
        printf("BUffer[%d]: %s\n",i ,buffer[i] );
      }
    }
    */
    printf("Primeiro: %s\n", buffer[0]);
    printf("Ultimo: %s\n",buffer[TAM_POSSIBILIDADES-1]);

  }
  
  n_local = TAM_POSSIBILIDADES/comm_sz; 
  if (TAM_POSSIBILIDADES % comm_sz){
    n_local += comm_sz;
  }
  buffer_local = (char **) calloc(n_local, sizeof(char *));
  for (i = 0; i < n_local; i++){
      buffer_local[i] = (char *) calloc(TAM_SENHA + 1, sizeof(char));
      //strcpy(buffer[i], senha);
  }  
  
  MPI_Scatter(buffer, n_local, MPI_BYTE, buffer_local, n_local, MPI_BYTE, 0, comm);
  //MPI_Bcast(buffer, TAM_POSSIBILIDADES + (TAM_POSSIBILIDADES % comm_sz), MPI_BYTE, 0, comm );
  //printf("N_local: %d\n",n_local );
  printf("Buffer[%d]: %d\n",n_local ,buffer_local[n_local-1] );
  
  MPI_Barrier(comm);
/*
  if(my_rank == 0){
  for (i = 0; i < n_local; i++){
      if(i%1000 == 0){
        printf("Buffer[%d]: %s\n",i ,buffer_local[i] );
      }
    }
  }
 
  strcpy(senha, buffer_local[0]);
  i = 0;
  while (i < n_local) {
    testa_senha(argv[1], senha);
    i++;
    if(i%10000 == 0){
      printf("processo: %d, test: %s, i: %d\n",my_rank, buffer_local[i], i);
    }
    strcpy(senha, buffer_local[i]);
  }
*/

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
}


void calcula_hash_senha(const char *senha, char *hash) {
  struct crypt_data data;
  data.initialized = 0;
  strcpy(hash, crypt_r(senha, "aa", &data));
}