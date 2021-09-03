/* header.h */
#define KEY ((key_t)(71761))
#define SEGSIZE sizeof(struct StudentInfo)
#define NUM_RECORDS 50

#define NUM_SEMAPHS 5
#define SEMA_KEY ((key_t)(1761))

#define READ_KEY ((key_t)(1207))

#define NAME_LENGTH 20
#define ID_LENGTH 10
#define ADDRESS_LENGTH 60
#define PHONE_LENGTH 11

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
int GetSemaphs(key_t k, int n);
int ValidateName(char *fullname);
int ChangeName(char *fullname, struct StudentInfo *student);
int ValidateID(char *id);
int ChangeID(char *id, struct StudentInfo *student);
int ValidateAddress(char *fulladdr);
int ChangeAddress(char *fulladdr, struct StudentInfo *student);
int ValidatePhone(char *phoneNum);
int ChangePhone(char *phoneNum, struct StudentInfo *student);
