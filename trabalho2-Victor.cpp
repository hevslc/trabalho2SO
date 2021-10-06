#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <list>
#include <iterator>

using namespace std;

#define NUM_PROC 3

list <pid_t> lista_filhos;
auto it = lista_filhos.begin();
pid_t filho_em_exec = -1;
int vez = 0;

pid_t filho()
{

  char const *args[] = {"./zzfilho", NULL};

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    return pid;
  } else {
    execv(args[0], const_cast<char **>(args));
    perror("execve");
    exit(EXIT_FAILURE);
  }
}

void RR(int sig)
{

  alarm(0);
  cout << "[Pai " << getpid() << "] ALARM!" << endl;
  alarm(5);
  it = lista_filhos.begin();

  while(it != lista_filhos.end()) {
    if (*it == filho_em_exec || filho_em_exec == -1) {
      kill(*it, SIGSTOP);
      cout << "[Pai " << getpid() << "] Pausei: " << *it << endl;
    }
    advance(it,1);
  }

  it = lista_filhos.begin();
  if (vez >= lista_filhos.size()) vez = 0;
  advance(it,vez);

  cout << "[Pai " << getpid() << "] Vez de: " << *it << endl;
  kill(*it, SIGCONT);
  filho_em_exec = *it;

  vez++;

}

int main()
{

  pid_t filho_finalizado;
  signal(SIGALRM, RR);


  cout << "[Pai " << getpid() << "] Criei os filhos: ";

  for (int i = 0; i < NUM_PROC; i++) {
    filho_finalizado = filho();
    cout << filho_finalizado << " ";
    lista_filhos.push_back(filho_finalizado);
  }
  cout << endl;

  RR(0);

  for (int i = 0; i < NUM_PROC; i++) {
    filho_finalizado = wait(NULL);
    lista_filhos.remove(filho_finalizado);
  }

  alarm(0);

  cout << "[Pai " << getpid() << "] Fim!" << endl;

  exit(EXIT_SUCCESS);
}
