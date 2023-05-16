# Shared Memory Data Synchronization
## Author: Michael Probst

This repository contains the resources for an assignment for an operating systems class I took in grad school. The goal is to synchronize data in shared memory.

## Summary

This project contains several programs that work on a small set of student records. These records are stored in a shared memory segment, and are accessed by several programs that use semaphores for data synchronization. 

## Program Files

Each of the programs that comprise this project are implemented by the following files:

### Load
usage: ./load <file.txt> 
Load takes a text file containg student records where the lines of text are separated by newlines and appear in the order: "student name", "student id", "student address", "student telephone number".
It is assumed that the info in the input file is correctly formatted.
If there are errors in the input file, they will be printed and the current student will not be added to the records.

Load also creates the shared memory segments and semaphores used by the other programs.

__RUN LOAD PRIOR TO RUNNING OTHER PROGRAMS__ otherwise, there will be no student records to access.

### Print
usage: ./print
Prints the current contents of the shared memories and values of the semaphores.
This program is primairly used for debugging.

### Query
usage: ./query
Query allows any user to gather information about any student when entering the id of a student that exists in the records. 

### Change
usage: ./change
Change allows an advisor to modify the student records in 3 ways:
1. Add: create a new student for the records. The advisor will then enter the student's name, address, and telephone number. The student's ID will be generated so that it does not match any of the IDs of existing students. If the student's name is longer than 40 characters, it will not be accepted. If the student's address is longer than 50 characters, it will not be accepted. If the student's telephone is not 10 numbers long, it will not be accepted.

2. Delete: given a valid student ID, destroys a student's information from the records.

3. Change: given a valid student ID, changes any of the information of a student's record (excluding ID). Each piece of information in the record faces the same input requirements as described in the "Add" section just above.
 
### Clear
usage: ./clear
__NOTE: NAME CHANGED FROM CLEAN TO CLEAR__ to distinguish from `make clean`
Clear destroys the shared memory segments and semaphores after saving the current state of the records to a text file called "output.txt"

## Common Files

The following files contain definitions and functions that are used by several of the programs in this project.

### header
header.h defines all global constants used in the projects, defines the StudentInfo struct and common functions.

header.c implements the functions defined in header.h

Both of these files were heavily based on the sample code provided by Dr. Mani.

### password
password.h and password.c implement functions that require a user to give their username and password in order to continue using the current program. The user gets 3 attempts to enter the password, which is always "000".

### makefile
usage: make all - creates all programs
usage: make clean - destroys all programs, object files, and the output file.
the makefile is a handy tool to create the programs needed to run this project.
