#ifndef SUPERVISORCALL_H
#define SUPERVISORCALL_H

// Helper macros
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// The codes
#define SVC_yield                               0
#define SVC_wakeupKernel                        1
#define SVC_serviceRequired                     2
#define SVC_serviced                            3
#define SVC_abort                               4
#ifdef DEBUG
#define SVC_test                                255
#endif // DEBUG

// Contains the macro to create the supervisor call
#define CALLSUPERVISOR(code) __asm__("svc "STR(code));

#endif // SUPERVISORCALL_H
