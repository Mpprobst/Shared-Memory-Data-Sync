#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "password.h"

int EnterPassword(int attempt) {
  printf("Please enter the password (20 char limit): ");
  char input[21];
  fgets(input, sizeof(input), stdin);
  for (int i = 0; i < strlen(password); i++) {
    if (password[i] != input[i]) {
      return -1;
    }
  }
  printf("Password correct!");
  return 0;
}

int ValidatePassword() {
  for (int i = 0; i < NUM_ATTEMPTS; i++) {
    if (EnterPassword(i) == 0) {
      return 0;
    }
    else {
      printf("Invalid password. %i attempts remaining.\n", NUM_ATTEMPTS-i-1);
    }
  }
  printf("Too many failed attempts. Aborting.\n");

  return -1;
}
