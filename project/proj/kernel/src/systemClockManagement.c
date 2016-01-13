#include <rom.h>            // Declare ROM addresses for rom funcs
#include <hw_types.h>       // Defines for TI libs
#include <timer.h>          // Function prototypes for timer hw interaction
#include <hw_memmap.h>      // address of GPIO etc

#include "systemClockManagement.h"
#include "kernEventNotifier.h"

#ifdef DEBUG
    #include <string.h>     // strerror

    #include "kernUtils.h"  // GenerateCrash
    #include "uartstdio.h"  // UARTprintf
#endif //DEBUG

void systemTimerInterrupt(void){
    ROM_TimerIntClear(WTIMER0_BASE,  TIMER_CAPA_MATCH|TIMER_CAPA_EVENT|TIMER_TIMA_TIMEOUT);
#ifdef DEBUG
    int code = passMessageToKernel(sysTimerOverflow);
    if (code != 0){
        UARTprintf("Failed to pass sysTimerOverflow to kernel. Code: %d (%s)\n", code, strerror(code));
        generateCrash();
    }
#else
    passMessageToKernel(sysTimerOverflow);
#endif //DEBUG
}

void sleepTimerInterrupt(void){
    ROM_TimerIntClear(WTIMER0_BASE,  TIMER_TIMB_MATCH);
#ifdef DEBUG
    int code = passMessageToKernel(sleepTimerExpired);
    if (code != 0){
        UARTprintf("Failed to pass sleepTimerExpired to kernel. Code: %d (%s)\n", code, strerror(code));
        generateCrash();
    }
#else
    passMessageToKernel(sleepTimerExpired);
#endif //DEBUG
}

unsigned getSystemClockValue(void){
    return ROM_TimerValueGet(WTIMER0_BASE, TIMER_A);
}