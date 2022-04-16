#pragma once

#include <brutal-text>

#define FOREACH_KB_KEY(KEY)                 \
    KEY(KBKEY_INVALID, 0x0)                 \
    KEY(KBKEY_ESC, 0x01)                    \
    KEY(KBKEY_NUM1, 0x02)                   \
    KEY(KBKEY_NUM2, 0x03)                   \
    KEY(KBKEY_NUM3, 0x04)                   \
    KEY(KBKEY_NUM4, 0x05)                   \
    KEY(KBKEY_NUM5, 0x06)                   \
    KEY(KBKEY_NUM6, 0x07)                   \
    KEY(KBKEY_NUM7, 0x08)                   \
    KEY(KBKEY_NUM8, 0x09)                   \
    KEY(KBKEY_NUM9, 0x0A)                   \
    KEY(KBKEY_NUM0, 0x0B)                   \
    KEY(KBKEY_SYM1, 0x0C)                   \
    KEY(KBKEY_SYM2, 0x0D)                   \
    KEY(KBKEY_BKSPC, 0x0E)                  \
    KEY(KBKEY_TAB, 0x0F)                    \
    KEY(KBKEY_Q, 0x10)                      \
    KEY(KBKEY_W, 0x11)                      \
    KEY(KBKEY_E, 0x12)                      \
    KEY(KBKEY_R, 0x13)                      \
    KEY(KBKEY_T, 0x14)                      \
    KEY(KBKEY_Y, 0x15)                      \
    KEY(KBKEY_U, 0x16)                      \
    KEY(KBKEY_I, 0x17)                      \
    KEY(KBKEY_O, 0x18)                      \
    KEY(KBKEY_P, 0x19)                      \
    KEY(KBKEY_SYM3, 0x1A)                   \
    KEY(KBKEY_SYM4, 0x1B)                   \
    KEY(KBKEY_ENTER, 0x1C)                  \
    KEY(KBKEY_LCTRL, 0x1D)                  \
    KEY(KBKEY_A, 0x1E)                      \
    KEY(KBKEY_S, 0x1F)                      \
    KEY(KBKEY_D, 0x20)                      \
    KEY(KBKEY_F, 0x21)                      \
    KEY(KBKEY_G, 0x22)                      \
    KEY(KBKEY_H, 0x23)                      \
    KEY(KBKEY_J, 0x24)                      \
    KEY(KBKEY_K, 0x25)                      \
    KEY(KBKEY_L, 0x26)                      \
    KEY(KBKEY_SYM5, 0x27)                   \
    KEY(KBKEY_SYM6, 0x28)                   \
    KEY(KBKEY_SYM7, 0x29)                   \
    KEY(KBKEY_LSHIFT, 0x2A)                 \
    KEY(KBKEY_SYM8, 0x2B)                   \
    KEY(KBKEY_Z, 0x2C)                      \
    KEY(KBKEY_X, 0x2D)                      \
    KEY(KBKEY_C, 0x2E)                      \
    KEY(KBKEY_V, 0x2F)                      \
    KEY(KBKEY_B, 0x30)                      \
    KEY(KBKEY_N, 0x31)                      \
    KEY(KBKEY_M, 0x32)                      \
    KEY(KBKEY_SYM9, 0x33)                   \
    KEY(KBKEY_SYM10, 0x34)                  \
    KEY(KBKEY_SYM11, 0x35)                  \
    KEY(KBKEY_RSHIFT, 0x36)                 \
    KEY(KBKEY_SYM12, 0x37)                  \
    KEY(KBKEY_LALT, 0x38)                   \
    KEY(KBKEY_SPACE, 0x39)                  \
    KEY(KBKEY_CAPSLOCK, 0x3A)               \
    KEY(KBKEY_F1, 0x3B)                     \
    KEY(KBKEY_F2, 0x3C)                     \
    KEY(KBKEY_F3, 0x3D)                     \
    KEY(KBKEY_F4, 0x3E)                     \
    KEY(KBKEY_F5, 0x3F)                     \
    KEY(KBKEY_F6, 0x40)                     \
    KEY(KBKEY_F7, 0x41)                     \
    KEY(KBKEY_F8, 0x42)                     \
    KEY(KBKEY_F9, 0x43)                     \
    KEY(KBKEY_F10, 0x44)                    \
    KEY(KBKEY_NUMLOCK, 0x45)                \
    KEY(KBKEY_SCROLLLOCK, 0x46)             \
    KEY(KBKEY_KPAD7, 0x47)                  \
    KEY(KBKEY_KPAD8, 0x48)                  \
    KEY(KBKEY_KPAD9, 0x49)                  \
    KEY(KBKEY_SYM13, 0x4A)                  \
    KEY(KBKEY_KPAD4, 0x4B)                  \
    KEY(KBKEY_KPAD5, 0x4C)                  \
    KEY(KBKEY_KPAD6, 0x4D)                  \
    KEY(KBKEY_SYM14, 0x4E)                  \
    KEY(KBKEY_KPAD1, 0x4F)                  \
    KEY(KBKEY_KPAD2, 0x50)                  \
    KEY(KBKEY_KPAD3, 0x51)                  \
    KEY(KBKEY_KPAD0, 0x52)                  \
    KEY(KBKEY_SYM15, 0x53)                  \
    KEY(KBKEY_ALTSYSRQ, 0x54)               \
    KEY(KBKEY_NO_STANDARD_MEANING_1, 0x55)  \
    KEY(KBKEY_NO_STANDARD_MEANING_2, 0x56)  \
    KEY(KBKEY_F11, 0x57)                    \
    KEY(KBKEY_F12, 0x58)                    \
    KEY(KBKEY_KPADENTER, (0x80 + 0x1C))     \
    KEY(KBKEY_RCTRL, (0x80 + 0x1D))         \
    KEY(KBKEY_FAKELSHIFT, (0x80 + 0x2A))    \
    KEY(KBKEY_SYM16, (0x80 + 0x35))         \
    KEY(KBKEY_FAKERSHIFT, (0x80 + 0x36))    \
    KEY(KBKEY_CTRLPRINTSCRN, (0x80 + 0x37)) \
    KEY(KBKEY_RALT, (0x80 + 0x38))          \
    KEY(KBKEY_CTRLBREAK, (0x80 + 0x46))     \
    KEY(KBKEY_HOME, (0x80 + 0x47))          \
    KEY(KBKEY_UP, (0x80 + 0x48))            \
    KEY(KBKEY_PGUP, (0x80 + 0x49))          \
    KEY(KBKEY_LEFT, (0x80 + 0x4B))          \
    KEY(KBKEY_RIGHT, (0x80 + 0x4D))         \
    KEY(KBKEY_END, (0x80 + 0x4F))           \
    KEY(KBKEY_DOWN, (0x80 + 0x50))          \
    KEY(KBKEY_PGDOWN, (0x80 + 0x51))        \
    KEY(KBKEY_INSERT, (0x80 + 0x52))        \
    KEY(KBKEY_DELETE, (0x80 + 0x53))        \
    KEY(KBKEY_LSUPER, (0x80 + 0x5B))        \
    KEY(KBKEY_RSUPER, (0x80 + 0x5C))        \
    KEY(KBKEY_MENU, (0x80 + 0x5D))

typedef enum
{
#define ITER(KEY, CODE) KEY = CODE,
    FOREACH_KB_KEY(ITER)
#undef ITER
} KbKey;

static inline Str kbkey_to_str(KbKey key)
{
#define ITER(KEY, CODE) \
    if (KEY == key)     \
        return str$(#KEY);
    FOREACH_KB_KEY(ITER)
#undef ITER

    return str$("INVALID");
}

typedef enum
{
    KBMOTION_UP,
    KBMOTION_DOWN,
    KBMOTION_TYPED,
} KbMotion;

#define KBMOD_ALT (1 << 0)
#define KBMOD_ALTGR (1 << 1)
#define KBMOD_SHIFT (1 << 2)
#define KBMOD_CTRL (1 << 3)
#define KBMOD_SUPER (1 << 4)

typedef uint8_t KbMod;
