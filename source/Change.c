// File: Change.c
// Purpose: Allows advisor to change any informtion about an existing student
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

int GetStudentIndex(int sema_set, int *readptr, struct StudentInfo *infoptr) {
  char input[ID_LENGTH];
  fflush(stdin);
  if (fgets(input, sizeof(input), stdin) ==  NULL) {
    exit(0);
  }

  input[strlen(input)] = '\0';
  if (ValidateID(input) != 0){
    exit(2);
  }

  IncramentReadCount(sema_set, readptr);

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

  DecramentReadCount(sema_set, readptr);

  if (stuIdx == -1) {
    printf("Student with that ID doesn't exist!\n");
    exit(2);
  }

  return stuIdx;
}

int main() {
  int id = 0;
  int read_id = 0;
  // Attach the shared memory segments;
  struct StudentInfo *infoptr = GetSharedMemories(&id);
  // Get pointer to read counter
  int *readptr=GetReadCounter(&read_id);
  // Get the id of the semaphore set created in Load.c;
  int sema_set=semget(SEMA_KEY, 0, 0);

  char advisor[10];
  // Prompt the user for password;
  if (ValidatePassword(advisor) == -1) {
    exit(1);
  }
  advisor[9] = '\0';

  char in;
  printf("Please enter an option: \n A - Add new student\n D - Delete existing student\n C - Change existing student info\nPlease enter your selection: ");
  scanf("%c", &in);

  int stuIdx = 0;
  if (in == 'A') {
    // find a valid, unique ID
    char newID[ID_LENGTH];

    while (1==1) {
      // generate a random ID
      int idValid = 0;
      for (int i = 0; i < ID_LENGTH; i++) {
        int r = rand() % 10;
        newID[i] = r + '0';
      }
      newID[ID_LENGTH-1] = '\0';
      // ensure the random ID is unique
      IncramentReadCount(sema_set, readptr);
      for (int i = 0; i < NUM_RECORDS; i++) {
        if (strcmp(infoptr[i].id, newID) == 0) {
          idValid = -1;
          break;
        }
      }
      DecramentReadCount(sema_set, readptr);

      // when random ID is valid, stop trying to generate IDs
      if (idValid == 0) {
        break;
      }
    }

    // find a valid slot to put the new record
    IncramentReadCount(sema_set, readptr);
    for (int i = 0; i < NUM_RECORDS; i++) {
      if (ValidateID(infoptr[i].id) == -1) {
        stuIdx = i;
        break;
      }
    }
    DecramentReadCount(sema_set, readptr);

    if (stuIdx == NUM_RECORDS) {
      printf("Student records are full. New entry can not be created.\n");
      exit(2);
    }

    printf("Please enter the student's name: ");
    char name[NAME_LENGTH*2];
    fflush(stdin);
    fgets(name, sizeof(name), stdin);
    if (ValidateName(name) == -1) {
      printf("Invalid name. New student will not be created.\n");
      exit(2);
    }

    printf("Please enter the student's address: ");
    char address[ADDRESS_LENGTH];
    fflush(stdin);
    fgets(address, sizeof(address), stdin);
    if (ValidateAddress(address) == -1) {
      printf("Invalid address. New student will not be created.\n");
      exit(2);
    }

    printf("Please enter the student's phone number (no dashes or parenthesis): ");
    char phone[PHONE_LENGTH];
    fflush(stdin);
    fgets(phone, sizeof(phone), stdin);
    if (ValidatePhone(phone) == -1) {
      printf("Invalid phone num. New student will not be created.\n");
      exit(2);
    }

    Wait(sema_set, 0);  // writing new record, lock semaph

    sleep(2);

    ChangeName(name, &infoptr[stuIdx]);
    ChangeID(newID, &infoptr[stuIdx]);
    ChangeAddress(address, &infoptr[stuIdx]);
    ChangePhone(phone, &infoptr[stuIdx]);
    strcpy(infoptr[stuIdx].whoModified, advisor);

    printf("Student created!\n");
    PrintStudent(&infoptr[stuIdx]);

    Signal(sema_set, 0);  // done creating student
  }
  else if (in == 'D') {
    printf("Enter ID of student to delete: ");
    stuIdx = GetStudentIndex(sema_set, readptr, infoptr);

    Wait(sema_set, 0);  // lock while writing

    printf("Destroying record of:\n");
    PrintStudent(&infoptr[stuIdx]);

    sleep(2);

    strcpy(infoptr[stuIdx].fName, "");
    strcpy(infoptr[stuIdx].lName, "");
    infoptr[stuIdx].middleInit = '\0';
    strcpy(infoptr[stuIdx].id, "");
    strcpy(infoptr[stuIdx].address, "");
    strcpy(infoptr[stuIdx].telNumber, "");
    strcpy(infoptr[stuIdx].whoModified, "");

    Signal(sema_set, 0);  // done writing
  }
  else if (in == 'C') {
    printf("Enter ID of student to change: ");
    stuIdx = GetStudentIndex(sema_set, readptr, infoptr);

    printf("What would you like to change?\n N - Student's name\n I - Student's ID\n A - Student's address\n T - Student's telephone number\nPlease enter your selection: ");
    fflush(stdin);
    scanf("%c", &in);
    if (in == 'N') {
      printf("Please enter the student's new name: ");
      char name[NAME_LENGTH*2];
      fflush(stdin);
      fgets(name, sizeof(name), stdin);
      if (ValidateName(name) == -1) {
        printf("Invalid name. Change will not occur.\n");
        exit(2);
      }

      Wait(sema_set, 0);  // writing new name, lock semaph
      sleep(2);
      ChangeName(name, &infoptr[stuIdx]);
      Signal(sema_set, 0);  // done creating student
    }
    else if (in == 'I') {
      printf("Please enter the student's new ID: ");
      char newID[ID_LENGTH];
      fflush(stdin);
      fgets(newID, sizeof(newID), stdin);
      if (ValidateID(newID) == -1) {
        printf("Invalid ID. Change will not occur.\n");
        exit(2);
      }

      // ensure the new ID is unique
      int idValid = 0;
      IncramentReadCount(sema_set, readptr);
      for (int i = 0; i < NUM_RECORDS; i++) {
        if (strcmp(infoptr[i].id, newID) == 0) {
          idValid = -1;
          break;
        }
      }
      DecramentReadCount(sema_set, readptr);

      if (idValid == -1) {
        printf("Another student already has that ID. Change will not occur\n");
      }

      Wait(sema_set, 0);  // writing new name, lock semaph
      sleep(2);
      ChangeID(newID, &infoptr[stuIdx]);
      Signal(sema_set, 0);  // done editing student
    }
    else if (in == 'A') {
      printf("Please enter the student's new address: ");
      char address[ADDRESS_LENGTH];
      fflush(stdin);
      fgets(address, sizeof(address), stdin);
      if (ValidateAddress(address) == -1) {
        printf("Invalid address. Change will not occur.\n");
        exit(2);
      }

      Wait(sema_set, 0);  // writing new name, lock semaph
      sleep(2);
      ChangeAddress(address, &infoptr[stuIdx]);
      Signal(sema_set, 0);  // done editing student
    }
    else if (in == 'T') {
      printf("Please enter the student's new telephone number: ");
      char phone[PHONE_LENGTH];
      fflush(stdin);
      fgets(phone, sizeof(phone), stdin);
      if (ValidatePhone(phone) == -1) {
        printf("Invalid phone number. Change will not occur.\n");
        exit(2);
      }

      Wait(sema_set, 0);  // writing new name, lock semaph
      ChangePhone(phone, &infoptr[stuIdx]);
      Signal(sema_set, 0);  // done editing student
    }
    else {
      printf("Option is invalid!\n");
      exit(2);
    }

    printf("Change successful! Showing updated info:\n");
    Wait(sema_set, 0);
    strcpy(infoptr[stuIdx].whoModified, advisor);
    Signal(sema_set, 0);

    IncramentReadCount(sema_set, readptr);
    PrintStudent(&infoptr[stuIdx]);
    DecramentReadCount(sema_set, readptr);
  }
  else {
      printf("Option invalid. Terminating...\n");
      exit(2);
    }

}
