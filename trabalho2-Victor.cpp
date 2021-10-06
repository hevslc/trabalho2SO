#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <list>
#include <iterator>

using namespace std;

//Quantos filhos a serem criados
#define NUM_PROC 3

//Para ativar prints extras coloque 1
#define DEBUG 0

list <pid_t> lista_filhos;
auto it = lista_filhos.begin();
pid_t filho_em_exec = -1;
int vez = 0;

//Função que cria o filho, e o faz executar o código definido por ./filho
pid_t filho()
{
  char const *args[] = {"./filho", NULL};

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

//Chamado a cada alarme
void RR(int sig)
{
  alarm(0);
  if (DEBUG) cout << "[Pai " << getpid() << "] ALARM!" << endl;
  alarm(5);

  //Iterador para percorrer a lista
  it = lista_filhos.begin();

  //Verifica qual filho está em execução, e para ele. -1 significa que todos estão, então para todos
  while(it != lista_filhos.end()) {
    if (*it == filho_em_exec || filho_em_exec == -1) {
      kill(*it, SIGSTOP);
      if (DEBUG) cout << "[Pai " << getpid() << "] Pausei: " << *it << endl;
    }
    advance(it,1);
  }

  //Variavel 'vez' diz qual filho deve executar no momento
  it = lista_filhos.begin();
  if (vez >= lista_filhos.size()) vez = 0;
  //Avança na lista 'vez' vezes
  advance(it,vez);

  //Manda continuar o próximo filho escalonado
  if (DEBUG) cout << "[Pai " << getpid() << "] Vez de: " << *it << endl;
  kill(*it, SIGCONT);
  filho_em_exec = *it;

  //Prepara o próximo a ser escalonado
  vez++;
}

int main()
{

  pid_t filho_finalizado;
  signal(SIGALRM, RR);

  if (DEBUG) cout << "[Pai " << getpid() << "] Criei os filhos: ";

  //Cria 'NUM_PROC' filhos e adiciona na lista do escalonador
  for (int i = 0; i < NUM_PROC; i++) {
    filho_finalizado = filho();
    if (DEBUG) cout << filho_finalizado << " ";
    lista_filhos.push_back(filho_finalizado);
  }
  if (DEBUG) cout << endl;

  //Ativa o Round Robin
  RR(0);

  //Aguarda os filhos finalizarem, e deleta da lista os já finalizados
  for (int i = 0; i < NUM_PROC; i++) {
    filho_finalizado = wait(NULL);
    lista_filhos.remove(filho_finalizado);
  }

  alarm(0);

  if (DEBUG) cout << "[Pai " << getpid() << "] Fim!" << endl;

  exit(EXIT_SUCCESS);
}
