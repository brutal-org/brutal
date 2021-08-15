#define I_M_A_BAD_PROGRAMMER_OR_I_M_WRITING_A_C_LIBRARY
#include <handover/handover.h>
#include <stddef.h>
#include <stdint.h>

uint8_t inb(uint16_t port)
{
    uint8_t data;
    __asm__ volatile("inb %1, %0"
                     : "=a"(data)
                     : "d"(port));
    return data;
}

void outb(uint16_t port, uint8_t data)
{
    __asm__ volatile("outb %0, %1"
                     :
                     : "a"(data), "Nd"(port));
}

typedef enum
{
    COM1 = 0x3F8,
    COM2 = 0x2F8,
    COM3 = 0x3E8,
    COM4 = 0x2E8
} SerialPort;

static int is_transmit_empty(SerialPort port)
{
    return inb(port + 5) & 0x20;
}

void *memcpy(void *dst, const void *src, size_t size)
{
    const char *from = src;
    char *to = dst;

    for (size_t i = 0; i < size; ++i)
        *to++ = *from++;
    return dst;
}

static int serial_received(SerialPort port)
{
    return inb(port + 5) & 1;
}

void com_initialize(SerialPort port)
{
    outb(port + 1, 0x00);
    outb(port + 3, 0x80);
    outb(port + 0, 0x03);
    outb(port + 1, 0x00);
    outb(port + 3, 0x03);
    outb(port + 2, 0xC7);
    outb(port + 4, 0x0B);
}

void com_putc(SerialPort port, char c)
{
    while (is_transmit_empty(port) == 0)
        ;
    outb(port, c);
}

void com_write_string(SerialPort port, char const *str)
{
    while (*str)
    {
        com_putc(port, *str++);
    }
}

char com_getc(SerialPort port)
{
    while (serial_received(port) == 0)
        ;
    return inb(port);
}

size_t strlen(char const *str)
{
    size_t i;
    for (i = 0; str[i] != '\0'; i++)
        ;
    return i;
}

char *strncat(char *dest, char const *src, size_t n)
{
    size_t dest_length = strlen(dest);
    size_t i;

    for (i = 0; i < n && src[i] != '\0'; i++)
    {
        dest[dest_length + i] = src[i];
    }

    dest[dest_length + i] = '\0';

    return dest;
}

char *strcat(char *dest, char *src)
{
    return strncat(dest, src, strlen(src));
}

// This function isn't perfect but it works for now :)
char *string_convert(unsigned int num, int base)
{
    static char Representation[] = "0123456789ABCDEF";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do
    {
        *--ptr = Representation[num % base];
        num /= base;
    } while (num != 0);
    return (ptr);
}

void *memset(void *bufptr, int value, size_t size)
{
    unsigned char *buf = (unsigned char *)bufptr;
    size_t i;
    for (i = 0; i < size; i++)
        buf[i] = (unsigned char)value;

    return bufptr;
}

void vsprintf(char *str, char *format, va_list arg)
{
    unsigned int i;
    unsigned int ZERO = 0;
    char *s;

    int position = 0;

    while (*format)
    {

        if (*format == '%')
        {
            format++;
            switch (*format)
            {
            case 'c':
                i = va_arg(arg, int);
                str[position] = i;
                position++;
                break;

            case 'd':
                i = va_arg(arg, int);
                if (i < ZERO)
                {
                    i = -i;
                    str[position] = '-';
                }
                strcat(str, string_convert(i, 10));
                position += strlen(string_convert(i, 10));

                break;

            case 'o':
                i = va_arg(arg, unsigned int);
                strcat(str, string_convert(i, 8));
                position += strlen(string_convert(i, 8));
                break;

            case 's':
                s = va_arg(arg, char *);
                strcat(str, s);
                position += strlen(s);
                break;

            case 'x':
                i = va_arg(arg, unsigned int);
                strcat(str, string_convert(i, 16));
                position += strlen(string_convert(i, 16));
                break;

            default:
                str[position] = '%';
                position++;
                break;
            }
        }

        else
        {
            str[position] = *format;
            position++;
        }

        format++;
    }
}

void printf(char *format, ...)
{

    va_list arg;
    va_start(arg, format);

    char message[4096] = {0};

    vsprintf(message, format, arg);

    com_write_string(COM1, message);

    va_end(arg);
}

void kmain(Handover *handover)
{

    printf("%x\n", handover->rsdp);

    auto fb = handover->framebuffer;
    auto mmap = handover->mmap;

    uint64_t size = 0;

    uint64_t free_pages = 0;
    
    for (int i = 0; i < mmap.size; i++)
    {
        auto entry = mmap.entries[i];

	if(entry.type == HANDOVER_MMAP_FREE)
	  free_pages += entry.length;
        size += entry.length;
    }

    printf("%d mb of memory\n %d", size / 1024 / 1024, free_pages / 1024 / 1024);

    size_t index = 10 + (fb.pitch / sizeof(uint32_t)) * 10;

    ((uint32_t *)fb.addr)[index] = 0xffffff;

    while (1)
        ;
}
