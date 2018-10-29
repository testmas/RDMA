// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NUM_ELEMENTS 10

int main(int argc, char **argv) {
  
    // In this program, everyone increases a vector by a certain amount
  
    int rank, N, i, workerRank, tag = 0;
    double result[NUM_ELEMENTS];

    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Initialize communication
    MPI_Comm_size(MPI_COMM_WORLD, &N); // Get number of members in Communication Group (number of total processes)
    MPI_Status status;

    if (rank == 0) { // If i'm the managing process

        for (i = 0; i < NUM_ELEMENTS; i++) { // Initialize result vector (could have used memset also, but one more library)
            result[i] = 0;
        }

        for (workerRank = 1; workerRank < N; workerRank++) { // rank 0 sends a message with the a vector to each participant
        
            MPI_Send((void *) &result[0], NUM_ELEMENTS, MPI_DOUBLE, workerRank, tag, MPI_COMM_WORLD); // For syntax, read the other two documents I sent you
            // (Buffer src address, number of elements in buffer, type of element, destination, type of communication, communication group (global))
        
        }

    } 

    else { // If I am the other processes, I wait for a message that is destined to me 

        MPI_Recv((void *) &result[0], NUM_ELEMENTS, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
        // buffer src address, number of elements, type, message sender, type of message, communication group, status
    }

    if (rank != 0) { // if I am a worker, I'll receive my share of the work
        
        // block divide the work (the way with if/module is a way to avoid using math.h. Otherwise we could split the work calling
        // start = floor((rank-1)*NUM_ELEMENTS/(N-1))
        // end = floor((rank) * NUM_ELEMENTS/(N-1))
        int start, end, total = 0;

        start = (rank-1)* NUM_ELEMENTS/(N-1);

        end = rank * NUM_ELEMENTS/(N-1);

        if (rank == (N - 1)) {
            
            end = end + (NUM_ELEMENTS % (N - 1));

        }

        for (i = start; i < end; i++) {
          
        total = total + 1; // Increase vector
        
        }
	printf("Rank %d received %d.\n", rank, total);

    }

    MPI_Barrier(MPI_COMM_WORLD); // Everyone in comm group will block in this semaphore until all processes reach this point

    MPI_Finalize(); // Close MPI

    return 0;
}
