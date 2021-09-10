// File: header.h
// Purpose: defines global constants and StudentInfo struct
// Author: Michael Probst

#define KEY ((key_t)(71761))
#define SEGSIZE sizeof(struct StudentInfo)
#define NUM_RECORDS 50

#define NUM_SEMAPHS 2 // semaph 0 is for data, semaph 1 is for read counter
#define SEMA_KEY ((key_t)(1761))

#define READ_KEY ((key_t)(1207))

#define NAME_LENGTH 20
#define ID_LENGTH 10
#define ADDRESS_LENGTH 60
#define PHONE_LENGTH 11

// Acknowldegement: This struct provided by project sample code
struct StudentInfo{
  char fName[NAME_LENGTH];
  char lName[NAME_LENGTH];
  char middleInit;
  char id[ID_LENGTH];
  char address[ADDRESS_LENGTH];
  char telNumber[PHONE_LENGTH];
  char whoModified[10];
};

void Wait(int semaph, int n);
void Signal(int semaph, int n);
void PrintSemaph(int semaph, int n);
int GetSemaphs(key_t k, int n);
struct StudentInfo *GetSharedMemories(int *id);
int *GetReadCounter(int *read_id);
void IncramentReadCount(int sema_set, int *readptr);
void DecramentReadCount(int sema_set, int *readptr);
int ValidateName(char *fullname);
int ChangeName(char *fullname, struct StudentInfo *student);
int ValidateID(char *id);
int ChangeID(char *id, struct StudentInfo *student);
int ValidateAddress(char *fulladdr);
int ChangeAddress(char *fulladdr, struct StudentInfo *student);
int ValidatePhone(char *phoneNum);
int ChangePhone(char *phoneNum, struct StudentInfo *student);
void PrintStudent(struct StudentInfo *s);
