#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/wait.h>
#include <time.h>

# define QUANTUM 5   // Quantum = 5s
int pid_run; 
int pid_filhos[3];
int ind = 0;
int erro = 0;
/** 
 * * Os alunos devem implementar um escalonador round-robin (RR) em modo usuário da seguinte maneira:
 * * O processo pai:
 * * a) cria três processos filhos e imediatamente pára a execução dos filhos;
 * * b) O processo pai implementa um escalonador RR com quantum de 5 segundos. 
 * * Atenção: em um dado momento, deve haver somente um processo no estado R.
 * * c) Após o término da execução dos três processos filhos, o processo pai termina a sua execução.
 * * 
 * * Os processos filho:
 * * a) Vao executar o seguinte código:
 * * ---
 * *    long int i;
 * *        for(i=0; i<20000000000; i++);/* busy waiting com 100% de CPU – demora 25s
 * * ---
 * * b)    Ao término do “for”, o processo filho imprime o seu pid e o tempo de execução. Depois disso, termina a execução.
*/

void escalona()
{ 
   kill (pid_filhos[ind],SIGSTOP);
   printf("processo pid = %d PARADO\n", pid_filhos[ind]);

   ind++;
   if(ind == 2)
      ind = 0;

   if(kill (pid_filhos[ind],SIGCONT) < 0)
      erro++;
   printf("processo pid = %d CONTINUA\n", pid_filhos[ind]);

   if(erro == 3)
      exit(0);
   
   alarm(5);
}

int main()
{
   int pid, estado;
   
   //Processo pai
   //! Cria 3 processos filhos 
   for(int i = 0; i < 3; i++)
   {
         pid = fork();                 //! Faz a chamada de sistema fork() e coloca seu retorno em pid
         if (pid == 0)
         {
            kill (getpid(),SIGSTOP);
            break;                     //! Se pid == 0 é o processo filho e dá um break
         }   
      pid_filhos[i] = pid;                       
   }


   //Processos filho
   if(pid == 0)
   {

      clock_t begin = clock();

      long int i;
      for(i=0; i<20000000000; i++); // busy waiting com 100% de CPU – demora 25s

      clock_t end = clock();
      double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

      pid_run = getpid();

      printf("sou o processo pid = %d\n", getpid());
      printf("tempo de execucao = %f\n", time_spent);

      exit(0);    //! Termina a execução do processo filho
   }

   //! Implementa um escalonador RR com quantum de 5 segundos.
   signal(SIGALRM, escalona); 
   alarm(5);

   //Processo pai
   for(int j = 0; j < 3; j++)
      wait(&estado);
   exit(0);    //! Termina a execução do processo pai
}