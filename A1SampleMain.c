#include "A1Header.h"

bool comIntPtrs(void* arg1, void* arg2){
    int *n1 = (int*)arg1;
    int *n2 = (int*)arg2;
    if (*n1 == *n2){
        return true;
    }else{
        return false;
    }
}

bool comFloatPtrs(void* arg1, void* arg2){
    float *n1 = (float*)arg1;
    float *n2 = (float*)arg2;
    if (*n1 == *n2){
        return true;
    }else{
        return false;
    }
}

bool comDoublePtrs(void* arg1, void* arg2){
    double *n1 = (double*)arg1;
    double *n2 = (double*)arg2;
    if (*n1 == *n2){
        return true;
    }else{
        return false;
    }
}

bool comStringPtrs(void* arg1, void* arg2){
    char *n1 = (char*)arg1;
    char *n2 = (char*)arg2;

    int c = strcmp(n1, n2);

    if (c == 0){
        return true;
    }else{
        return false;
    }
}

void* add1int(void *arg){
    int* num = arg;
    *num = *num + 1;

    return arg;

}

void* sigsegv(void *arg){
    int* num = arg;
    *num = *num + 1;

    raise(SIGSEGV);

    return arg;
}

void* sigquit(void *arg){
    int* num = arg;
    *num = *num + 1;

    raise(SIGQUIT);

    return arg;
}

int main(void){ 

    /*The initUnitTest function makes a copy of arg into a global variable and then frees arg*/

    // TestArgs *arg = malloc(sizeof(TestArgs));
    // char file[10];
    // strcpy(file, "test.txt");
    // arg->logging = true;
    // arg->timing = false;
    // arg->logFileName = file;
 
    initUnitTests(NULL);

    //correct termination passes
    int expRes = 3;
    int testArg = 2;
    runTest(&expRes, add1int, &testArg, comIntPtrs);

    // correct termination fails
    testArg = 0;
    runTest(&expRes, add1int, &testArg, comIntPtrs);

    //test terminated by SIGSEGV
    runTest(&expRes, sigsegv, &testArg, comIntPtrs);

    //test terminated by SIGQUIT
    runTest(&expRes, sigquit, &testArg, comIntPtrs);
    
    cleanUpUnitTests();

    return 0;

}