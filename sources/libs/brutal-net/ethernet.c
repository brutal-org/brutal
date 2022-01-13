#include <brutal-net>
#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-mem>

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
