#include <brutal/net.h>
#include <brutal/alloc.h>
#include "brutal/debug/assert.h"
#include "brutal/mem/funcs.h"
#include "brutal/net/ethernet.h"


EthernetFrame ethernet_make_frame(EthernetFrameHeader hdr, uint8_t *payload, size_t payload_size)
{
    assert_lower_equal(payload_size, ETHERNET_PAYLOAD_MAX_SIZE); /* fuck you */

    EthernetFrame frame;
    frame.size = sizeof(EthernetFrameHeader) + payload_size;
    frame.frame = alloc_malloc(alloc_global(), frame.size);

    mem_cpy(frame.frame, &hdr, sizeof(EthernetFrame));
    mem_cpy(frame.frame + sizeof(EthernetFrameHeader), payload, payload_size);
    return frame;
}
