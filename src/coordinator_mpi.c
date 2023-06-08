#include <mpi.h>

#include "coordinator.h"

#define READY 0
#define NEW_TASK 1
#define TERMINATE -1

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Error: not enough arguments\n");
    printf("Usage: %s [path_to_task_list]\n", argv[0]);
    return -1;
  }

  // Read and parse task list file
  int num_tasks;
  task_t **tasks;
  if (read_tasks(argv[1], &num_tasks, &tasks))
    return -1;

  // MPI_Init to initialize the program
  MPI_Init(&argc, &argv);

  int procID, totalProcs;
  // total number of processes and store in `totalProcs`
  MPI_Comm_size(MPI_COMM_WORLD, &totalProcs);
  // get the ID of the current program, and store in `procID`
  MPI_Comm_rank(MPI_COMM_WORLD, &procID);

  if (procID == 0) {
    // Manager node
    int nextTask = 0;
    MPI_Status status;
    int32_t message;

    while (nextTask < num_tasks) {
        // TODO: receive a message from any source (so we know that this node is done with its task)
        MPI_Recv(&message, 1, MPI_INT32_T, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        int sourceProc = status.MPI_SOURCE;

        // TODO: send `nextTask` as the message to the process we just received a message from
        message = nextTask;
        MPI_Send(&message, 1, MPI_INT32_T, sourceProc, 0, MPI_COMM_WORLD);

        nextTask++;
    }

    // Wait for all processes to finish
    for (int i = 0; i < totalProcs - 1; i++) {

        // receive a message from any source (so we know that this node is done with its task)
        MPI_Recv(&message, 1, MPI_INT32_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        int sourceProc = status.MPI_SOURCE;

        message = TERMINATE;
        MPI_Send(&message, 1, MPI_INT32_T, sourceProc, 0, MPI_COMM_WORLD);
    }

    free(tasks);

  } else {
    // Worker node
    int32_t message;

    while (true) {

        // TODO: let the manager node know that this worker is ready
        message = READY;
        MPI_Send(&message, 1, MPI_INT32_T, 0, 0, MPI_COMM_WORLD);

        // TODO: receive 1 message from the manager and store it in `message`
        MPI_Recv(&message, 1, MPI_INT32_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (message == TERMINATE) break;

        // Do the task of worker node
        if (execute_task(tasks[message])) {
          printf("Task %d failed\n", message);
          return -1;
        }
        free(tasks[message]->path);
    }
  }

  MPI_Finalize();
}
