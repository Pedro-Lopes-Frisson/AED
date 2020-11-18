////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// AED, 2020/2021
//
// TODO: place the student number and name here
// TODO: place the student number and name here (if applicable)
// TODO: place the student number and name here (if applicable)
//
// Brute-force solution of the generalized weighted job selection problem
//
// Compile with "cc -Wall -O2 job_selection.c -lm" or equivalent
//
// In the generalized weighted job selection problem we will solve here we have T programming tasks and P programmers.
// Each programming task has a starting date (an integer), an ending date (another integer), and a profit (yet another
// integer). Each programming task can be either left undone or it can be done by a single programmer. At any given
// date each programmer can be either idle or it can be working on a programming task. The goal is to select the
// programming tasks that generate the largest profit.
//
// Things to do:
//   0. (mandatory)
//      Place the student numbers and names at the top of this file.
//   1. (highly recommended)
//      Read and understand this code.
//   2. (mandatory)
//      Solve the problem for each student number of the group and for
//        N=1, 2, ..., as higher as you can get and
//        P=1, 2, ... min(8,N)
//      Present the best profits in a table (one table per student number).
//      Present all execution times in a graph (use a different color for the times of each student number).
//      Draw the solutions for the highest N you were able to do.
//   3. (optional)
//      Ignore the profits (or, what is the same, make all profits equal); what is the largest number of programming
//      tasks that can be done?
//   4. (optional)
//      Count the number of valid task assignments. Calculate and display an histogram of the number of occurrences of
//      each total profit. Does it follow approximately a normal distribution?
//   5. (optional)
//      Try to improve the execution time of the program (use the branch-and-bound technique).
//      Can you use divide and conquer to solve this problem?
//      Can you use dynamic programming to solve this problem?
//   6. (optional)
//      For each problem size, and each student number of the group, generate one million (or more!) valid random
//      assignments and compute the best solution found in this way. Compare these solutions with the ones found in
//      item 2.
//   7. (optional)
//      Surprise us, by doing something more!
//   8. (mandatory)
//      Write a report explaining what you did. Do not forget to put all your code in an appendix.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../P02/elapsed_time.h"
#define pBusy printf("{%d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",p->busy[0], p->busy[1], p->busy[2], p->busy[3], p->busy[4], p->busy[5], p->busy[6], p->busy[7], p->busy[8], p->busy[9]);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Random number generator interface (do not change anything in this code section)
//
// In order to ensure reproducible results on Windows and GNU/Linux, we use a good random number generator, available at
//   https://www-cs-faculty.stanford.edu/~knuth/programs/rng.c
// This file has to be used without any modifications, so we take care of the main function that is there by applying
// some C preprocessor tricks
//

#define main  rng_main                        // main gets replaced by rng_main
#ifdef __GNUC__
int rng_main() __attribute__((__unused__));   // gcc will not complain if rnd_main() is not used
#endif
#include "rng.c"
#undef main                                   // main becomes main again

#define srandom(seed)  ran_start((long)seed)  // start the pseudo-random number generator
#define random()       ran_arr_next()         // get the next pseudo-random number (0 to 2^30-1)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// problem data (if necessary, add new data fields in the structures; do not change anything else in this code section)
//
// on the data structures declared below, a comment starting with
// * a I means that the corresponding field is initialized by init_problem()
// * a S means that the corresponding field should be used when trying all possible cases
// * IS means both (part initialized, part used)
//

#if 1

#define MAX_T  64  // maximum number of programming tasks
#define MAX_P  10  // maximum number of programmers

typedef struct
{
  int starting_date;      // I starting date of this task
  int ending_date;        // I ending date of this task
  int profit;             // I the profit if this task is performed
  int assigned_to;        // S current programmer number this task is assigned to (use -1 for no assignment)
  int best_assigned_to;        // S current programmer number this task is assigned to (use -1 for no assignment)
}
task_t;

typedef struct
{
  int NMec;               // I  student number
  int T;                  // I  number of tasks
  int P;                  // I  number of programmers
  int I;                  // I  if 1, ignore profits
  long int nAlt;
  int nn;
  int total_profit;       // S  current total profit
  int biggest_profit;       // S  current total profit
  int lastAssigned_T;
  double cpu_time;        // S  time it took to find the solution
  task_t task[MAX_T];     // IS task data
  int busy[MAX_P];        // S  for each programmer, record until when she/he is busy (-1 means idle)
  int lastBusy[MAX_P];        // S  for each programmer, record until when she/he is busy (-1 means idle)
  char dir_name[16];      // I  directory name where the solution file will be created
  char file_name[64];     // I  file name where the solution data will be stored
}
problem_t;

////////////////////////7777777777777777777777777777777777777777777777
// recursEE
//
int recurse(problem_t *p , int t) { // p e o pointer para o a coisa do problema t e o indice de tasks
  int *busy = p->busy;
  int *lastBusy = p->lastBusy;
  int t_profit = p->total_profit;
  int viable = 0;
  task_t *tasksp = p->task;
#if 1
  if (!( tasksp[t].starting_date < tasksp[p->T - 1].starting_date)){
    p->nAlt++;
    pBusy;
    puts("nao falhou");
    return 1;
  }
#endif
  for( int i = 0; i < p->P; p++){
 // Verificar se o programador i esta disponivel para fazer a task t
    if(busy[i] < (tasksp[t].starting_date)){
 // Verificar se a task t esta disponivel
      if(tasksp[t].assigned_to < 0){
        // incluir a task t
        lastBusy[i] = busy[i];
        busy[i] = tasksp[t].ending_date;
        t_profit += tasksp[t].profit;
        tasksp[t].assigned_to = i;
        viable = recurse(p,t + 1);

        if( t_profit > p->biggest_profit && viable ){
          p->biggest_profit = t_profit;
          for(int j = 0; j < p->T; j++){
            tasksp[j].best_assigned_to = tasksp[j].assigned_to;
          }//end for
        }  // end if profit e not assign

        // não incluir a task t
        busy[i] = -1;
        t_profit -= tasksp[t].profit;
        tasksp[t].assigned_to = -1;
        recurse(p,t + 1);

        if( t_profit > p->biggest_profit && viable ){
          p->biggest_profit = t_profit;
          for(int j = 0; j < p->T; j++){
            tasksp[j].best_assigned_to = tasksp[j].assigned_to;
          } //end for
        }   // end if profit e not assign

      }else{ // else do if task assign
        busy[tasksp[t].assigned_to] = lastBusy[tasksp[t].assigned_to];
        busy[i] = tasksp[t].ending_date;
        tasksp[t].assigned_to = i;
        viable = recurse(p,t + 1);

        if( t_profit > p->biggest_profit && viable ){
          p->biggest_profit = t_profit;
          for(int j = 0; j < p->T; j++){
            tasksp[j].best_assigned_to = tasksp[j].assigned_to;
          }//end for
        }  // end if profit e not assign
      }
    }
  }
  pBusy;
  puts("falhou");
  return 0;

}
#if 0
int recurse(problem_t *p , int t) { // p e o pointer para o a coisa do problema t e o indice de tasks
  // temos de encontrar duas coisas
  // o caso terminal que e terminal por que a escolhas das tasks
  // t[T-1].starting_date  caso para solucao viavel
  task_t *tasksp = p->task;
  int *busy = p->busy;
  int *lastBusy = p->lastBusy;
  int t_profit = p->total_profit;
  int viable;
  if ( tasksp[t].starting_date < tasksp[p->T - 1].starting_date){
    p->nAlt++;
    pBusy;
    puts("nao falhou");
    return 1;
  }
  for( int i = 0; i > p->P; p++){
 // Verificar se o programador i esta disponivel para fazer a task t
    if(busy[i] < (tasksp[t].starting_date)){
 // Verificar se a task t esta disponivel
      if(tasksp[t].assigned_to < 0){
        // incluir a task t
        lastBusy[i] = busy[i];
        busy[i] = tasksp[t].ending_date;
        t_profit += tasksp[t].profit;
        tasksp[t].assigned_to = i;
        viable = recurse(p,t + 1);

        if( t_profit > p->biggest_profit && viable ){
          p->biggest_profit = t_profit;
          for(int j = 0; j < p->T; j++){
            tasksp[j].best_assigned_to = tasksp[j].assigned_to;
          }//end for
        }  // end if profit e not assign

        // não incluir a task t
        busy[i] = -1;
        t_profit -= tasksp[t].profit;
        tasksp[t].assigned_to = -1;
        recurse(p,t + 1);

        if( t_profit > p->biggest_profit && viable ){
          p->biggest_profit = t_profit;
          for(int j = 0; j < p->T; j++){
            tasksp[j].best_assigned_to = tasksp[j].assigned_to;
          } //end for
        }   // end if profit e not assign

      }else{ // else do if task assign
        busy[tasksp[t].assigned_to] = lastBusy[tasksp[t].assigned_to];
        busy[i] = tasksp[t].ending_date;
        tasksp[t].assigned_to = i;
        viable = recurse(p,t + 1);

        if( t_profit > p->biggest_profit && viable ){
          p->biggest_profit = t_profit;
          for(int j = 0; j < p->T; j++){
            tasksp[j].best_assigned_to = tasksp[j].assigned_to;
          }//end for
        }  // end if profit e not assign

      }
    }
  pBusy;
  puts("falhou");
  return 0;

}
#endif
#if 0
int recurse(problem_t *p , int t) {
  static int fts = 0; // da me a proxima task tipo inicial algo do genero se cheagar aqui uma vez mando a task 1 para avalaiar se posso ou nao meter ou se devo trocar se chegar duas mando a task 2
  // se ultrapassa e um dead end entao tenho que  retirar a ultima task que meti e tentar meter outra// se ultrapassa e um dead end entao tenho que  retirar a ultima task que meti e tentar meter outra
  pBusy;
  if( t > p->T){ // verifica se t utltrapassa T
    // se ultrapassa e um dead end entao tenho que  retirar a ultima task que meti e tentar meter outra
    p->busy[p->task[p->lastAssigned_T].assigned_to]= -1;      //
    p->total_profit -= p->task[p->lastAssigned_T].profit;     //  processo para considerar que a task foi atribuida
    p->task[p->lastAssigned_T].assigned_to = -1;               //
    p->lastAssigned_T = fts + 1;
    printf(" %d \n",p->lastAssigned_T);
    return recurse(p, ++fts);
  }
  for(int i = 0; i < p->P; i++){ // for loop no arrray dos programmers
    if(p->busy[i] < (p->task[t].starting_date)){ // verifica se o programador i esta disponivel
      if(p->task[t].assigned_to <0){ // se a task t estiver disponivel atribuo a i
        puts("i != p->P");                        // visual debug inside  the debugger
        p->busy[i] = p->task[t].ending_date;      //
        p->total_profit += p->task[t].profit;     //  processo para considerar que a task foi atribuida
        p->task[t].assigned_to = i;               //
        p->lastAssigned_T = t;
        recurse(p, ++t);
    }else{ //se nao estiver retiro a task do programador que a esta a fazer e dou a task a outro
      // mas preciso de guardar o last busy de cada programador para desfazer o que ja fiz
        p->busy[i] = p->task[t].ending_date;      //
        p->total_profit += p->task[t].profit;     //  processo para considerar que a task foi atribuida
        p->task[t].assigned_to = i;               //
        p->lastAssigned_T = t;
    }
   }
                                                // se estiver meto a task se nao tiver tiro a task em que ele esta
  }
  p->nAlt++;
  return recurse(p, ++t);
}
#endif
#if 0
int recurse(problem_t *p , int t) {
  if( t > p->T){
    return 1;
  }
  for(int i = 0; i < p->P; i++){ // for loop no arrray dos programmers
    if(p->busy[i] < (p->task[t].starting_date)){ // verifica se o programador i esta disp.
        p->busy[i] = p->task[t].ending_date; //muda a disp
        p->total_profit += p->task[t].profit; // aumenta a var profit do prob
        p->task[t].assigned_to = i; // diz que a task t esta assigned a alguem
        recurse(p, ++t);
        puts("fim 1 recurse");
        pBusy;
        p->busy[i] = -1;                      //
        p->total_profit -= p->task[t].profit; // reverte o que se faz anteriormente para considerar o caso em que nao se considera a task t para este prog
        p->task[t].assigned_to = -1;          //
        recurse(p, ++t);                      // recurse
        puts("fim 2 recurse");
        pBusy;
     }else{
        recurse(p,++t);                       // idk
        puts("fim 3 recurse");
        pBusy;
     }
   }
  p->nAlt++;
  return 0;
}
#endif

int compare_tasks(const void *t1,const void *t2)
{
  int d1,d2;

  d1 = ((task_t *)t1)->starting_date;
  d2 = ((task_t *)t2)->starting_date;
  if(d1 != d2)
    return (d1 < d2) ? -1 : +1;
  d1 = ((task_t *)t1)->ending_date;
  d2 = ((task_t *)t2)->ending_date;
  if(d1 != d2)
    return (d1 < d2) ? -1 : +1;
  return 0;
}

void init_problem(int NMec,int T,int P,int ignore_profit,problem_t *problem)
{
  int i,r,scale,span,total_span;
  int *weight;

  //
  // input validation
  //
  if(NMec < 1 || NMec > 999999)
  {
    fprintf(stderr,"Bad NMec (1 <= NMex (%d) <= 999999)\n",NMec);
    exit(1);
  }
  if(T < 1 || T > MAX_T)
  {
    fprintf(stderr,"Bad T (1 <= T (%d) <= %d)\n",T,MAX_T);
    exit(1);
  }
  if(P < 1 || P > MAX_P)
  {
    fprintf(stderr,"Bad P (1 <= P (%d) <= %d)\n",P,MAX_P);
    exit(1);
  }
  //
  // the starting and ending dates of each task satisfy 0 <= starting_date <= ending_date <= total_span
  //
  total_span = (10 * T + P - 1) / P;
  if(total_span < 30)
    total_span = 30;
  //
  // probability of each possible task duration
  //
  // task span relative probabilities
  //
  // |  0  0  4  6  8 10 12 14 16 18 | 20 | 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1 | smaller than 1
  // |  0  0  2  3  4  5  6  7  8  9 | 10 | 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 | 30 31 ... span
  //
  weight = (int *)alloca((size_t)(total_span + 1) * sizeof(int)); // allocate memory (freed automatically)
  if(weight == NULL)
  {
    fprintf(stderr,"Strange! Unable to allocate memory\n");
    exit(1);
  }
#define sum1  (298.0)                      // sum of weight[i] for i=2,...,29 using the data given in the comment above
#define sum2  ((double)(total_span - 29))  // sum of weight[i] for i=30,...,data_span using a weight of 1
#define tail  100
  scale = (int)ceil((double)tail * 10.0 * sum2 / sum1); // we want that scale*sum1 >= 10*tail*sum2, so that large task
  if(scale < tail)                                      // durations occur 10% of the time
    scale = tail;
  weight[0] = 0;
  weight[1] = 0;
  for(i = 2;i <= 10;i++)
    weight[i] = scale * (2 * i);
  for(i = 11;i <= 29;i++)
    weight[i] = scale * (30 - i);
  for(i = 30;i <= total_span;i++)
    weight[i] = tail;
#undef sum1
#undef sum2
#undef tail
  //
  // accumulate the weigths (cummulative distribution)
  //
  for(i = 1;i <= total_span;i++)
    weight[i] += weight[i - 1];
  //
  // generate the random tasks
  //
  srandom(NMec + 314161 * T + 271829 * P);
  problem->NMec = NMec;
  problem->T = T;
  problem->P = P;
  problem->I = (ignore_profit == 0) ? 0 : 1;
  for(i = 0;i < T;i++)
  {
    //
    // task starting an ending dates
    //
    r = 1 + (int)random() % weight[total_span]; // 1 .. weight[total_span]
    for(span = 0;span < total_span;span++)
      if(r <= weight[span])
        break;
    problem->task[i].starting_date = (int)random() % (total_span - span + 1);
    problem->task[i].ending_date = problem->task[i].starting_date + span - 1;
    //
    // task profit
    //
    // the task profit is given by r*task_span, where r is a random variable in the range 50..300 with a probability
    //   density function with shape (two triangles, the area of the second is 4 times the area of the first)
    //
    //      *
    //     /|   *
    //    / |       *
    //   /  |           *
    //  *---*---------------*
    // 50 100 150 200 250 300
    //
    scale = (int)random() % 12501; // almost uniformly distributed in 0..12500
    if(scale <= 2500)
      problem->task[i].profit = 1 + round((double)span * (50.0 + sqrt((double)scale)));
    else
      problem->task[i].profit = 1 + round((double)span * (300.0 - 2.0 * sqrt((double)(12500 - scale))));
  }
  //
  // sort the tasks by the starting date
  //
  qsort((void *)&problem->task[0],(size_t)problem->T,sizeof(problem->task[0]),compare_tasks);
  //
  // finish
  //
  if(problem->I != 0)
    for(i = 0;i < problem->T;i++)
      problem->task[i].profit = 1;
#define DIR_NAME  problem->dir_name
  if(snprintf(DIR_NAME,sizeof(DIR_NAME),"%06d",NMec) >= sizeof(DIR_NAME))
  {
    fprintf(stderr,"Directory name too large!\n");
    exit(1);
  }
#undef DIR_NAME
#define FILE_NAME  problem->file_name
  if(snprintf(FILE_NAME,sizeof(FILE_NAME),"%06d/%02d_%02d_%d.txt",NMec,T,P,problem->I) >= sizeof(FILE_NAME))
  {
    fprintf(stderr,"File name too large!\n");
    exit(1);
  }
#undef FILE_NAME
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// problem solution (place your solution here)
//

#if 1

static void solve(problem_t *problem)
{
  FILE *fp;
  int i;

  //
  // open log file
  //
  (void)mkdir(problem->dir_name,S_IRUSR | S_IWUSR | S_IXUSR);
  fp = fopen(problem->file_name,"w");
  if(fp == NULL)
  {
    fprintf(stderr,"Unable to create file %s (maybe it already exists? If so, delete it!)\n",problem->file_name);
    exit(1);
  }
  //
  // solve
  //
  problem->cpu_time = cpu_time();
  recurse(problem,0);
    // call your (recursive?) function to solve the problem here
  problem->cpu_time = cpu_time() - problem->cpu_time;
  //
  // save solution data
  //
  fprintf(fp,"NMec = %d\n",problem->NMec);
  fprintf(fp,"T = %d\n",problem->T);
  fprintf(fp,"P = %d\n",problem->P);
  fprintf(fp,"Profits%s ignored\n",(problem->I == 0) ? " not" : "");
  fprintf(fp,"Solution time = %.3e\n",problem->cpu_time);
  fprintf(fp,"Task data\n");
#define TASK  problem->task[i]
  for(i = 0;i < problem->T;i++)
    fprintf(fp,"  %3d %3d %5d\n",TASK.starting_date,TASK.ending_date,TASK.profit);
#undef TASK
  fprintf(fp,"End\n");
  //
  // terminate
  //
  if(fflush(fp) != 0 || ferror(fp) != 0 || fclose(fp) != 0)
  {
    fprintf(stderr,"Error while writing data to file %s\n",problem->file_name);
    exit(1);
  }
}

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// main program
//

int main(int argc,char **argv)
{
  problem_t problem;
  int NMec,T,P,I;

  NMec = (argc < 2) ? 2020 : atoi(argv[1]);
  T = (argc < 3) ? 5 : atoi(argv[2]);
  P = (argc < 4) ? 2 : atoi(argv[3]);
  I = (argc < 5) ? 0 : atoi(argv[4]);
  init_problem(NMec,T,P,I,&problem);
  solve(&problem);
  problem.nAlt = 0;
  printf(" nAlt %ld\n",problem.nAlt);
  return 0;
}