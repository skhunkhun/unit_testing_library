#include "A1Header.h"

TestArgs *test;
int testId = 0;
bool testargs = true;
bool logging = false;
bool timing = false;
bool fileOpen = false;
char *filename;
pid_t child_pids[5], wpid;
int numPids = 0;
FILE *fp;
int fd[2];

void terminationHandler(int signo){ //all signal handling

    signal(SIGINT, terminationHandler);

    if(signo == SIGINT){

        char answer[10];
        bool menu = true;
            
        while(menu == true){
            printf("Are you sure you want to terminate (Y or N)\n");
            scanf("%s", answer);

            if(strcmp(answer, "Y") == 0 || strcmp(answer, "y") == 0){
                menu = false;
                printf("Cleaning up and terminating...\n");
                cleanUpUnitTests();
                kill(getpid(), SIGTERM);

            } else if(strcmp(answer, "N") == 0 || strcmp(answer, "n") == 0){
                menu = false;
                printf("Resuming execution of current process...\n");
                return;

            } else {
                printf("\nPlease enter Y or N\n");
            }
        }
    } 
    
    if(signo == SIGSEGV){
        cleanUpUnitTests();
        kill(getpid(), SIGSEGV);

    } 
    
    if(signo == SIGQUIT){
        cleanUpUnitTests();
        kill(getpid(), SIGQUIT);
    }

    if(signo == SIGPIPE){
        cleanUpUnitTests();
        kill(getpid(), SIGTERM);
    }

    if(signo == SIGALRM){
        cleanUpUnitTests();
        kill(getpid(), SIGTERM);
    }


}

void initUnitTests(TestArgs* arg){  //The initUnitTest function makes a copy of arg into a global struct 'test' and then frees arg

    //initialize signal handling
    if(signal(SIGINT, terminationHandler)  == SIG_ERR)
        fprintf(stderr, "Can't catch SIGINT");
    if(signal(SIGSEGV, terminationHandler)  == SIG_ERR)
        fprintf(stderr, "Can't catch SIGSEGV");
    if(signal(SIGQUIT, terminationHandler)  == SIG_ERR)
        fprintf(stderr, "Can't catch SIGQUIT");
    if(signal(SIGPIPE, terminationHandler)  == SIG_ERR)
        fprintf(stderr, "Can't catch SIGPIPE");
    if(signal(SIGALRM, terminationHandler)  == SIG_ERR)
        fprintf(stderr, "Can't catch SIGPIPE");

    if(arg == NULL){
        testargs = false;
        return;
    }

    if(arg->logging == true){
        logging = true;
        if(arg->logFileName == NULL){
            filename = malloc(16);
            strcpy(filename, "defaultLog.txt");
        } else {
            filename = malloc(strlen(arg->logFileName) + 1);
            strcpy(filename, arg->logFileName);
        }

        fp = fopen(filename, "w");
          
    } else {
        logging = false;
    }

    if(arg->timing == true){
        timing = true;
    } else {
        timing = false;
    }

    test = malloc(sizeof(TestArgs));
    memcpy(test, arg, sizeof(TestArgs));
    free(arg);
}

void cleanUpUnitTests(void){

    if(testargs == true){
        free(test);
        if(logging == true){
            free(filename);
            fclose(fp);
        }
    }
}

void runTest(void *refValue, testCase *testFunc, void *testArg, equalComp *compFunc){

    if(refValue == NULL || testFunc == NULL || testArg == NULL || compFunc == NULL){
        printf("\nNO NULL ARGUMENTS\n");
        return;
    }
    
    testId++;

    clock_t start;
    clock_t end;
    double runningTime = 0.0; //hold the running time of the program

    start = clock(); //start time

    pid_t child_pid;
    int status;

    if (pipe(fd) == -1){
        perror("pipe");
        exit(1);
    }
   
    child_pid = fork(); //create new process

    if(child_pid >= 0){

        if(child_pid == 0){

            close(fd[0]);
            char test[5];

            void* resVal = testFunc(testArg); //test function

            if (compFunc(refValue, resVal))
                strcpy(test, "PASS");
            
            else
                strcpy(test, "FAIL");
            
            write(fd[1], test, 5); //send test results to parent
                
        } else {

            signal(SIGINT, SIG_IGN);//ignore SIGINT signal handler

            close(fd[1]);

            char readbuffer[5];
 
            if(logging == true){
                int filedesc = open(filename, O_WRONLY | O_APPEND);
                dup2(filedesc, STDOUT_FILENO);
                close(filedesc);
            }
           
            

            wpid = wait(&status);
                
            if (WIFEXITED(status)) { //if child terminated voluntarily, output the correct results, cleanup, and exit process.
                read(fd[0], readbuffer, sizeof(readbuffer));

                end = clock(); //end time
                runningTime = ((double)(end-start))/CLOCKS_PER_SEC; //calculate running time in milliseconds

                if(timing == true && logging == true) {
                    fprintf(stdout, "TestId: %d\nTest status: %s\nTest feedback: Timing(%f seconds)\n\n", testId, readbuffer, runningTime);
                } else if(timing == true && logging == false) {
                    fprintf(stdout, "\nTestId: %d\nTest status: %s\nTest feedback: Timing(%f seconds)\n\n", testId, readbuffer, runningTime);
                } else if(logging == true && timing == false) {
                    fprintf(stdout, "TestId: %d\nTest status: %s\nTest feedback:\n\n",testId, readbuffer);
                } else {
                    printf("\nTestId: %d\nTest status: %s\nTest feedback: \n\n", testId, readbuffer);
                    fflush(stdout);
                }

                cleanUpUnitTests();
                exit(0);
                

            } else if (WIFSIGNALED(status)){ //if child process is terminated abnormally, then output results, cleanup, and exit
                signal(SIGINT, terminationHandler);
                char* exitSignalStr = strsignal(WTERMSIG(status));

                end = clock(); //end time
                runningTime = ((double)(end-start))/CLOCKS_PER_SEC; //calculate running time in milliseconds

                if(logging == true && timing == true){
                    fprintf(stdout, "TestId interrupted: %d\nTest feedback: Child %d exited with status %d (%s). Timing(%f seconds).\n\n",testId, wpid, WTERMSIG(status), exitSignalStr, runningTime);
                    if(WTERMSIG(status) == 15){
                        cleanUpUnitTests();
                        exit(1);
                    }

                } else if(timing == true && logging == false){
                    fprintf(stdout, "TestId interrupted: %d\nTest feedback: Child %d exited with status %d (%s). Timing(%f seconds).\n\n",testId, wpid, WTERMSIG(status), exitSignalStr, runningTime);
                    if(WTERMSIG(status) == 15){
                        cleanUpUnitTests();
                        exit(1);
                    }
                } else if(logging == true && timing == false) {
                    fprintf(stdout, "TestId interrupted: %d\nTest feedback: Child %d exited with status %d (%s).\n\n",testId, wpid, WTERMSIG(status), exitSignalStr);
                    if(WTERMSIG(status) == 15){
                        cleanUpUnitTests();
                        exit(1);
                    }
                    
                } else {
                    fprintf(stdout, "TestId interrupted: %d\nTest feedback: Child %d exited with status %d (%s).\n\n",testId, wpid, WTERMSIG(status), exitSignalStr);
                    if(WTERMSIG(status) == 15){
                        cleanUpUnitTests();
                        exit(1);
                    }
                }

            }
        }

    } else {

        fprintf(stderr, "\nError with fork()!\n");
        exit(1);
    }
    
}