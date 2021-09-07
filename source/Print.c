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

// print is a reader. readers use semaphore idx 1
int main() {
  int id = 0;
  int read_id = 0;
  // Attach the shared memory segments;
  struct StudentInfo *infoptr = GetSharedMemories(&id);
  // Get pointer to read counter
  int *readptr = GetReadCounter(&read_id);
  // Get the id of the semaphore set created in Load.c;
  int sema_set=semget(SEMA_KEY, 0, 0);
/*
  Wait(sema_set, 0);  // writing to readptr, prevent others from doing so
  *readptr = *readptr+1;
  Signal(sema_set, 0);

  Wait(sema_set, 1);  // reading the readptr, prevent others from editing it
  if (*readptr == 1) {
    // print is reading, prevent writing
    Wait(sema_set, 0);
  }

  sleep(2); // for debugging and testing
*/
  printf("the value of sema_set=%d\n", sema_set);
  PrintSemaph(sema_set, 0);
  PrintSemaph(sema_set, 1);
  printf("readcount = %i\n", *readptr);
  printf("----- STUDENT RECORDS -----\n");
  // print the records
  for (int i = 0; i < NUM_RECORDS; i++) {
    PrintStudent(&infoptr[i]);
  }
  printf("---------------------------\n");
/*
  Signal(sema_set, 1);  // no longer reading

  Wait(sema_set, 0);  // writing to readptr, lock the semaphore
  *readptr = *readptr-1;
  Signal(sema_set, 0);

  if (*readptr == 0) {
    Signal(sema_set, 0);  // no one is reading anymore, allow writers
  }
  */
}
