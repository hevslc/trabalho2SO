# Trabalho Prático 2 - Sistemas Operacionais - 2021/1


Implementar um escalonador round-robin (RR) em modo usuário da seguinte maneira:

O processo pai:

a)    cria três processos filhos e imediatamente pára a execução dos filhos;

b)    O processo pai implementa um escalonador RR com quantum de 5 segundos. Atenção: em um dado momento, deve haver somente um processo no estado R.

c)     Após o término da execução dos três processos filhos, o processo pai termina a sua execução.

Os processos filho:

a)    Vao executar o seguinte código:

---

long int i;

   for(i=0; i<20000000000; i++);/* busy waiting com 100% de CPU – demora 25s */

---

b)    Ao término do “for”, o processo filho imprime o seu pid e o tempo de execução. Depois disso, termina a execução.
