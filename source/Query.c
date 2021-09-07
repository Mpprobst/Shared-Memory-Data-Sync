// File: Query.c
// Purpose: Prints the record of a particular student.
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

// query is a reader. readers use semaphore idx 1
int main() {
  int id = 0;
  int read_id = 0;
  // Attach the shared memory segments;
  struct StudentInfo *infoptr = GetSharedMemories(&id);
  // Get pointer to read counter
  int *readptr=GetReadCounter(&read_id);
  // Get the id of the semaphore set created in Load.c;
  int sema_set=semget(SEMA_KEY, 0, 0);

  char input[ID_LENGTH];
  printf("Enter ID of student to query: ");
  if (fgets(input, sizeof(input), stdin) ==  NULL) {
    exit(0);
  }

  input[strlen(input)] = '\0';
  if (ValidateID(input) != 0){
    exit(2);
  }

  IncramentReadCount(sema_set, readptr);

  Wait(sema_set, 1);  // reading records, lock semaphore
  printf("reading student records...\n");
  sleep(2); // for debugging and testing

  // print the records
  int stuIdx = -1;
  for (int i = 0; i < NUM_RECORDS; i++) {
    if (strcmp(infoptr[i].id, input) == 0) {
      stuIdx = i;
      break;
    }
  }

  if (stuIdx != -1) {
    printf("Student found!\n");
    PrintStudent(&infoptr[stuIdx]);
  }
  else {
    printf("Could not find student with that ID.\n");
  }

  Signal(sema_set, 1);  // done reading, free semaphore
  DecramentReadCount(sema_set, readptr);

}
