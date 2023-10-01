/*
 * coreipc.h
 *
 *  Created on: 2023Äê9ÔÂ26ÈÕ
 *      Author: hanyi
 */

#ifndef COREMESSAGE_COREIPC_H_
#define COREMESSAGE_COREIPC_H_
#include <ti/ipc/MessageQ.h>

#define NUM_CORE     8

#define MESSAGE_SIZE_IN_BYTES  52

typedef struct {
    MessageQ_MsgHeader header;      /* 32 bytes */
    int src;
    int flags;
    int numMsgs;
    int seqNum;
    int heartbeat;
}CoreMsg;

int InitIpc(void);
int AttachCore(int coreNum);
int MessageCreate(void);
int RegisterMem(int heapid);
int MessageOpen(int corenum, MessageQ_QueueId *QueueId);
CoreMsg * MessageAlloc(int headid);
int MessageFree(MessageQ_Msg msg);
int MessageGet(MessageQ_Handle handle, MessageQ_Msg *msg);
int MessagePut(MessageQ_QueueId QueueId, MessageQ_Msg msg);
#endif /* COREMESSAGE_COREIPC_H_ */
