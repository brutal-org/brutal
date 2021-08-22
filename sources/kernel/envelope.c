#include "kernel/envelope.h"

BrResult envelope_send(Envelope *self, BrMsg const *msg, Domain *domain)
{
    self->msg = *msg;

    for (size_t i = 0; i < BR_MSG_ARG_COUNT; i++)
    {
        if (msg->flags & BR_MSG_HND(i))
        {
            self->objs[i] = domain_lookup(domain, msg->arg[i], BR_OBJECT_ANY);

            if (!self->objs[i])
            {
                envelope_deinit(self);

                return BR_BAD_HANDLE;
            }
        }
    }

    return BR_SUCCESS;
}

BrResult envelope_send_without_object(Envelope *self, BrMsg const *msg)
{
    self->msg = *msg;

    for (size_t i = 0; i < BR_MSG_ARG_COUNT; i++)
    {
        if (msg->flags & BR_MSG_HND(i))
        {
            return BR_BAD_HANDLE; // check if the message ask to pass an object
        }
    }

    return BR_SUCCESS;
}

BrResult envelope_recv(Envelope *self, BrMsg *msg, Domain *domain)
{
    *msg = self->msg;

    for (size_t i = 0; i < BR_MSG_ARG_COUNT; i++)
    {
        if (self->objs[i])
        {
            domain_add(domain, self->objs[i]);
        }
    }

    envelope_deinit(self);

    return BR_SUCCESS;
}

void envelope_deinit(Envelope *self)
{
    for (size_t i = 0; i < BR_MSG_ARG_COUNT; i++)
    {
        if (self->objs[i])
        {
            object_deref(self->objs[i]);
        }
    }
}
