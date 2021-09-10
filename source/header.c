// File: header.c
// Purpose: Implements common functions such as getting references to shared
//          memory and semaphores and common functions on semaphores. Also
//          implements logic that verifies a student record is properly formatted.
// Author: Michael Probst

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

/* Lock the semaphore n of the semaphore set semaph */
// Acknowldegement: This function provided by project sample code
void Wait(int semaph, int n)
{
  struct sembuf sop;
  sop.sem_num = n;
  sop.sem_op =  -1;
  sop.sem_flg = 0;
  semop(semaph,&sop,1);
}

/* Unlock the semaphore n of the semaphore set semaph */
// Acknowldegement: This function provided by project sample code
void Signal(int semaph, int n)
{
  struct sembuf sop;
  sop.sem_num = n;
  sop.sem_op =  1;
  sop.sem_flg = 0;
  semop(semaph,&sop,1);
}

void PrintSemaph(int semaph, int n) {
  int val = semctl(semaph, n, GETVAL);
  printf("semaph %i value = %i\n", n, val);
}

/* make an array of n semaphores with key k */
// Acknowldegement: This function provided by project sample code
int GetSemaphs(key_t k, int n)
{
  int semid, i;
  /* get a set of n semaphores with the given key k */
  if ((semid=semget(k,n,IPC_CREAT|0666))!= -1){
    for (i=0;i<n;i++)
      Signal(semid,i); /* unlock all the semaphores */
  }

  if ((semid < 0) ){
    perror("semget failed");
    exit(2);
  }

  return semid;
}

struct StudentInfo *GetSharedMemories(int *id) {
  // Get the ids of the two shared memory segments created;
  *id = shmget(KEY, SEGSIZE*NUM_RECORDS, 0);
  if (*id < 0){
    perror("shmget failed 1");
    exit(1);
  }

  // Attach the shared memory segments;
  struct StudentInfo *infoptr=(struct StudentInfo *)shmat(*id, 0, 0);
  if (infoptr <= (struct StudentInfo *) (0)) {
    perror("shmat failed");
    exit(2);
  }

  return infoptr;
}

int* GetReadCounter(int *read_id) {
  *read_id = shmget(READ_KEY, sizeof(int),IPC_CREAT|0666);
  if (*read_id <0){
    perror("shmget failed");
    exit(1);
  }

  // attach the shared memory segment to the process's address space
  int *readptr=(int *)shmat(*read_id, 0, 0);
  if (readptr <= (int *) (0)) {
    perror("shmat failed");
    exit(2);
  }
  return readptr;
}

void IncramentReadCount(int sema_set, int *readptr) {
  Wait(sema_set, 1);   // writing to readptr, lock semaphore
  *readptr = *readptr+1;

  if (*readptr == 1) {
    Wait(sema_set, 0);  // there are readers, lock data
  }
  Signal(sema_set, 1); // done with  readptr, free semaphore
}

void DecramentReadCount(int sema_set, int *readptr) {
  Wait(sema_set, 1);   // writing to readptr, lock semaphore
  *readptr = *readptr-1;

  if (*readptr == 0) {
    Signal(sema_set, 0);// no one is reading, free writers
  }
  Signal(sema_set,1);   // done with readptr, free semaphore
}

int ValidateName(char *fullname) {
  //printf("validating %s (len: %lu)\n", fullname, strlen(fullname));
  if (strlen(fullname) < 1) {
    return -1;
  }

  int ct = 0;
  int spaces[3];   // 2 max spaces between firstname, middlename, lastname, last idx is length of total string
  spaces[1] = strlen(fullname);
  spaces[2] = strlen(fullname);

  for (int i = 0; i < strlen(fullname); i++) {
    if (fullname[i] == ' ') {
      spaces[ct] = i;
      ct++;
    }
  }

  if (spaces[0] > NAME_LENGTH || ct < 1 || ct > 2) {
    return -1;
  }

  if ((spaces[ct] - spaces[ct-1]) > NAME_LENGTH) {
    return -1;
  }

  return 0;
}

// return -1 for failure, 0 for success
int ChangeName(char *fullname, struct StudentInfo *student) {
  if (ValidateName(fullname) == -1) {
    return -1;
  }

  int ct = 0;
  int spaces[3];   // 2 max spaces between firstname, middlename, lastname, last idx is length of total string
  spaces[1] = strlen(fullname);
  spaces[2] = strlen(fullname);

  for (int i = 0; i < strlen(fullname); i++) {
    if (fullname[i] == ' ') {
      spaces[ct] = i;
      ct++;
    }
  }

  // todo: trim excess whitespace and capatialize name;
  memcpy(student->fName, &fullname[0], (spaces[0])*sizeof(char));
  student->fName[spaces[0]] = '\0';
  if (ct == 2) {
    student->middleInit = fullname[spaces[1]-1];
  }
  else {
    student->middleInit = ' ';
  }

  memcpy(student->lName, &fullname[spaces[ct-1]+1], (spaces[ct]-spaces[ct-1])*sizeof(char));
  student->lName[strlen(student->lName)-1] = '\0';
  //printf("spacecount: %i. changed name to: %s %c %s\n", ct, student->fName, student->middleInit, student->lName);
  //printf("fName: %s (%lu) space at %i ", student->fName, strlen(student->fName), spaces[0]);
  //printf("lName: %s (%lu) space at: %i\n", student->lName, strlen(student->lName), spaces[ct]);

  return 0;
}

int ValidateID(char *id) {
  if (strlen(id) < 1) {
    return -1;
  }

  id[ID_LENGTH-1] = '\0';
  if (strlen(id) != ID_LENGTH-1) {
    printf("id: %s is not of length %i.\n", id, ID_LENGTH-1);
    return -1;
  }

  return 0;
}

int ChangeID(char *id, struct StudentInfo *student) {
  if (ValidateID(id) == -1) {
    return -1;
  }

  id[ID_LENGTH-1] = '\0';
  memcpy(student->id, id, (ID_LENGTH*sizeof(char)) );
  return 0;
}

int ValidateAddress(char *fulladdr) {
  if (strlen(fulladdr) < 1) {
    return -1;
  }

  if (strlen(fulladdr) > ADDRESS_LENGTH) {
    return -1;
  }

  // TODO: ensure address is in format: Housenum Streetname City, State ZIP
  return 0;
}

int ChangeAddress(char *fulladdr, struct StudentInfo *student) {
  if (ValidateAddress(fulladdr) == -1) {
    return -1;
  }

  fulladdr[strlen(fulladdr)-1] = '\0';
  memcpy(student->address, fulladdr, (ADDRESS_LENGTH*sizeof(char)) );
  return 0;
}

int ValidatePhone(char *phoneNum) {
  if (strlen(phoneNum) < 1) {
    return -1;
  }

  phoneNum[PHONE_LENGTH-1] = '\0';
  if (strlen(phoneNum) != PHONE_LENGTH-1) {
    return -1;
  }
  // TODO: make sure all characters are numbers
  // if number includes dashes, remove them.
  return 0;
}

int ChangePhone(char *phoneNum, struct StudentInfo *student) {
  if (ValidatePhone(phoneNum) == -1) {
    return -1;
  }

  phoneNum[PHONE_LENGTH-1] = '\0';
  memcpy(student->telNumber, phoneNum, (PHONE_LENGTH*sizeof(char)) );
  return 0;
}

void PrintStudent(struct StudentInfo *stu) {
  char fullname[50];
  strcpy(fullname, stu->fName);
  strcat(fullname, " ");
  if (stu->middleInit != ' ') {
    strncat(fullname, &stu->middleInit, 1);
    strcat(fullname, " ");
  }
  strcat(fullname, stu->lName);
  printf("%s\n ID: %s\n Address: %s\n Phone Number: %s\n",
    fullname, stu->id, stu->address, stu->telNumber);
  printf(" Last modified by: %s\n", stu->whoModified);
}
