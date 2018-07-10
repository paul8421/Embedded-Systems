
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
Task* schedule_collector(TaskList* tl)
{
  return get_next_task();
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
    printf(" -->(%d--%d)",  taskl_.tasks[i]->id, taskl_.tasks[i]->dead_line);
  }
  
}

int main(int argc, char** argv)
{
/*
* accept the tasks from 0 to 6 and add them in the task_list
*/
  printf("\n Add task 1");
  accept_task(&t1, &taskl_);
  print_tasks();

  printf("\n Add task 2");
  accept_task(&t2, &taskl_);
  print_tasks();

  printf("\n Add task 3");
  accept_task(&t3, &taskl_);
  print_tasks();

  printf("\n Add task 4");
  accept_task(&t4, &taskl_);
  print_tasks();

  printf("\n Add task 5");
  accept_task(&t5, &taskl_);
  print_tasks();

  printf("\n Add task 6");
  accept_task(&t6, &taskl_);
  print_tasks();
/*
* schedule all the tasks 
*/
  printf("\n");
  printf("SCHEDULED: %d", schedule_collector(&taskl_)->id);
  printf("\nCURRENT TASK LIST: "); 
  print_tasks();

  printf("\n");
  printf("SCHEDULED: %d", schedule_collector(&taskl_)->id);
  printf("\nCURRENT TASK LIST: "); 
  print_tasks();

  printf("\n");
  printf("SCHEDULED: %d", schedule_collector(&taskl_)->id);
  printf("\nCURRENT TASK LIST: "); 
  print_tasks();


  printf("\n");
  printf("SCHEDULED: %d", schedule_collector(&taskl_)->id);
  printf("\nCURRENT TASK LIST: "); 
  print_tasks();
  
  printf("\n");
  printf("SCHEDULED: %d", schedule_collector(&taskl_)->id);
  printf("\nCURRENT TASK LIST: "); 
  print_tasks();


  printf("\n");
  printf("SCHEDULED: %d", schedule_collector(&taskl_)->id);
  printf("\nCURRENT TASK LIST: "); 
  print_tasks();
  printf("\n");
}



