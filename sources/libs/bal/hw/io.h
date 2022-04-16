#pragma once

#include <bal/hw/mem.h>
#include <brutal-base>

typedef enum
{
    BAL_IO_MEMORY,
    BAL_IO_PORTS,
} BalIoType;

typedef struct
{
    BalIoType type;
    size_t base;
    size_t len;
} BalIo;

/**
 *  @brief Initialize an IO object from memory.
 *  @param mem The memory object to use.
 *  @return MMIO object.
 */
BalIo bal_io_mem(BalMem *mem);

/**
 * @brief Initialize an IO object from a port.
 * @param base Base of the port to use.
 * @param len Length of the port to use.
 * @return port IO object.
 */
BalIo bal_io_port(uintptr_t base, size_t len);

/**
 * @brief Initialize a 'slice' IO object from another IO object.
 * @param offset offset from the io object base to use.
 * @param size length of the io object to use.
 * @param io io object to use to create the slice object.
 * @return IO object.
 */
BalIo bal_io_slice(BalIo io, uintptr_t offset, size_t size);

/**
 * @brief read a 8bit value from an IO object.
 * @param io The IO object to read from \p io.
 * @param offset The offset to read from \p io.
 * @return The value read from \p io.
 */
uint8_t bal_io_in8(BalIo io, size_t offset);

/**
 * @brief read a 16bit value from an IO object.
 * @param io The IO object to read from \p io.
 * @param offset The offset to read from \p io.
 * @return The value read from \p io.
 */
uint16_t bal_io_in16(BalIo io, size_t offset);

/**
 * @brief read a 32bit value from an IO object.
 * @param io The IO object to read from.
 * @param offset The offset to read from.
 * @return The value read.
 */
uint32_t bal_io_in32(BalIo io, size_t offset);

/**
 * @brief read a 64bit value from an IO object.
 * @param io The IO object to read from \p io.
 * @param offset The offset to read from \p io.
 * @return The value read from \p io.
 */
uint64_t bal_io_in64(BalIo io, size_t offset);

/**
 * @brief write a 8bit value to an IO object.
 * @param io The IO object to write to \p io.
 * @param offset The offset to write to \p io.
 * @param data The value to write to \p io.
 */
void bal_io_out8(BalIo io, size_t offset, uint8_t data);

/**
 * @brief write a 16bit value to an IO object.
 * @param io The IO object to write to \p io.
 * @param offset The offset to write to \p io.
 * @param data The value to write to \p io.
 */
void bal_io_out16(BalIo io, size_t offset, uint16_t data);

/**
 * @brief write a 32bit value to an IO object.
 * @param io The IO object to write to \p io.
 * @param offset The offset to write to \p io.
 * @param data The value to write to \p io.
 */
void bal_io_out32(BalIo io, size_t offset, uint32_t data);

/**
 * @brief write a 64bit value to an IO object.
 * @param io The IO object to write to \p io.
 * @param offset The offset to write to \p io.
 * @param data The value to write to \p io.
 */
void bal_io_out64(BalIo io, size_t offset, uint64_t data);
