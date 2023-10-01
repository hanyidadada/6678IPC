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
    int i = 0;
    MessageQ_QueueId QueueId;
    AttachCore(0);
    RegisterMem(0);
    MessageOpen(0, &QueueId);
    for (;;) {
        msg = MessageAlloc(0);
        printf("Running core1 function\n");
        msg->src = 1;
        msg->flags = 0x12;
        msg->heartbeat = 10 + i;
        msg->numMsgs = 1;
        msg->seqNum = i+1;
        MessagePut(QueueId, (MessageQ_Msg)msg);
        i++;
        Task_sleep(1000);
    }
}


