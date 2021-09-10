// File: Print.c
// Purpose: Prints the contents of the shared memory.
// Author: Michael Probst

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

int main() {
  int id = 0;
  int read_id = 0;
  // Attach the shared memory segments;
  struct StudentInfo *infoptr = GetSharedMemories(&id);
  // Get pointer to read counter
  int *readptr = GetReadCounter(&read_id);
  // Get the id of the semaphore set created in Load.c;
  int sema_set=semget(SEMA_KEY, 0, 0);

  IncramentReadCount(sema_set, readptr);

  sleep(2); // for debugging and testing

  printf("the value of sema_set=%d\n", sema_set);
  PrintSemaph(sema_set, 0);
  PrintSemaph(sema_set, 1);
  printf("readcount = %i\n", *readptr);
  printf("----- STUDENT RECORDS -----\n");
  // print the records
  for (int i = 0; i < NUM_RECORDS; i++) {
    PrintStudent(&infoptr[i]);
    printf("\n");
  }
  printf("---------------------------\n");

  DecramentReadCount(sema_set, readptr);
}
