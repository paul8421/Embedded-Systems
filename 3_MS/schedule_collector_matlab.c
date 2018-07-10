
#include "schedule_collector.h"
/*
* inserts tasks into the task list and increments the count of number of 
* tasks by one
*/
bool insert_TaskList(Task* task)
{
  int c = taskl_.count; 
  if(taskl_.count >= taskl_.size)
    return 0;
  taskl_.tasks[c] = task;
  taskl_.count += 1;
  return 1;
}
/*
* creates a min heap with the minimal element on the top 
*/
void min_heapify(int i)
{
  Task *temp;
  if( i < 0 || (taskl_.count-i) <= 1 || i > (taskl_.count/2)-1)
    return;
  else
    {
    if(taskl_.tasks[i]->dead_line > taskl_.tasks[2*i + 1]->dead_line){
     temp = taskl_.tasks[i];
     taskl_.tasks[i] = taskl_.tasks[2*i + 1];
     taskl_.tasks[2*i + 1] = temp;
      min_heapify(2*i + 1);
    }
    /*there exists no second child for the bottom left most element is the number of nodes are even*/
    if( !(taskl_.count % 2) && i == (taskl_.count/2)-1)
     {if(!i) return; min_heapify(--i);}
    if(taskl_.tasks[i]->dead_line > taskl_.tasks[2*i + 2]->dead_line){
     temp = taskl_.tasks[i];
     taskl_.tasks[i] = taskl_.tasks[2*i + 2];
     taskl_.tasks[2*i + 2] = temp;
      min_heapify(2*i + 2);
    }
  }
  min_heapify(--i);
}
/*
* returns the next task with least dead line next 
*/
int schedule_collector(int ct_t1, int dl_t1, int ct_t2, int dl_t2, int ct_t3, int dl_t3,int ct_t4, int dl_t4, int ct_t5, int dl_t5)
{
  
  int i = 0;

  /* adds all the tasks to the task_list at each
  *  task_list and return the next task to be executed
  */
  i += 1;
  taskl_.tasks[i] = malloc(sizeof(Task));
  taskl_.tasks[i]->id = i;

  if(ct_t1 == 0)
    taskl_.tasks[i]->dead_line = 9999;
  else 
    taskl_.tasks[i]->dead_line = dl_t1;

  taskl_.tasks[i]->comp_time= ct_t1;
  accept_task(taskl_.tasks[i], &taskl_);
  
  i += 1;
  
  taskl_.tasks[i] = malloc(sizeof(Task));
  taskl_.tasks[i]->id = i;

  if(ct_t2 == 0)
    taskl_.tasks[i]->dead_line = 9999;
  else 
    taskl_.tasks[i]->dead_line = dl_t2;

  taskl_.tasks[i]->comp_time= ct_t2;
  accept_task(taskl_.tasks[i], &taskl_);

  i += 1;

  taskl_.tasks[i] = malloc(sizeof(Task));
  taskl_.tasks[i]->id = i;
  if(ct_t3 == 0)
    taskl_.tasks[i]->dead_line = 9999;
  else 
    taskl_.tasks[i]->dead_line = dl_t3;

  taskl_.tasks[i]->comp_time= ct_t3;
  accept_task(taskl_.tasks[i], &taskl_);

  i += 1;

  taskl_.tasks[i] = malloc(sizeof(Task));
  taskl_.tasks[i]->id = i;
  if(ct_t4 == 0)
    taskl_.tasks[i]->dead_line = 9999;
  else 
    taskl_.tasks[i]->dead_line = dl_t4;

  taskl_.tasks[i]->comp_time= ct_t4;
  accept_task(taskl_.tasks[i], &taskl_);

  i += 1;

  taskl_.tasks[i] = malloc(sizeof(Task));
  taskl_.tasks[i]->id = i;
  if(ct_t5 == 0)
    taskl_.tasks[i]->dead_line = 9999;
  else 
    taskl_.tasks[i]->dead_line = dl_t5;

  taskl_.tasks[i]->comp_time= ct_t5;
  accept_task(taskl_.tasks[i], &taskl_);

  return get_next_task()->id;
}
/*
* adds the task to the task list 
*/
void* accept_task(Task *task, TaskList* tl)
{
  insert_TaskList(task); 
  min_heapify((taskl_.count/2)-1);

}
/*
* returns the root element from the heap, which is the next task 
* to be scheduled 
*/

Task* get_next_task()
{
  Task *temp;
  Task *next_task = *(taskl_.tasks);
  temp = taskl_.tasks[taskl_.count-1];
  taskl_.tasks[taskl_.count-1] = taskl_.tasks[0];
  taskl_.tasks[0] = temp;
  taskl_.count -= 1;
  min_heapify((taskl_.count/2)-1);
  return next_task;

}

void print_tasks()
{
  int i;
  for (i = 0; i <= taskl_.count-1 ; ++i)
  {
    printf(" -->(%d)",  taskl_.tasks[i]->id);
  }
  
}

int main(int argc, char** argv)
{
int i;
 printf("\nSCHEDULED: %d", schedule_collector(1,11,3,4,5,6,7,8,9,10));
 return 0;

}
