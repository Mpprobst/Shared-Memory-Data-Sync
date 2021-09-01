/*header.c*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "header.h"

/* Lock the semaphore n of the semaphore set semaph */

void Wait(int semaph, int n)
{
  struct sembuf sop;
  sop.sem_num = n;
  sop.sem_op =  -1;
  sop.sem_flg = 0;
  semop(semaph,&sop,1);
}

/* Unlock the semaphore n of the semaphore set semaph */
void Signal(int semaph, int n)
{
  struct sembuf sop;
  sop.sem_num = n;
  sop.sem_op =  1;
  sop.sem_flg = 0;
  semop(semaph,&sop,1);
}

/* make an array of n semaphores with key k */
int GetSemaphs(key_t k, int n)
{
  int semid, i;

  /* get a set of n semaphores with the given key k */

  if ((semid=semget(k,n,IPC_CREAT|0666))!= -1){
    for (i=0;i<n;i++)
      Signal(semid,i); /* unlock all the semaphores */
  }
  return semid;
}

// return -1 for failure, 0 for success
int ChangeName(char *fullname, struct StudentInfo *student) {
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

  // todo: trim excess whitespace and capatialize name;
  memcpy(student->fName, &fullname[0], spaces[0]*sizeof(char));
  student->fName[spaces[0]] = '\0';

  if (ct == 2) {
    student->middleInit = fullname[spaces[1]];
  }

  if ((spaces[ct] - spaces[ct-1]) > NAME_LENGTH) {
    return -1;
  }

  memcpy(student->lName, &fullname[spaces[ct-1]+1], (spaces[ct]-spaces[ct-1])*sizeof(char));
  student->lName[spaces[ct]-spaces[ct-1]-2] = '\0';

  return 0;
}

int ChangeID(char *id, struct StudentInfo *student) {
  if (strlen(id) > ID_LENGTH) {
    return -1;
  }

  memcpy(student->id, id, (strlen(id)*sizeof(char)) );
  student->id[ID_LENGTH-1] = '\0';
  return 0;
}

int ChangeAddress(char *fulladdr, struct StudentInfo *student) {
  if (strlen(fulladdr) > ADDRESS_LENGTH) {
    return -1;
  }
  // TODO: ensure address is in format: Housenum Streetname City, State ZIP

  memcpy(student->address, fulladdr, (strlen(fulladdr)*sizeof(char)) );
  student->address[strlen(fulladdr)-1] = '\0';
  return 0;
}

int ChangePhone(char *phoneNum, struct StudentInfo *student) {
  if (strlen(phoneNum) > PHONE_LENGTH) {
    return -1;
  }

  memcpy(student->telNumber, phoneNum, (strlen(phoneNum)*sizeof(char)) );
  student->telNumber[PHONE_LENGTH-1] = '\0';
  return 0;
}
