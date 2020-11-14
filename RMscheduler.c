#include <stdio.h>
#include <stdlib.h>

struct Task {
  int period;
  int comp_time;
  int priority;
  int remaining_time;
  int activation;
};

int gcd(int a, int b)
{
    if (a == 0)
        return b;
    return gcd(b % a, a);
}

// Function to return LCM of two numbers
int lcm(int a, int b)
{
    return (a / gcd(a, b)) * b;
}

void create_task(struct Task *n, int i) {

    printf("Insert Period for Task %d: ", i);
    scanf("%d", &n->period);
    printf("Insert Computation Time for Task %d: ", i);
    scanf("%d", &n->comp_time);
}

int main(int argc, char* argv[]) {

  int task_num;
  int i, j, k;
  float util_factor, test;
  int greatest_priority_task, divider;

  printf("Insert Number of Tasks: ");
  scanf("%d", &task_num);

  int response_time[task_num];

  //////////////////////////////////////////////////////////////////////////////
  //Task Initialization
  //////////////////////////////////////////////////////////////////////////////
  struct Task tTask[task_num];

  for (i=0;i<task_num;i++) {
    create_task(&tTask[i], i+1);
    tTask[i].priority = task_num;
    tTask[i].remaining_time = tTask[i].comp_time;
    tTask[i].activation = 1;
    response_time[i]=0;
  }

  //////////////////////////////////////////////////////////////////////////////
  //Scedulability Test
  //////////////////////////////////////////////////////////////////////////////

  switch(task_num) {
    case 1:
      test = 1.0;
      break;
    case 2:
      test = 0.83;
      break;
    case 3:
      test = 0.78;
      break;
    case 4:
      test = 0.76;
      break;
    case 5:
      test = 0.74;
      break;
    default:
      test = 0.69;
      break;
  }

  util_factor = 0.0;
  for (i=0; i<task_num; i++) {
    util_factor += ((float)tTask[i].comp_time)/((float)tTask[i].period);
  }

  printf("Up = %0.2lf -> ", util_factor);
  if (util_factor <= test) {
    printf("This Task Set is Scedulable with Rate Monotoinc\n");
  }
  else {
    printf("We cannot conclude about the Scedulability of this Task Set\n");
  }

  //////////////////////////////////////////////////////////////////////////////
  //Hyperperiod Calculation of 3 tasks
  //////////////////////////////////////////////////////////////////////////////

  int hyp1 = lcm(tTask[0].period, tTask[1].period);
  int hyp2 = lcm(hyp1, tTask[2].period);
  int hyperperiod = lcm(hyp2, tTask[3].period);
  printf ("Hyperperiod = %d\n", hyperperiod);

  //////////////////////////////////////////////////////////////////////////////
  //Priority assignment
  //////////////////////////////////////////////////////////////////////////////

  for (i=0; i<task_num; i++) {
    for (j=i+1; j<task_num; j++) {
      if (tTask[i].period > tTask[j].period) {
        tTask[i].priority--;
      }
      else if (tTask[i].period == tTask[j].period){
        tTask[j].priority = tTask[i].priority;
      }
      else {
        tTask[j].priority--;
      }
    }
  }

  //////////////////////////////////////////////////////////////////////////////
  //Deadline Missing Array Initialization
  //////////////////////////////////////////////////////////////////////////////
  int deadline_miss_array[hyperperiod];

  for(i=0;i<hyperperiod;i++) {
    deadline_miss_array[i] = 0;
  }

  //////////////////////////////////////////////////////////////////////////////
  //Sceduling for 1 Hyperperiod
  //////////////////////////////////////////////////////////////////////////////

  int times[task_num];

  for(i=0;i<task_num;i++) {
    times[i]=0;
  }

  for (k=0; k<hyperperiod; k++) {
    greatest_priority_task = 0;
    for (i=0; i<task_num; i++) {
      if (k%tTask[i].period == 0) {
        if ((tTask[i].remaining_time>0) && (k>0)) {
          deadline_miss_array[k-1]=1;
        }
        tTask[i].remaining_time = tTask[i].comp_time;
        tTask[i].activation = 1;
        times[i]++;
      }
      if (tTask[greatest_priority_task].activation) {
        if (tTask[i].activation) {
          if (tTask[i].priority > tTask[greatest_priority_task].priority) {
            greatest_priority_task = i;
          }
        }
      }
      else {
        i=0;
        greatest_priority_task++;
        if (greatest_priority_task > task_num) {
          greatest_priority_task = 0;
        }
      }
    }

    if ((greatest_priority_task<task_num) && (greatest_priority_task>=0)) {
      printf("%d|___T%d___|", k, greatest_priority_task+1);
      tTask[greatest_priority_task].remaining_time--;
      if (tTask[greatest_priority_task].remaining_time == 0) {

        tTask[greatest_priority_task].activation = 0;
        if (times[greatest_priority_task] == 1) {
          response_time[greatest_priority_task] = k+1;
        }
        else if (times[greatest_priority_task]>1){
          response_time[greatest_priority_task] += k + 1 - (times[greatest_priority_task]-1)*tTask[greatest_priority_task].period;
        }
      }
      else {
        tTask[greatest_priority_task].activation = 1;

      }
    }
    else {
      printf("%d|__IDLE__|", k);
    }
  }
  printf("%d\n", hyperperiod);

  //////////////////////////////////////////////////////////////////////////////
  //Metrics
  //////////////////////////////////////////////////////////////////////////////
  float av_waiting_time, av_response_time;

  for (i=0; i<hyperperiod; i++) {
    if (deadline_miss_array[i]==1) {
      printf("Deadline Missing at time %d!\n", i+1);
    }
  }

  for (i=0; i<task_num; i++) {
    divider = hyperperiod/tTask[i].period;
    av_response_time = ((float)response_time[i])/divider;
    av_waiting_time = (float)av_response_time - tTask[i].comp_time;
    printf("Average Response Time for T%d = %0.2lf\n", i+1, av_response_time);
    printf("Average Waiting Time for T%d = %0.2lf\n", i+1, av_waiting_time);
  }

  return (0);
}
