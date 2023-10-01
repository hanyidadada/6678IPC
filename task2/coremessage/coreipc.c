/*
 * coreipc.c
 *
 *  Created on: 2023年9月26日
 *      Author: hanyi
 */

#include <stdio.h>

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Error.h>

/* XDC.RUNTIME module Headers */
#include <xdc/runtime/System.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Timestamp.h>

/* IPC module Headers */
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/SharedRegion.h>
/* IPC modules */
#include <ti/ipc/Notify.h>
#include <ti/ipc/Ipc.h>

/* PDK module Headers */
#include <ti/platform/platform.h>

/* BIOS6 module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* CSL modules */
#include <ti/csl/csl_chip.h>

// for memset, what do i include for memset?
#include <ti/drv/qmss/qmss_drv.h>

#include "coremessage/coreipc.h"

char *QueueName[NUM_CORE] = {"CORE0", "CORE1", "CORE2", "CORE3", "CORE4", "CORE5", "CORE6", "CORE7"};
MessageQ_Handle messageQ = NULL;


int InitIpc(void)
{
    SharedRegion_Entry entry;
    Int status;
    UInt16 clusterBaseId = MultiProc_getBaseIdOfCluster();

    /* Call Ipc_start() */
    status = Ipc_start();
    if (status < 0) {
        printf("Ipc_start failed!\n");
        return -1;
    }

    /* get region 0 information */
    SharedRegion_getEntry(0, &entry);

    /* if entry is not valid then return */
    if (entry.isValid == FALSE) {
        printf("entry is not valid failed!\n");
        return -1;
    }

    /* Must attach to owner first to get default GateMP */
    if (MultiProc_self() != entry.ownerProcId) {
        do {
            status = Ipc_attach(entry.ownerProcId);
        } while (status < 0);
    }
    return 0;
}

int AttachCore(int coreNum)
{
    SharedRegion_Entry entry;
    Int status;
    SharedRegion_getEntry(0, &entry);

    if ((coreNum == MultiProc_self()) ||
         (coreNum == entry.ownerProcId)) {
        return -1;
    }

    if (Notify_numIntLines(coreNum) == 0) {
        return -1;
    }

    /* call Ipc_attach for every remote processor */
    do {
        status = Ipc_attach(coreNum);
    } while (status < 0);

    return 0;
}

int MessageCreate(void)
{
    messageQ = MessageQ_create(QueueName[MultiProc_self()], NULL);
    if (messageQ == NULL) {
      System_abort("MessageQ_create failed\n" );
      return -1;
    }
    return 0;
}

int RegisterMem(int heapid)
{
    MessageQ_registerHeap((IHeap_Handle)SharedRegion_getHeap(0), heapid);
    return 0;
}

// 打开对方的MessageQ，corenum为对方的core号，QueueID为传出参数，作为handle，下一步message_put使用
int MessageOpen(int corenum, MessageQ_QueueId *QueueId)
{
    int32_t status;
    do {
        status = MessageQ_open(QueueName[corenum], QueueId);
        Task_yield();
    }while (status < 0);
    return 0;
}

CoreMsg * MessageAlloc(int headid)
{
    CoreMsg *msg;
    msg = (CoreMsg *)MessageQ_alloc(headid, MESSAGE_SIZE_IN_BYTES);
    if (msg == NULL) {
       printf("MessageQ_alloc failed\n");
       return NULL;
    }
    return msg;
}

int MessageFree(MessageQ_Msg msg)
{
    int ret;
    ret = MessageQ_free(msg);
    if (ret == MessageQ_E_FAIL) {
       printf("MessageQ_alloc failed\n");
       return ret;
    }
    return ret;
}

int MessagePut(MessageQ_QueueId QueueId, MessageQ_Msg msg)
{
    int32_t status;
    status = MessageQ_put(QueueId, msg);
    if (status < 0) {
        printf("MessageQ_put failed\n");
        return status;
    }
    return status;
}

int MessageGet(MessageQ_Handle handle, MessageQ_Msg *msg)
{
    int32_t status;
    status = MessageQ_get(handle, msg, MessageQ_FOREVER);
    if (status < 0) {
      printf("MessageQ_get failed\n");
      return status;
    }
    return status;
}

