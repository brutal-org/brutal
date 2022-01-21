#include <brutal/codec/gzip/gzip.h>
#include <brutal/font/ssfn2/ssfn.h>
#include <stdc/ansi/string.h>

/* magic bytes */
#define SSFN_MAGIC "SFN2"
#define SSFN_COLLECTION "SFNC"
#define SSFN_ENDMAGIC "2NFS"

typedef enum
{
    FS_REGULAR = 0,
    FS_BOLD = 1,
    FS_ITALIC = 2,
} FontStyle;

static MaybeError ssfn2_load_internal(IoReader reader)
{
    SSFN2CommonHeader common_header;
    TRY(MaybeError, io_read(reader, (uint8_t *)&common_header, sizeof(SSFN2CommonHeader)));

    if (memcmp(common_header.magic, SSFN_COLLECTION, 4) == 0)
    {
        // uint32_t end = load_le(common_header.size);
        while (ssfn2_load_internal(reader).succ)
            ;
    }
    else if (memcmp(common_header.magic, SSFN_MAGIC, 4) == 0)
    {
        SSFN2FontHeader font_header;
        TRY(MaybeError, io_read(reader, (uint8_t *)&font_header, sizeof(SSFN2FontHeader)));

        FontStyle style = load_le(font_header.type);
        UNUSED(style);
    }
    else
    {
        return ERROR(ERR_UNDEFINED);
    }

    return SUCCESS;
}

MaybeError ssfn2_load(IoRSeek rseek)
{
    IoReader reader;
    if (gzip_probe(rseek))
    {
        // TODO:
        // reader = gzip_reader(rseek.reader);
        return ERROR(ERR_NOT_IMPLEMENTED);
    }
    else
    {
        reader = rseek.reader;
    }
    ssfn2_load_internal(reader);

    return SUCCESS;
}
