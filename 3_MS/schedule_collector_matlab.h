#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define NEXT_TASK *(Task_list)->tasks 



#define NUMBER_OF_TASKS 10


typedef struct task {
  int id; 
  int comp_time;
  int dead_line;
}Task;


typedef struct task_list{
  int size;  // size of the task lists
  int count; // number of the tasks in the task list at an instance
  Task *tasks[NUMBER_OF_TASKS];// each individual tasks
}TaskList;

/* the list of all the tasks
* 
*
*/
TaskList taskl_ = {NUMBER_OF_TASKS, 0, {NULL}};



bool insert_TaskList(Task* task);
void min_heapify(int );
int schedule_collector(int, int, int ,int, int, int, int, int, int, int); 
void* accept_task(Task *task, TaskList* tl);
Task* get_next_task();
