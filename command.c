/*
 * COMP 3500: Project 5 Scheduling
 * Xiao Qin
 * 
 * This source code shows how to pass commandline arguments to your progrm
 *
 * How to compile? 
 * $gcc command.c -o command
 *
 * How to run?
 * Case 1: no argument. Sample usage is printed
 * $./command
 * Usage: command file_name [FCFS|RR|SRTF] [time_quantum]
 *
 * Case 2: 2 arguments. 
 * $./command file1 FCFS 
 * The file name is: file1
 * The chosen policy is: FCFS
 *
 * Case 3: 2 arguments.
 * $./command file2 RR
 * The file name is file2
 * The chosen policy is RR
 * Please enter time_quantum for the RR policy!
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TASK_NUM 32

typedef unsigned int u_int;

typedef struct task {    
	u_int pid;    
	u_int arrival_time;    
	u_int burst_time;
	u_int time_remaining;
} task_t;

int wait_time_array[MAX_TASK_NUM];
int turn_around_array[MAX_TASK_NUM];
int response_time_array[MAX_TASK_NUM];
int start_times[MAX_TASK_NUM];

int main( int argc, char *argv[] )  {
	char *file_name; /* file name from the commandline */
    FILE *fp; /* file descriptor */
    task_t task_array[MAX_TASK_NUM];
    
    u_int i;
    u_int count;
	u_int time_quantum;
    
    if (argc == 1 || argc > 4) {
        printf("Usage: command file_name [FCFS|RR|SRFT] [time_quantum]\n");
        return EXIT_FAILURE;
    }
    
    file_name = argv[1];
    if (! (fp = fopen(file_name, "r"))) {
        printf("File %s can't be opened. Please retry ...\n");
        return EXIT_FAILURE;
    }

    printf("Open file: %s\n", file_name);
    
    printf("The chosen policy is %s\n", argv[2]);
    
    if (strcmp(argv[2], "RR") == 0) {
        if (argc == 4) {
            printf("time_quantum is set to %s\n", argv[3]);
            printf("TimeQ = %u\n", time_quantum);
        }
        else {
            printf("Please enter time_quantum for the RR policy!\n");
            return EXIT_FAILURE;
        }
    }

    /* read data from input file */
    count = 0;
    while (fscanf(fp, "%u %u %u", &task_array[count].pid, &task_array[count].arrival_time, \
                  &task_array[count].burst_time)!= EOF) {
        task_array[count].time_remaining = task_array[count].burst_time;
        count++;
    }
 
    printf("There are %u tasks loaded from %s ...\n", count, file_name);

    /* print task_array to check input file */
    for (i = 0; i < count; i++)
        printf("Task PID: %u, Arrival Time: %u, Burst Time: %u\n", task_array[i].pid, \
               task_array[i].arrival_time, task_array[i].burst_time);

    printf("Press any key to continue ...\n");
    getchar();
    

    fclose(fp);
    u_int time = 0;
    if (strcmp(argv[2], "FCFS") == 0) {
        printf("Run FCFS\n");
		 time = fcfs_policy(task_array, count);
    }
    else if (strcmp(argv[2], "RR") == 0) {
        time = rr_policy(task_array, count, argv[3]);
    }
    else if (strcmp(argv[2], "SRFT") == 0) {
         time = srtf_policy(task_array, count);
    }
    else {
        printf("Error\n");
    } 
	
	u_int total_burst_time = 0;
	i = 0;
	for (i = 0; i < count; i++) {
		total_burst_time += task_array[i].burst_time;
	}
	double total_cpu_usage = total_burst_time / time;
    
    return EXIT_SUCCESS;
	
}

int fcfs_policy(task_t task_array[], u_int count) {
	int time = 0, countCompleted = 0;
	while (countCompleted < count) {
		while (task_array[countCompleted].time_remaining > 0) {
			printf("<time %u> process %u is running\n", time, task_array[countCompleted].pid);
			if (task_array[countCompleted].time_remaining == task_array[countCompleted].burst_time) {
				start_times[countCompleted] = time;
			}
			time++;
			task_array[countCompleted].time_remaining--;
		}
		printf("<time %u> process %u is finished...\n", time, task_array[countCompleted].pid);
		calcTurnAroundTime(task_array[countCompleted], time, countCompleted);
		calcResponseTime(task_array[countCompleted], start_times[countCompleted], countCompleted);
		countCompleted++;
	}
	printf("<time %u> All processes finish\n", time);
	return time;
}

int rr_policy(task_t task_array[], u_int count, int quantum) {
	int time = 0, countCompleted = 0;
	while (countCompleted < count) {
		int i = 0;
		for (i = 0; i < count; i++) {
			if (task_array[i].time_remaining > 0) {
				int index = 0;
				while (index < quantum) {
					printf("<time %u> process %u is running\n", time, task_array[i].pid);
					if (task_array[i].time_remaining == task_array[i].burst_time) {
						start_times[i] = time;
					}
					time++;
					index++;
					task_array[i].time_remaining--;
					if (task_array[i].time_remaining <= 0) {
						countCompleted++;
						printf("<time %u> process %u is finished...\n", time, task_array[i].pid);
						calcTurnAroundTime(task_array[i], time, i);
						calcResponseTime(task_array[i], start_times[i], i);
						break;
					}
				}
			}
		}
	}
	return time;
}

int srtf_policy(task_t task_array[], u_int count) {
    int time = 0, countCompleted = 0;
    task_t choice = task_array[0];
    while (countCompleted < count) {
        int i = 0;
        for (i = 0; i < count; i++) {
            if (task_array[i].arrival_time <= time && task_array[i].time_remaining < choice.time_remaining && task_array[i].time_remaining > 0) {
                choice = task_array[i];
            }
        }
        printf("<time %u> process %u is running\n", time, choice.pid);
        time ++;
        for (i = 0; i < count; i++) {
			if (task_array[i].pid == choice.pid) {
				if (task_array[i].time_remaining == task_array[i].burst_time) {
					start_times[i] = time - 1;
				}
                choice.time_remaining--;
				task_array[i].time_remaining--;
                if (task_array[i].time_remaining <= 0) {
                    countCompleted++;
					printf("<time %u> process %u is finished...\n", time, choice.pid);
                    calcTurnAroundTime(task_array[i], time, i);
                    calcResponseTime(task_array[i], start_times[i], i);
                }
            }
        }
    }
    return time;
}

void calcWaitTime(task_t task, u_int turnaround_time, int slot) {
	u_int execution_time = task.burst_time;
	u_int waiting_time = turnaround_time - execution_time;
	wait_time_array[slot] = waiting_time;
}

void calcTurnAroundTime(task_t task, u_int finish_time, int slot) {
	u_int turnaround_time = finish_time - task.arrival_time;
	turn_around_array[slot] = turnaround_time;
	calcWaitTime(task, turnaround_time, slot);
}

void calcResponseTime(task_t task, u_int start_time, int slot) {
	u_int response_time = start_time - task.arrival_time;
	response_time_array[slot] = response_time;
}