#include <embed/crypto.h>
#include <fcntl.h>
#include <unistd.h>

size_t embed_crypto_entropy(void *buf, size_t len)
{
    int fd = open("/dev/random", O_RDONLY);
    if (fd < 0)
        return 0;

    ssize_t r = read(fd, buf, len);
    close(fd);
    if (r < 0)
        return 0;

    return r;
}
