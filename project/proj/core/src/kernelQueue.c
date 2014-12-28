#include "process.h"
#include "stdlib.h"
#include "kernelQueue.h"
#include "string.h"
#include "supervisorInterrupt.h"

struct KernelQueue* kernelQueue = NULL;
extern struct Process* currentProcess;

void pushItem(struct KernelQueueItem* item){
    lockMutexBlocking(kernelQueue->listProtectionMutex);
    struct KernelQueueItem* currentItem = kernelQueue->firstItem;
    struct KernelQueueItem* prevItem = NULL;
    while(currentItem != NULL){
        prevItem = currentItem;
        currentItem = currentItem->nextItem;
    }
    if (prevItem == NULL){
        kernelQueue->firstItem = item;
    } else {
        prevItem->nextItem = item;
        item->nextItem = currentItem;
    }
    releaseMutex(kernelQueue->listProtectionMutex);
    increaseSemaphoreBlocking(kernelQueue->readyItems);
}

void __addSleeper(struct Process* process, unsigned sleepUntil, unsigned overflowsV){
    increaseSemaphoreBlocking(kernelQueue->existingItems);
    //Try to create all necessary datastructures
    struct KernelQueueItem* item = (struct KernelQueueItem*)malloc(sizeof(struct KernelQueueItem));
    struct NewProcess* newProcess = NULL;
    if (item != NULL){
        newProcess = (struct NewProcess*)malloc(sizeof(struct NewProcess));
        if (newProcess != NULL){
            newProcess->name = (char*)malloc(strlen(name)+1);
            if (newProcess->name == NULL){
                free(newProcess);
                free(item);
                item = NULL;
            }
        } else {
                free(item);
                item = NULL;
        }
        if (item == NULL){
            decreaseSemaphoreBlocking(kernelQueue->existingItems);
            //TODO notify supervisor
            return;
        }
    }
    
}   

void createProcess(unsigned long stacklen, char* name, void (*procFunc)(void*), void* param, char priority){
    increaseSemaphoreBlocking(kernelQueue->existingItems);
    //Try to create all necessary datastructures
    struct KernelQueueItem* item = (struct KernelQueueItem*)malloc(sizeof(struct KernelQueueItem));
    struct NewProcess* newProcess = NULL;
    if (item != NULL){
        newProcess = (struct NewProcess*)malloc(sizeof(struct NewProcess));
        if (newProcess != NULL){
            newProcess->name = (char*)malloc(strlen(name)+1);
            if (newProcess->name == NULL){
                free(newProcess);
                free(item);
                item = NULL;
            }
        } else {
                free(item);
                item = NULL;
        }
        if (item == NULL){
            decreaseSemaphoreBlocking(kernelQueue->existingItems);
            //TODO notify supervisor
            return;
        }
    }

    newProcess->mPid = currentProcess->pid;
    newProcess->stacklen = stacklen;
    strcpy(newProcess->name, name);
    newProcess->procFunc = procFunc;
    newProcess->param = param;
    newProcess->priority = priority;
    item->itemtype = newprocess;
    item->item = (void*) newProcess;
    pushItem(item);
}