
#include "disk.h"

void disk_error_pack(BalPack *self, DiskError *data)
{
    bal_pack_enum(self, (int *)data);
}

void disk_error_unpack(BalUnpack *self, DiskError *data)
{
    bal_unpack_enum(self, (int *)data);
}

void disk_buf_pack(BalPack *self, DiskBuf *data)
{
    bal_pack_slice(self, data, bal_pack_u8);
}

void disk_buf_unpack(BalUnpack *self, DiskBuf *data)
{
    bal_unpack_slice(self, data, bal_unpack_u8);
}

void disk_write_request_pack(BalPack *self, DiskWriteRequest *data)
{
    disk_buf_pack(self, &data->buf);
    bal_pack_u64(self, &data->pos);
}

void disk_write_request_unpack(BalUnpack *self, DiskWriteRequest *data)
{
    disk_buf_unpack(self, &data->buf);
    bal_unpack_u64(self, &data->pos);
}

void disk_read_request_pack(BalPack *self, DiskReadRequest *data)
{
    bal_pack_u64(self, &data->pos);
    bal_pack_u64(self, &data->len);
}

void disk_read_request_unpack(BalUnpack *self, DiskReadRequest *data)
{
    bal_unpack_u64(self, &data->pos);
    bal_unpack_u64(self, &data->len);
}

DiskError disk_write(IpcEv *ev, BrAddr from, DiskWriteRequest const *req, Alloc *alloc)
{
    return idl_hook_call(ev, from, (IdlBinding){
                                       -429303110,
                                       MSG_DISK_WRITE_REQ,
                                       (BalPackFn *)disk_write_request_pack,
                                       (BalUnpackFn *)disk_write_request_unpack,
                                       -1,
                                       nullptr,
                                       nullptr,
                                   },
                         req, nullptr, alloc);
}

DiskError disk_read(IpcEv *ev, BrAddr from, DiskReadRequest const *req, DiskBuf *resp, Alloc *alloc)
{
    return idl_hook_call(ev, from, (IdlBinding){
                                       -429303110,
                                       MSG_DISK_READ_REQ,
                                       (BalPackFn *)disk_read_request_pack,
                                       (BalUnpackFn *)disk_read_request_unpack,
                                       MSG_DISK_READ_RESP,
                                       (BalPackFn *)disk_buf_pack,
                                       (BalUnpackFn *)disk_buf_unpack,
                                   },
                         req, resp, alloc);
}

void disk_dispatch(IpcEv *ev, BrMsg *req, void *ctx)
{
    DiskVTable *vtable = (DiskVTable *)ctx;
    switch (req->type)
    {
    case MSG_DISK_WRITE_REQ:
    {
        DiskWriteRequest req_buf;
        idl_hook_handle((IdlHandler){
                            IDL_HANDLER_REQ_NIL,
                            (IdlHandlerFn *)vtable->write,
                        },
                        ev, req, (IdlBinding){
                                     -429303110,
                                     MSG_DISK_WRITE_REQ,
                                     (BalPackFn *)disk_write_request_pack,
                                     (BalUnpackFn *)disk_write_request_unpack,
                                     -1,
                                     nullptr,
                                     nullptr,
                                 },
                        &req_buf, nullptr);
        break;
    }
    case MSG_DISK_READ_REQ:
    {
        DiskReadRequest req_buf;
        DiskBuf resp_buf;
        idl_hook_handle((IdlHandler){
                            IDL_HANDLER_REQ_RESP,
                            (IdlHandlerFn *)vtable->read,
                        },
                        ev, req, (IdlBinding){
                                     -429303110,
                                     MSG_DISK_READ_REQ,
                                     (BalPackFn *)disk_read_request_pack,
                                     (BalUnpackFn *)disk_read_request_unpack,
                                     MSG_DISK_READ_RESP,
                                     (BalPackFn *)disk_buf_pack,
                                     (BalUnpackFn *)disk_buf_unpack,
                                 },
                        &req_buf, &resp_buf);
        break;
    }
    };
}

void disk_provide(IpcEv *ev, DiskVTable *vtable)
{
    br_ev_provide(ev, -429303110, disk_dispatch, vtable);
}
