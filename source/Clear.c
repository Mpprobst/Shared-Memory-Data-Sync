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

// clear is a writer
int main (int argc, char* argv[]) {
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

  // Prompt the user for password;
  if (ValidatePassword() == -1) {
    exit(1);
  }
  //
  Wait(sema_set, 0); // assuming semaset is the id of the semaphore set

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
  // delete the shared memory segments
  shmdt((char *)infoptr); // detach the shared memory segment
  shmctl(id, IPC_RMID,(struct shmid_ds *)0);  // destroy shared memory segment

  shmdt((int *)readptr);  // detach the shared memory segment
  shmctl(read_id, IPC_RMID,(struct shmid_ds *)0); // destroy shared memory segment
  Signal(sema_set, 0);

  // delete the semaphores.
  semctl(sema_set, 0, IPC_RMID); // Remove the semaphore set
  return 0;
}
