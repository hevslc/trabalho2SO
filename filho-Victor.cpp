#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <bits/stdc++.h>

using namespace std;

int main()
{
  clock_t time_req;

  //Inicia o clock
  time_req = clock();

  cout << "[Filho " << getpid() << "] Vou executar o loop!" << endl;
  for(long int i = 0; i < 20000000000; i++);

  //Para o clock e calcula o tempo
  time_req = clock() - time_req;

  cout << "[Filho " << getpid() << "] Finalizei com sucesso! Tempo: " << (float)time_req/CLOCKS_PER_SEC << endl;

  exit(EXIT_SUCCESS);
}
