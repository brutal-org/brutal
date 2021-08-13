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

void com_write_string(SerialPort port, char *str)
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

int main(void)
{
    com_write_string(COM1, "Hello world!");
    return 42;
}
