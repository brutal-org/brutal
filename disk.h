#pragma once

#include <bal/ipc.h>

typedef enum
{
    DISK_SUCCESS = 0,
    DISK_UNEXPECTED_MESSAGE = 1,
    DISK_BAD_COMMUNICATION = 2,
} DiskError;

typedef struct
{
    uint8_t* buf;
    size_t len;
} DiskBuf;

typedef struct
{
    DiskBuf buf;
    uint64_t pos;
} DiskWriteRequest;

typedef struct
{
    uint64_t pos;
    uint64_t len;
} DiskReadRequest;

typedef DiskError DiskWrite(IpcEv* ev, BrAddr from, DiskWriteRequest const* req, Alloc* alloc);

DiskError disk_write(IpcEv* ev, BrAddr from, DiskWriteRequest const* req, Alloc* alloc);

typedef DiskError DiskRead(IpcEv* ev, BrAddr from, DiskReadRequest const* req, DiskBuf* resp, Alloc* alloc);

DiskError disk_read(IpcEv* ev, BrAddr from, DiskReadRequest const* req, DiskBuf* resp, Alloc* alloc);

typedef enum
{
    MSG_DISK_WRITE_REQ = 0,
    MSG_DISK_WRITE_RESP = 1,
    MSG_DISK_READ_REQ = 2,
    MSG_DISK_READ_RESP = 3,
} DiskMsgs;

typedef struct
{
    DiskWrite* write;
    DiskRead* read;
} DiskVTable;

void disk_provide(IpcEv* ev, DiskVTable* vtable);
