/*
 *  ======== mutex.c ========
 *  This example shows the use of two tasks and one semaphore to perform
 *  mutual exclusive data access.
 */
#include <stdio.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <xdc/cfg/global.h>

#include "coremessage/coreipc.h"

Void task1(UArg arg0, UArg arg1);

Int resource = 0;
Task_Handle tsk1;

extern MessageQ_Handle messageQ;
/*
 *  ======== main ========
 */
int main()
{ 
    Task_Params taskParams;

    /* Create a Semaphore object to be use as a resource lock */
    InitIpc();
    /* Create two tasks that share a resource*/
    Task_Params_init(&taskParams);
    taskParams.priority = 1;
    tsk1 = Task_create (task1, &taskParams, NULL);

    BIOS_start();    /* does not return */
    return(0);
}


/*
 *  ======== task1 ========
 */
void task1(UArg arg0, UArg arg1)
{
    CoreMsg *msg;
    AttachCore(1);
    RegisterMem(0);
    MessageCreate();
    for (;;) {
        printf("Running core0 function\n");
        MessageGet(messageQ, (MessageQ_Msg*)&msg);
        printf("src:%d, flags:%d, numMsgs:%d, seqNum:%d, heartbeat:%d \n", msg->src, msg->flags, msg->numMsgs, msg->seqNum, msg->heartbeat);
        MessageFree((MessageQ_Msg)msg);
        Task_sleep(1000);
    }
}


