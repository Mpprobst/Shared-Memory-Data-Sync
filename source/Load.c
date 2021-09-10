// File: Load.c
// Purpose: Loads a database of student information into segments of shared
//          memory. Also implements semaphores to synchronize access to the
//          shared memory segmens.
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

int main (int argc, char* argv[]) {
  if (argc != 2){
    fprintf(stderr, "usage: change <file name>\n");
    exit(3);
  }

  struct StudentInfo students[NUM_RECORDS];

  // read the input file
  FILE *fp ;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int ct = 0;
  int skip = 0;

  fp = fopen(argv[1], "r");
  if (fp == NULL) {
    fprintf(stderr, "\nError opening file\n");
    exit(1);
  }

  int student_ct = 0;
  while ((read = getline(&line, &len, fp)) != -1) {
    // some part of the record was invalid, skip the rest
    if (skip > 0) {
      ct = 0;
      skip--;
      continue;
    }

    ct++;
    if (ct == 1) {
      skip = ChangeName(line, &students[student_ct]);
    }
    else if (ct == 2) {
      skip = ChangeID(line, &students[student_ct]);
    }
    else if (ct == 3) {
      skip = ChangeAddress(line, &students[student_ct]);
    }
    else if (ct == 4) {
      ct = 0;
      skip = ChangePhone(line, &students[student_ct]);
      student_ct++;
    }

    if (skip == -1) {
      skip = 4 - ct;
    }
  }
  free(line);
  fclose(fp);
  // Create a shared memory segment to store the students' records
  int id = shmget(KEY, SEGSIZE*NUM_RECORDS,IPC_CREAT|0666);
  if (id <0){
    perror("load: shmget failed");
    exit(1);
  }

  // attach the shared memory segment to the process's address space
  struct StudentInfo *infoptr;
  infoptr=(struct StudentInfo *)shmat(id,0,0);
  if (infoptr <= (struct StudentInfo *) (0)) {
    perror("load: shmat failed");
    exit(2);
  }

  // Create a semaphore set consisting of enough semaphores (actually 2 will do);
  int sema_set;
  sema_set = GetSemaphs(SEMA_KEY, NUM_SEMAPHS);
  if ((sema_set < 0) ){
    perror("load: semget failed");
    exit(2);
  }

  // Create a shared memory segment to store the shared variable read_count;
  int *readptr;
  int read_id = shmget(READ_KEY, sizeof(int),IPC_CREAT|0666);
  if (read_id <0){
    perror("load: shmget failed");
    exit(1);
  }

  // attach the shared memory segment to the process's address space
  readptr=(int *)shmat(read_id, 0, 0);
  if (readptr <= (int *) (0)) {
    perror("load: shmat failed");
    exit(2);
  }
  Wait(sema_set, 0); // writing, lock the shared memory
  // Initialize read_count to 0 (the shared memory allocated for storing this var);
  *readptr = 0;
  // Load the shared memory segment with data from the file;
  for (int i = 0; i < student_ct; i++) {
    strcpy(infoptr[i].fName, students[i].fName);
    strcpy(infoptr[i].lName, students[i].lName);
    infoptr[i].middleInit = students[i].middleInit;
    strcpy(infoptr[i].id, students[i].id);
    strcpy(infoptr[i].address, students[i].address);
    strcpy(infoptr[i].telNumber, students[i].telNumber);
    strcpy(infoptr[i].whoModified, " ");
  }

  sleep(2);
  printf("loaded %i students\n", student_ct);
  Signal(sema_set, 0);  // done writing, free semaphore
  return 0;
}
