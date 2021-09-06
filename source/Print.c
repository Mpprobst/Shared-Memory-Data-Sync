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
  // Get the ids of the two shared memory segments created;
  int id = shmget(KEY, SEGSIZE*NUM_RECORDS, 0);
  if (id < 0){
    perror("clear: shmget failed 1");
    exit(1);
  }

  int read_id = shmget(READ_KEY, sizeof(int), 0);
  if (read_id < 0){
    perror("clear: shmget failed 2");
    exit(1);
  }

  // Attach the shared memory segments;
  struct StudentInfo *infoptr=(struct StudentInfo *)shmat(id, 0, 0);
  if (infoptr <= (struct StudentInfo *) (0)) {
    perror("clear: shmat failed");
    exit(2);
  }

  int *readptr=(int *)shmat(read_id, 0, 0);
  if (readptr <= (int *) (0)) {
    perror("clear: shmat failed");
    exit(2);
  }

  // Get the id of the semaphore set created in Load.c;
  int sema_set=semget(SEMA_KEY, 0, 0);

  Wait(sema_set,1);
  *readptr = *readptr+1;
  if (*readptr == 1) {
    Wait(sema_set, 0);
  }

  Signal(sema_set,1);

  printf("the value of sema_set=%d\n", sema_set);
  printf("----- STUDENT RECORDS -----\n");
  // print the records
  for (int i = 0; i < NUM_RECORDS; i++) {
    PrintStudent(&infoptr[i]);
  }
  printf("---------------------------\n");


  sleep(2); // for debugging and testing

  Wait(sema_set, 1);
  *readptr = *readptr-1;

  if (*readptr == 0) {
    Signal(sema_set, 0);
  }
  Signal(sema_set,1);



}
