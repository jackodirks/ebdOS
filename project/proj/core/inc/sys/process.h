#ifndef PROCESS_H
#define PROCESS_H

//States are handled as different bit flags:
//pos 0 = sleep
//pos 1 = wait
//if state = 0, then it is ready
//This way, a process can both be sleeping and waiting for a mutex.

#define STATE_READY 0x0
#define STATE_SLEEP 0x1
#define STATE_WAIT 0x2

//The order of the first two is vital to make the assembly work correctly
struct Process {
    void* stackPointer; //The actual stackpointer
    unsigned pid;

    unsigned mPid;
    char* name;
    struct Process* nextProcess;
    void* stack; //refers to the address returned by Malloc
    char state; //Set of 1 bit flags
    char priority; //Higher is higher: 255 is highest  

    //Sleep vars
    unsigned sleepClockTime;
    char sleepClockOverflows;

    //Block vars
    void* blockAddress;
};

/*
    Codes:
        0: Everything ok
        1: out of PIDS
        2: Out of mem
        3: Wrong caller pid, only kernel (PID 0) can create processes
*/
int __createNewProcess(unsigned char mPid, unsigned long stacklen, char* name, void (*procFunc)(void*), void* param, char priority );

int __deleteProcess(unsigned char pid);

void __addProcessToReady(struct Process* process);

#endif