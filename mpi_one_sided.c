/*
// Copyright 2003-2014 Intel Corporation. All Rights Reserved.
// 
// The source code contained or described herein and all documents related 
// to the source code ("Material") are owned by Intel Corporation or its
// suppliers or licensors.     Title to the Material remains with Intel Corporation
// or its suppliers and licensors.     The Material is protected by worldwide
// copyright and trade secret laws and treaty provisions.     No part of the
// Material may be used, copied, reproduced, modified, published, uploaded,
// posted, transmitted, distributed, or disclosed in any way without Intel's
// prior express written permission.
// 
// No license under any patent, copyright, trade secret or other intellectual
// property right is granted to or conferred upon you by disclosure or delivery
// of the Materials, either expressly, by implication, inducement, estoppel
// or otherwise.     Any license under such intellectual property rights must
// be express and approved by Intel in writing.
*/
#include <stdio.h>
#include <mpi.h>

#define NUM_ELEMENT     4

int main(int argc, char** argv)
{

		/* This example program populates the sharedbuffer of each process with numbers in the interval [10*id, 10*id + NUM_ELEMENTS - 1]
		   where id is the initial process id. 
		   After all processes have generated their numbers, everyone is blocked on a semaphore (MPI_Win_fence) that only unblocks when all processes
		   in a communication group are blocked there. 
		   Then they do an RDMA Get (unidirectional, fetch) of each other's sharedbuffer, updating their data. This get is done to 
		   circularly shift right the sharedbuffers among processes, transmitting the values through an MPI_window
		   among processes. 
		   This way, all processes will have [(10 * (id + 1) % NUM_PROCESSES), (10 * (id + 1) % NUM_PROCESSES)  + NUM_ELEMENTS- 1].
		   When everyone has finished, they again are blocked on a semaphore (MPI_Win_fence), where each process does an RDMA Put (unidirectional, load) 
		   to circularly shift left the sharedbuffers among the processes so everyone returns to their original interval, 
		   [10*id, 10*id + NUM_ELEMENTS - 1] */
      /* Also, if you didn't want to synchronize everyone using Active RMA, you could do Passive RMA by using MPI_Win_lock/MPI_Win_unlock to guarantee
      mutual exclusion or multiple readers/single writer on the window */
		   
         int i, id, num_procs, len, localbuffer[NUM_ELEMENT], sharedbuffer[NUM_ELEMENT];

         // Number of process, localbuffer to store data, sharedbuffer to share data among all processes
         char name[MPI_MAX_PROCESSOR_NAME];
         MPI_Win win; // MPI window

         MPI_Init(&argc, &argv);
         MPI_Comm_rank(MPI_COMM_WORLD, &id);
         MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
         MPI_Get_processor_name(name, &len);

         printf("Rank %d running on %s\n", id, name);

         MPI_Win_create(sharedbuffer, NUM_ELEMENT, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
         /* int MPI_Win_create(void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win) */
         /* Creates a shared array (window object) that can be used by all processes in the comm set World to perform RDMA operations.
		info provides runtime optimization arguments (no_locks, accumulation_ordering, etc) 
		Note that is is only an agreement of where the shared data is located. At this point, the data can only be accessed by its own process.*/         


         for (i = 0; i < NUM_ELEMENT; i++)
         {
        	sharedbuffer[i] = 10*id + i; // We are altering the sharedbuffer, but changes (synchronization) are only seen when MPI_Win_fence is called.
            localbuffer[i] = 0;		     // Thus, the window is "opaque" until MPI_Win_fence makes it "transparent".
         }

         printf("Rank %d sets data in the shared memory:", id);

         for (i = 0; i < NUM_ELEMENT; i++)
             printf(" %02d", sharedbuffer[i]);

         printf("\n");
    
         MPI_Win_fence(0, win);
         /* int MPI_Win_fence(int assert, MPI_Win win) */
         /* MPI fence synchronization (no RMA communication can be done until the target has called this fence.)
         Essentially, it is equivalent of blocking everyone on a semaphore that will be unblocked only when all are ready to talk.
      	 This is done to ensure all processes will receive the same information at the same time.
		 (Synchronizes the windows and exposes them to other processes. No GET can be done before this step)
      	 It is possible to define a communicator group different than MPI_COMM_WORLD for the window and thus only block a set of processes. */

         if (id != 0)
            // If you didn't want to use MPI_Win_fence, you could put:
            // MPI_LOCK_EXCLUSIVE => Mutex, MPI_LOCK_SHARED => multiple readers, one writer
            // MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 1, 0, win);
         	MPI_Get(&localbuffer[0], NUM_ELEMENT, MPI_INT, id-1, 0, NUM_ELEMENT, MPI_INT, win);
            // MPI_Win_unlock(MPI_LOCK_EXCLUSIVE, 1, 0 ,win)
            /* int MPI_Get(void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp,
            int target_count, MPI_Datatype target_datatype, MPI_Win win) */
         	/* This function gets origin_count bytes (NUM_ELEMENT) from another process' window (in this case process id - 1)
         	of type MPI_INT, starting from target_disp (in this case 0 implies we want the entire window). 
         	Observe that we interpret the data in the same way in both the source and destiny and want all the bytes, thus the result is
         	the entire target buffer is copied to the originator of the request */
         else
            MPI_Get(&localbuffer[0], NUM_ELEMENT, MPI_INT, num_procs-1, 0, NUM_ELEMENT, MPI_INT, win);

         MPI_Win_fence(0, win); 
         /* Again, as we are synchronizing, it will block any process that finished getting his data from the window until all processes are finished */

         printf("Rank %d gets data from the shared memory:", id);

         for (i = 0; i < NUM_ELEMENT; i++)
            printf(" %02d", localbuffer[i]);

         printf("\n");

         MPI_Win_fence(0, win); /* Synchronize everyone again (Starting data transfer)*/

         if (id < num_procs-1)
            MPI_Put(&localbuffer[0], NUM_ELEMENT, MPI_INT, id+1, 0, NUM_ELEMENT, MPI_INT, win);
        	/* int MPI_Put(const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp,
            int target_count, MPI_Datatype target_datatype, MPI_Win win) */
        	/* This is the opposite of MPI_Get, the process is inserting its own data into another process' window. The syntax is exactly the same as MPI_Get */

         else
            MPI_Put(&localbuffer[0], NUM_ELEMENT, MPI_INT, 0, 0, NUM_ELEMENT, MPI_INT, win);

         MPI_Win_fence(0, win); /* Everyone finished receiving his own data */

         printf("Rank %d has new data in the shared memory:", id);
    
         for (i = 0; i < NUM_ELEMENT; i++)
            printf(" %02d", sharedbuffer[i]);

         printf("\n");

         MPI_Win_free(&win); // We free the window
         MPI_Finalize(); // and finalize the communication
         return 0;
}
