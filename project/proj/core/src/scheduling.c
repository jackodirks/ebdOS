#include "process.h"
#include "processManagement.h"
#include "stdlib.h"
#include "asmUtils.h"
#include "uartstdio.h"
#include "sleep.h"
#include "lm4f120h5qr.h" //Hardware regs


//Defines for correct returns
// See datasheet pp 106
// These values are now used in the assembly, but for references sake
#define PSPNONFP_RETURN ((unsigned)0xFFFFFFFD)
#define MSPNONFP_RETURN ((unsigned)0xFFFFFFF9)
#define PSPFP_RETURN    ((unsigned)0xFFFFFFED)
#define MSPFP_RETURN    ((unsigned)0xFFFFFFE9)

// This file handles scheduling, basically

extern struct Process* kernel;
extern struct Process* currentProcess;
extern struct Process* firstProcess;
extern struct Process* nextProcess;
struct Process* sleepProcess; //Runs when no other process wants to run
struct Process* hibernateProcess; //Runs when there are no other processes left and the kernel has nothing to do either

void schedule(void){
    unsigned long sleepCounterValue = getCurrentSleepTimerValue();
    //Actual scheduling.
    nextProcess = NULL;
    //If the kernel is ready, kernel is nextprocess
    if (kernel->state == READY){
        nextProcess = kernel;
    } else if (firstProcess == NULL){
        nextProcess = hibernateProcess;
    } else {
        //The actual selection
        for (struct Process* proc = firstProcess; proc != NULL; proc = proc->nextProcess ){
            if (nextProcess == NULL || proc->priority > nextProcess->priority){
                if (proc->state == READY || proc->state == SLEEP){
                    if (proc->state == SLEEP){
                        if (proc->sleepClockOverflows == 0 && proc->sleepClockTime <= sleepCounterValue){
                            proc->sleepClockTime = 0;
                            proc->state = READY;
                        }
                    } 
                    if (proc->state == READY){
                        nextProcess = proc;
                    }
                }
            }
        }
    }
    if (nextProcess == NULL){
        nextProcess = sleepProcess;
    } 
    //Generate the correct interrupt to actually switch context
    NVIC_INT_CTRL_R |= (1<<28);
}

//For future reference
/**
void PendSVHandler(void){
    //We are assuming nextProcess is not null
    //The PSP always contains the location where the registers are written to, including when switching from and to kernel
    volatile unsigned* msp = getMSP();
    currentProcess->stackPointer = saveRegistersToPSPAndMovePSP();   
    setPSP(nextProcess->stackPointer);
    loadRegistersFromPSPAndMovePSP();
    if (nextProcess->pid == 0){
        msp[3] = MSPNONFP_RETURN;
    } else {
        msp[3] = PSPNONFP_RETURN;
    }
    currentProcess = nextProcess;
}
**/
