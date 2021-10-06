#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

#define number_of_sons 3

int sons[number_of_sons];
int last_son = 0;
int state;
int errors_on_creating_sons = 0;

void on_alarm()
{
    kill(sons[last_son], SIGSTOP);
    last_son++;
    if (last_son >= number_of_sons)
    {
        last_son = 0;
    }
    printf("\nExecuting son %d", sons[last_son]);
    alarm(5);
    int res = kill(sons[last_son], SIGCONT);
    if (res < 0)
    {
        printf("\nProblem trying to make son execute");
        errors_on_creating_sons++;
        if (errors_on_creating_sons >= number_of_sons)
        {
            exit(0);
        }
    }
}

void do_what_sons_do()
{
    printf("\nstarting son %d", getpid());
    long i;
    for(i = 0; i < 20000000000; i++) {}
    exit(0);
}

void do_what_fathers_do()
{
    signal(SIGALRM, on_alarm);
    kill(getpid(), SIGALRM);
    for (int i = 0; i < number_of_sons; i++)
    {
        wait(&state);
    }
}

int main(int argc, char const *argv[])
{
    // Creating the sons processes
    int pid;
    for (int i = 0; i < number_of_sons; i++)
    {
        pid = fork();
        if (pid > 0)
        {
            sons[i] = pid;
        }
        else
        {
            break;
        }
    }

    // This is a son
    if (pid == 0)
    {
        kill(getpid(), SIGSTOP);
        do_what_sons_do();
        exit(0);
    }
    else if(pid < 0)
    {
        printf("\nCoudn't create the son");
        exit(0);
    }

    // Luke, I'm your father process
    do_what_fathers_do();
    exit(0);
}
