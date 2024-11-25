#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int start_time;
    int finish_time;
    int waiting_time;
    int turnaround_time;
    int remaining_time;  // For preemptive and round robin
} Process;

void calculate_average_times(Process processes[], int n) {
    double total_tat = 0, total_wt = 0;
    for (int i = 0; i < n; i++) {
        total_tat += processes[i].turnaround_time;
        total_wt += processes[i].waiting_time;
    }
    printf("\nAverage Turnaround Time: %.2f", total_tat / n);
    printf("\nAverage Waiting Time: %.2f\n", total_wt / n);
}

void print_process_table(Process processes[], int n) {
    printf("\nProcess Table:\n");
    printf("PID\tAT\tBT\tPriority\tFT\tTAT\tWT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time, 
               processes[i].priority, processes[i].finish_time, processes[i].turnaround_time, processes[i].waiting_time);
    }
}

void priority_non_preemptive(Process processes[], int n) {
    printf("\n--- Priority Scheduling (Non-Preemptive) ---\n");
    int completed = 0, current_time = 0;
    bool visited[n];
    for (int i = 0; i < n; i++) visited[i] = false;

    while (completed < n) {
        int idx = -1, highest_priority = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (!visited[i] && processes[i].arrival_time <= current_time && processes[i].priority < highest_priority) {
                highest_priority = processes[i].priority;
                idx = i;
            }
        }
        if (idx == -1) {
            current_time++;
            continue;
        }

        visited[idx] = true;
        processes[idx].start_time = current_time;
        processes[idx].finish_time = current_time + processes[idx].burst_time;
        current_time = processes[idx].finish_time;
        processes[idx].turnaround_time = processes[idx].finish_time - processes[idx].arrival_time;
        processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
        completed++;
    }

    print_process_table(processes, n);
    calculate_average_times(processes, n);
}

void priority_preemptive(Process processes[], int n) {
    printf("\n--- Priority Scheduling (Preemptive) ---\n");
    int completed = 0, current_time = 0, last_pid = -1;
    int remaining[n];
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        remaining[i] = processes[i].remaining_time;
    }

    while (completed < n) {
        int idx = -1, highest_priority = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && processes[i].priority < highest_priority) {
                highest_priority = processes[i].priority;
                idx = i;
            }
        }
        if (idx == -1) {
            current_time++;
            continue;
        }
        if (processes[idx].remaining_time == processes[idx].burst_time)
            processes[idx].start_time = current_time;

        processes[idx].remaining_time--;
        current_time++;

        if (processes[idx].remaining_time == 0) {
            processes[idx].finish_time = current_time;
            processes[idx].turnaround_time = processes[idx].finish_time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
            completed++;
        }
    }

    print_process_table(processes, n);
    calculate_average_times(processes, n);
}

void round_robin(Process processes[], int n, int quantum) {
    printf("\n--- Round Robin Scheduling ---\n");
    int completed = 0, current_time = 0;
    int remaining[n];
    bool visited[n];
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        remaining[i] = processes[i].remaining_time;
        visited[i] = false;
    }

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (!visited[i]) {
                    processes[i].start_time = current_time;
                    visited[i] = true;
                }

                if (processes[i].remaining_time <= quantum) {
                    current_time += processes[i].remaining_time;
                    processes[i].remaining_time = 0;
                    processes[i].finish_time = current_time;
                    processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                    completed++;
                } else {
                    current_time += quantum;
                    processes[i].remaining_time -= quantum;
                }
            }
        }
    }

    print_process_table(processes, n);
    calculate_average_times(processes, n);
}

int main() {
    int n, quantum;
    printf("Enter the number of processes: ");
    scanf("%d", &n);
    Process processes[n];

    printf("Enter the time quantum for Round Robin: ");
    scanf("%d", &quantum);

    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        printf("Enter arrival time, burst time, and priority for process %d: ", i + 1);
        scanf("%d %d %d", &processes[i].arrival_time, &processes[i].burst_time, &processes[i].priority);
    }

    priority_non_preemptive(processes, n);
    priority_preemptive(processes, n);
    round_robin(processes, n, quantum);

    return 0;
}
