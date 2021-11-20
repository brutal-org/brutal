#include "kernel/parcel.h"

BrResult parcel_pack(Parcel *self, BrMsg const *msg, Domain *domain)
{
    self->msg = *msg;

    if (domain == nullptr)
    {
        return BR_SUCCESS;
    }

    for (size_t i = 0; i < BR_MSG_ARG_COUNT; i++)
    {
        if (msg->flags & BR_MSG_HND(i))
        {
            self->objs[i] = domain_lookup(domain, msg->args[i], BR_OBJECT_ANY);

            if (!self->objs[i])
            {
                parcel_deinit(self);

                return BR_BAD_HANDLE;
            }
        }
    }

    return BR_SUCCESS;
}

BrResult parcel_unpack(Parcel *self, BrMsg *msg, Domain *domain)
{
    *msg = self->msg;

    if (domain == nullptr)
    {
        parcel_deinit(self);

        return BR_SUCCESS;
    }

    for (size_t i = 0; i < BR_MSG_ARG_COUNT; i++)
    {
        if (self->objs[i])
        {
            msg->args[i] = domain_add(domain, self->objs[i]);
        }
    }

    parcel_deinit(self);

    return BR_SUCCESS;
}

void parcel_deinit(Parcel *self)
{
    for (size_t i = 0; i < BR_MSG_ARG_COUNT; i++)
    {
        if (self->objs[i])
        {
            object_deref(self->objs[i]);
        }
    }
}
