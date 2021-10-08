#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

//Quantos filhos a serem criados
#define NUM_PROC 3

//Para ativar prints extras coloque 1
#define DEBUG 0

pid_t filho_em_exec = -1;

struct estrutura_filhos {
  pid_t id_filho;
  bool vivo;
  bool vez;
};

struct estrutura_filhos vetor_filhos[NUM_PROC];

//Função que cria o filho, e o faz executar o código definido por ./filho
pid_t filho()
{
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    return pid;
  } else {
    clock_t time_req;

    //Inicia o clock
    time_req = clock();

    if (DEBUG) printf("[Filho %d] Vou executar o loop!\n", getpid());

    //Função principal do filho
    for(long int i = 0; i < 20000000000; i++);

    //Para o clock e calcula o tempo
    time_req = clock() - time_req;

    printf("[Filho %d] Finalizei com sucesso! Tempo: %f segundos\n", getpid(), (float)time_req/CLOCKS_PER_SEC);

    exit(EXIT_SUCCESS);
  }
}

//Chamado a cada alarme
void RR(int sig)
{
  alarm(0);
  if (DEBUG) printf("[Pai %d] ALARME!\n", getpid());

  //-1 Significa que nenhum filho se encontra em execução no momento
  //Se esta em execucao para ele.
  if (filho_em_exec != -1) {
    kill(filho_em_exec, SIGSTOP);
    if (DEBUG) printf("[Pai %d] Pausei: %d\n", getpid(), filho_em_exec);
  }

  //Verifica quem eh o proximo a executar
  int k;
  for (k = 0; k < NUM_PROC; k++)
    if (vetor_filhos[k].vez == true) break;
  if (k == NUM_PROC) k = -1; //Eh a primeira vez, entao escolhe -1 pra dar 0 no indice
  else vetor_filhos[k].vez = false;

  k += 1;
  if (k == NUM_PROC) k = 0;

  //Verifica se esse proximo ainda esta vivo
  int j = 0;
  while(j != NUM_PROC) {
    if (vetor_filhos[j+k].vivo == true) break;
    j++;
    if (j+k == NUM_PROC) {
      k = 0;
      j = 0;
    }
  }
  //Todos os processos ja executaram
  if (j == NUM_PROC) return;

  //Manda executar o proximo filho escalonado
  vetor_filhos[j+k].vez = true;
  filho_em_exec = vetor_filhos[j+k].id_filho;
  kill(filho_em_exec, SIGCONT);

  if (DEBUG) printf("[Pai %d] Vez de: %d\n", getpid(), filho_em_exec);

  alarm(5);
}

int main()
{
  pid_t filho_finalizado;
  signal(SIGALRM, RR);

  if (DEBUG) printf("[Pai %d] Criei os filhos:", getpid());

  //Cria 'NUM_PROC' filhos e adiciona no vetor do escalonador
  for (int i = 0; i < NUM_PROC; i++) {
    filho_finalizado = filho();
    kill(filho_finalizado, SIGSTOP);
    if (DEBUG) printf(" %d", filho_finalizado);
    vetor_filhos[i].id_filho = filho_finalizado;
    vetor_filhos[i].vivo = true;
    vetor_filhos[i].vez = false;
  }
  if (DEBUG) printf("\n");

  //Ativa o Round Robin
  RR(0);

  //Aguarda os filhos finalizarem, e deleta da lista os já finalizados
  for (int i = 0; i < NUM_PROC; i++) {
    filho_finalizado = wait(NULL);
    for (int j = 0; j < NUM_PROC; j++) {
      if (vetor_filhos[j].id_filho == filho_finalizado)
        vetor_filhos[j].vivo = false;
        break;
    }
  }

  alarm(0);

  if (DEBUG) printf("[Pai %d] FIM!\n", getpid());

  exit(EXIT_SUCCESS);
}
