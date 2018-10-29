#include <stdio.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    char message[20], hostname[256];
    int rank, size, tag = 0;
    MPI_Status status;

    /* Initialize the MPI library */
    MPI_Init(&argc, &argv); // can receive NULL,NULL if you're not passing parameters from command-line
    /* Determine unique id of the calling process of all processes participating
       in this MPI program. This id is usually called MPI rank. */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    /* (int) MPI_Comm_rank(MPI_Comm comm, int *rank) receives the communicator to use and a pointer to process id */
    /* MPI_COMM_WORLD is a communicator created by MPI_INIT. All ranks (processes)
    created by MPI_INIT are in the set defined by MPI_COMM_WORLD. You can also restrict it to something more specific. */
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    /* Receives the size of the communication group (number of nodes)*/
    gethostname(hostname, 256); // Receive the hostname to show it is not running in the same machine

    if (rank == 0) { // Process 0
        strcpy(message, "Hello world"); // Copy message at runtime to prove it's being transmitted
        /* Send the message "Hello, there" from the process with rank 0 to the
           process with rank 1. */
        MPI_Send(message, strlen(message)+1, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
        /* (int) MPI_Send (void *buf, int count, MPI_Datatype, int dest, int tag, MPI_Comm comm) */
        /* Basic send of a 20 byte char message (with NULL terminator), to process 1 (rank 1) with message tag (type), with communicator Comm */
    } 

    else {
        /* Receive a message with a maximum length of 20 characters from process
           with rank 0. */
        MPI_Recv(message, 20, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status); /* Same definition from MPI_Send*/
        printf("Rank %d (running in host %s) of %d processes received message %s\n", rank, hostname, size, message);
    }

    /* Finalize the MPI library to free resources acquired by it. */
    MPI_Finalize();
    return 0;
}