#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>

#define NPROCESS  3
#define QUANTUM  5.0 // seconds

// linked list manipulation functions _______________________________________________________ 
typedef struct pidstruct{ // linked list to contain the pids of the created child processes
	int pid; 
	struct pidstruct *next; 
} childpid;

void removeNext(childpid *p){
   childpid *trash;
   trash = p->next;
   p->next = trash->next;
   free(trash);
}

void insert(int pid, childpid *p){
  childpid *newchild;
  newchild = malloc(sizeof(childpid));
  newchild->pid = pid;
  newchild->next = p->next;
  p->next = newchild;
}

// process manipulation functions _______________________________________________________ 
void resumeProcess(int pid){
  if(kill(pid, SIGCONT) < 0){
    printf("Falha em SIGCONT no processo filho %d\n", getpid()); 
    exit(1);
  }
}

void stopProcess(int pid){
  if(kill(pid, SIGSTOP) < 0){
    printf("Falha em SIGSTOP no processo filho %d\n", getpid()); 
    exit(1);
  }
}

// function to create a process and then stop it
int createAndStop(){
  int pid;
  if((pid = fork()) < 0){
    printf("erro no fork\n");
    exit(1);
  }
  if(pid != 0) stopProcess(pid);
  return pid; // returns the pid of the child or the parent
}

// recursive function to create and stop processes
int createStopRecur(int child, childpid *pidlist){
  int pid = createAndStop();
  int curpid = 0;
  child++;

  if(pid != 0){ // if father
    curpid = getpid(); 
    insert(pid, pidlist);
    if(child < NPROCESS){
      curpid = createStopRecur(child, pidlist->next);
    }
  }
  return curpid; // if it is parent, the if block was executed and therefore curpid = parent's pid, else curpid = 0 (child)
}

double getTime(clock_t t){
  int t2 = clock() - t;
  return ((double)t2)/CLOCKS_PER_SEC;
}
//_____________________________________________________________________________________

int main(){
  int child = 0;
  int len = NPROCESS;
  int status;

  childpid *pidlist;
  pidlist = malloc(sizeof(childpid));
  pidlist->next = NULL;

  int ppid = createStopRecur(child, pidlist);

  clock_t te = clock();

  // father process execution block
  if(getpid() == ppid){
    while(len != 0){                // while there are processes in the list
      childpid *previous = pidlist; // keeps pointer of the previous item
      for(childpid *p = pidlist->next; p != NULL; p = p->next){
        clock_t t = clock(); 
        int ret;

        resumeProcess(p->pid);

        while(ret = waitpid(p->pid, &status, WNOHANG) == 0 && (getTime(t)) < QUANTUM);
        
        if(ret = waitpid(p->pid, &status, WNOHANG) == 0) // quantum, but the process is not over yet
          stopProcess(p->pid); 

        else if(WIFEXITED(status)){ // process ended
          removeNext(previous);
          p = previous;
          len--;
        }
        previous = p;
      }   
    }
  }

  // child processes execution block
  else{
    for(long int i=0; i<20000000000; i++); 
    printf("pid: %d runtime: %f \n", getpid(), getTime(te));
  }

  return 0;
}
   