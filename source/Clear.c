// File: Clear.c
// Purpose: Destroys the shared memory segments storing student records and
//          semaphores created by Load.c
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
#include "password.h"

int main () {
  // Attach the shared memory segments;
  int id = 0;
  int read_id = 0;
  struct StudentInfo *infoptr = GetSharedMemories(&id);
  // Get read count pointer
  int *readptr = GetReadCounter(&read_id);
  // Get the id of the semaphore set created in Load.c;
  int sema_set=semget(SEMA_KEY, 0, 0);

  char advisor[10];
  // Prompt the user for password;
  if (ValidatePassword(advisor) == -1) {
    exit(1);
  }
  advisor[9] = '\0';
  Wait(sema_set, 0); // writing to data, lock it

  // write the contents of the shared memory to file in the format of the input file;
  FILE *fp;
  fp = fopen("output.txt", "w");

  if (fp == NULL) {
    fprintf(stderr, "\nError creating output file\n");
    exit(1);
  }

  for (int i = 0; i < NUM_RECORDS; i++) {
    struct StudentInfo *stu = &infoptr[i];
    // validate the record
    char fullname[50];
    strcpy(fullname, stu->fName);
    strcat(fullname, " ");
    if (stu->middleInit != ' ') {
      strncat(fullname, &stu->middleInit, 1);
      strcat(fullname, " ");
    }
    strcat(fullname, stu->lName);

    int status = ValidateName(fullname) +
                 ValidateID(stu->id) +
                 ValidateAddress(stu->address) +
                 ValidatePhone(stu->telNumber);

    if (status == 0) {
      fprintf(fp, "%s\n", fullname);
      fprintf(fp, "%s\n", stu->id);
      fprintf(fp, "%s\n", stu->address);
      fprintf(fp, "%s\n", stu->telNumber);
    }
  }

  fclose(fp);
  sleep(2);
  // delete the shared memory segments
  shmdt((char *)infoptr); // detach the shared memory segment
  shmctl(id, IPC_RMID,(struct shmid_ds *)0);  // destroy shared memory segment

  Wait(sema_set, 1); // writing to read counter, lock it
  shmdt((int *)readptr);  // detach the shared memory segment
  shmctl(read_id, IPC_RMID,(struct shmid_ds *)0); // destroy shared memory segment

  Signal(sema_set, 0);  // done writing
  Signal(sema_set, 1);  // done writing

  // delete the semaphores.
  semctl(sema_set, 0, IPC_RMID); // Remove the semaphore set

  printf("Shared memory and semaphores destroyed.\n");
  printf("Records saved in output.txt\n");
  return 0;
}
