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

// load is a writer
int main (int argc, char* argv[]) {
  if (argc != 2){
    fprintf(stderr, "usage: change <file name>\n");
    exit(3);
  }

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
/*
  // get a set of NUM_SEMAPHS semaphores
  int sema_set;
  sema_set = GetSemaphs(SEMA_KEY, NUM_SEMAPHS);
  if ((sema_set < 0) ){
    perror("create: semget failed");
    exit(2);
  }

  // Create a shared memory segment to store the shared variable read_count;
  int read_count;
  int read_id = shmget(KEY, sizeof(int),IPC_CREAT|0666);
  if (read_id <0){
    perror("create: shmget failed");
    exit(1);
  }

  // attach the shared memory segment to the process's address space
  read_count=(int)shmat(read_id,0,0);
  if (read_count < 0 {
    perror("create: shmat failed");
    exit(2);
  }
*/
  struct StudentInfo student;
  while ((read = getline(&line, &len, fp)) != -1) {
    // some part of the record was invalid, skip the rest
    if (skip > 0) {
      //printf("--------SKIP %i--------\n", skip);
      ct = 0;
      skip--;
      continue;
    }

    ct++;
    if (ct == 1) {
      skip = ChangeName(line, &student);
    }
    else if (ct == 2) {
      skip = ChangeID(line, &student);
    }
    else if (ct == 3) {
      skip = ChangeAddress(line, &student);
    }
    else if (ct == 4) {
      ct = 0;
      skip = ChangePhone(line, &student);
/*
      // Do we create a shared memory segment each time we read a new student?
      // Create a shared memory segment to store the students' records ;
      int id = shmget(KEY, SEGSIZE,IPC_CREAT|0666);
      if (id <0){
        perror("create: shmget failed");
        exit(1);
      }

      // attach the shared memory segment to the process's address space
      struct StudentInfo *infoptr;
      infoptr=(struct StudentInfo *)shmat(id,0,0);
      if (infoptr <= (struct StudentInfo *) (0)) {
        perror("create: shmat failed");
        exit(2);
      }

      // store data in the shared memory segment
      strcpy(infoptr->fName, student->fName);
      strcpy(infoptr->lName, student->lName);
      infoptr->middleInit = *student->middleInit;
      strcpy(infoptr->id, student->id);
      strcpy(infoptr->address, student->address);
      strcpy(infoptr->telNumber, student->telNumber);
      strcpy(infoptr->whoModified, " ");
      */
      printf("--- NEW STUDENT ---\n NAME: %s %s\n ID: %s\n ADDRESS: %s\n PHONE: %s\n-------------------\n", student.fName, student.lName, student.id, student.address, student.telNumber);
    }

    if (skip == -1) {
      skip = 4 - ct;
    }
  }
  free(line);
  fclose(fp);

  // Create a semaphore set consisting of enough semaphores (actually 2 will do);
  // Attach the two shared memory segments created;
  //
  // Wait(semaset, 0); // assuming semaset is the id of the semaphore set created
  // Initialize read_count to 0 (the shared memory allocated for storing this var);
  // Load the shared memory segment with data from the file;
  // Signal(semaset, 0);
  // ---------------------
  return 0;
}
