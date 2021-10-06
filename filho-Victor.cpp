#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <bits/stdc++.h>

using namespace std;

//Para ativar prints extras coloque 1
#define DEBUG 0

int main()
{
  clock_t time_req;

  //Inicia o clock
  time_req = clock();

  if (DEBUG) cout << "[Filho " << getpid() << "] Vou executar o loop!" << endl;

  //Função principal
  for(long int i = 0; i < 20000000000; i++);

  //Para o clock e calcula o tempo
  time_req = clock() - time_req;

  cout << "[Filho " << getpid() << "] Finalizei com sucesso! Tempo: " << (float)time_req/CLOCKS_PER_SEC << " segundos" << endl;

  exit(EXIT_SUCCESS);
}
