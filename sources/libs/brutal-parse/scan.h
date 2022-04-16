#pragma once

#include <brutal-io>
#include <brutal-text>
#include <ctype.h>

typedef int(ScanMatch)(int);

typedef struct
{
    int position;
    Str message;
    StrFix128 token;
} ScanError;

typedef struct
{
    uint8_t const *buf;
    int size;
    int head;

    int token;

    bool has_error;
    ScanError error;
} Scan;

/*!
    @brief Abbort the program the scanned encouter an error

    @param SELF (Scanner*)
*/
#define scan_assert_no_error(SELF)                              \
    ({                                                          \
        if ((SELF)->has_error)                                  \
        {                                                       \
            scan_dump_error(SELF, io_chan_out());               \
            panic$("scan: terminating due to previous errors"); \
        }                                                       \
    })

/*!
    @brief Initialize a scanner using a string

    @param self
    @param str The string to be scanned
*/
void scan_init(Scan *self, Str str);

/*!
    @brief Tell if the scanner reached the end of the input

    @param self
    @return true if the end is reached or an error has been thrown
    @return false if there is still data to scan
*/
bool scan_ended(Scan *self);

/*!
    @brief Tell the current position of the scanner head

    @param self
    @return int An offset relative to start of the input
*/
int scan_tell(Scan *self);

/*!
    @brief Move the scanner's head

    @param self
    @param offset An offset relative to start of the input to move the head to
*/
void scan_seek(Scan *self, int offset);

/*!
    @brief Tell the size of the input

    @param self
    @return int The size of the input
*/
int scan_size(Scan *self);

/*!
    @brief Peek ahead in the input

    @param self
    @param offset An offset relative the scanner's reading head
    @return char The byte at this offset
*/
char scan_peek(Scan *self, int offset);

bool scan_peek_is_any(Scan *self, int offset, Str chars);

char scan_curr(Scan *self);

char scan_curr_is_any(Scan *self, Str chars);

char scan_next(Scan *self);

void scan_next_n(Scan *self, int n);

bool scan_skip(Scan *self, char c);

bool scan_skip_word(Scan *self, Str word);

bool scan_skip_any(Scan *self, Str chars);

bool scan_skip_match(Scan *self, ScanMatch *match);

bool scan_skip_space(Scan *self);

bool scan_eat(Scan *self, ScanMatch *match);

bool scan_eat_any(Scan *self, Str chars);

Str scan_eat_match(Scan *self, ScanMatch *match);

void scan_begin(Scan *self);

Str scan_end(Scan *self);

/* --- Error Handling ------------------------------------------------------ */

void scan_throw(Scan *self, Str message, Str token);

void scan_breakpoint(Scan *self);

bool scan_expect(Scan *self, char c);

bool scan_expect_word(Scan *self, Str word);

bool scan_expect_any(Scan *self, Str chars);

bool scan_dump_error(Scan *self, IoWriter writer);
