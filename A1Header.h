#define _POSIX_C_SOURCE 200809L
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>

typedef struct {
    bool logging;
    bool timing;
    char* logFileName;
}TestArgs;

typedef void* testCase(void *arg);
typedef bool equalComp(void *arg1, void *arg2);

//If args are NULL, assume that logging and timing are set to false
//if logging is true but logFileName is NULL, write test cases logs to defaultLog.txt
//This is also the function that sets up the signal handling for you
void initUnitTests(TestArgs* arg);

//This is the function that does all the work - runs the test case, displays or logs the test status + feedback, 
//times the test case, etc.
void runTest(void *refValue, testCase *testFunc, void *testArg, equalComp *compFunc);

//This deallocates all the global data you may have malloc'd, closes file descriptors, etc. 
void cleanUpUnitTests(void);

void terminationHandler(int signo);
bool comStringPtrs(void* arg1, void* arg2);


