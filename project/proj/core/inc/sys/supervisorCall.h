//The codes
#define SVC_reschedule "0"

#define SVC_objectLock "1"
#define SVC_objectRelease "2"

#define SVC_multiObjectIncrease "3"
#define SVC_multiObjectDecrease "4"
#define SVC_multiObjectWaitForIncrease "5"
#define SVC_multiObjectWaitForDecrease "6"

#define SVC_setKernelPrioMax "7"

#define SVC_sleep "8"
#define SVC_sleepNoBlock "9"
#define SVC_wakeup "10"

#define SVC_processAdd "11"

#ifdef DEBUG
#define SVC_test "255"
#endif //DEBUG
//Contains the macro to create the supervisor call
#define CALLSUPERVISOR(code) __asm__("svc "code); //Inject the code into the ASM
