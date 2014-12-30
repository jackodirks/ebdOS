#include "defenitions.h"
#include "process.h"
#include "processManagement.h"
#include "string.h"
#include "stdlib.h"
#include "asmUtils.h"
#include "uartstdio.h"
#include "process.h"
#include "supervisorCall.h" 
#include "utils.h"
#include "lm4f120h5qr.h" //Hardware regs

//Responsible for creating and managing processes

struct Process* processesReady = NULL;
struct Process* kernel = NULL;
extern struct Process* currentProcess;
extern struct Process* nextProcess;
struct Process* sleepProcess; //Runs when no other process wants to run
struct Process* hibernateProcess; //Runs when there are no other processes left and the kernel has nothing to do either
//Usefull for selecting the pids
static unsigned char nextPid = 1; //Short, has to be able to become 256
//TODO create system to find out which pids are in use and which not.
#define MAX_PROCESSID (254)

void __processReturn(void){
    UARTprintf("Process %s with pid %d has just returned.\r\n",currentProcess->name, currentProcess->pid);
    //TODO tell kernel about this stuff and let it cleanup the mess
    currentProcess->state = STATE_WAIT;
    while(1){
        CALLSUPERVISOR(SVC_reschedule);
    }
}

void __addProcessToReady(struct Process* process){
    if (!__processInList(processesReady, process)){
        processesReady = __sortProcessIntoList(processesReady, process);
    }
}

void __removeProcessFromReady(struct Process* process){
    __removeProcessFromList(processesReady, process);
}

int __createNewProcess(unsigned mPid, unsigned long stacklen, char* name, void (*procFunc)(void*), void* param, char priority){
    if (priority == 255) priority = 254; //Max 254, 255 is kernel only
    if (currentProcess->pid != 1) {
        return 3;
    } 
    if ( nextPid > MAX_PROCESSID ) {
        return 1;
    }
    //Create the process on the heap
    struct Process* newProc = (struct Process*)malloc(sizeof(struct Process)); 
    if ( newProc == NULL) { //Insufficient mem
       return 2; 
    }
    if (stacklen < 67){
        return 4;
    }
    //Set some vars
    newProc->pid = nextPid++;
    newProc->mPid = mPid;
    newProc->nextProcess = NULL;
    newProc->priority = priority;
    newProc->state = STATE_READY;
    newProc->blockAddress = NULL;
    newProc->sleepObjAddress = NULL;
    newProc->name = (char*)malloc(strlen(name)+1);
    if (newProc->name == NULL){
        free(newProc);
        return 2;
    }
    strcpy(newProc->name,name);
    //Create the stack.
    void* stack = malloc(stacklen);
    if (stack == NULL){
        free(newProc->name);
        free(newProc);
        return 2;
    }
    newProc->stack = stack;
    //Because a stack moves up (from high to low) move the pointer to the last address and then move it back up to a position where for the address of the pointer lsb and lsb+1 = 0 (lsb and lsb+1 of SP are always 0)
    int* stackPointer = (int*)((((long)newProc->stack) + stacklen - 4) & (long)0xFFFFFFFC ); 
    //Now start pushing registers
    //The first set of registers are for the interrupt handler, those will be read when the system returns from an interrupt
    //These are in order from up to down: R0, R1, R2, R3, R12, LR, PC, XSPR
    *stackPointer-- = 0x01000000; //XPSR, standard stuff 
    *stackPointer-- = (int)procFunc; //PC, initally points to start of function
    *stackPointer-- = (int)&__processReturn; //LR, return func
    *stackPointer-- = 12; // reg12, 12 for debug
    *stackPointer-- = 3; // reg3, 3 for debug
    *stackPointer-- = 2; // reg2, 2 for debug
    *stackPointer-- = 1; // reg1, 1 for debug
    *stackPointer-- = (int)param; // reg 0, first param

    //The second set is the registers that we have to move manually between RAM and regs when switching contextst
    //Order: R4, R5, R6, R7, R8, R9, R10, R11
    for ( int u = 11; u > 4; u-- ){
        *stackPointer-- = u; //Reg u, u for debug
    }  
    *stackPointer = 4;
    //Save the stackpointer to the struct
    newProc->stackPointer = (void*)stackPointer;

    //Add the new process to the list of processes
    __addProcessToReady(newProc);
    return 0; 
}


void __sleepProcessFunc(void* param){
    UARTprintf("sleepProcessFunc!\r\n");
    while(1){
        waitForInterrupt();
    }
}

void __hibernateProcessFunc(void* param){
    UARTprintf("No more processes...\r\n");
    __sleepProcessFunc(NULL);
    //TODO make it, you know, hibernate
}
