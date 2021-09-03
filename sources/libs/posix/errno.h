#pragma once

/* Argument list too long. */
#define E2BIG 1

/* Permission denied. */
#define EACCES 2

/* Address in use. */
#define EADDRINUSE 3

/* Address not available. */
#define EADDRNOTAVAIL 4

/* Address family not supported. */
#define EAFNOSUPPORT 5

/* Resource unavailable, try again (may be the same value as [EWOULDBLOCK]). */
#define EAGAIN 6

/* Operation would block (may be the same value as [EAGAIN]). */
#define EWOULDBLOCK 6

/* Connection already in progress. */
#define EALREADY 7

/* Bad file descriptor. */
#define EBADF 8

/* Bad message. */
#define EBADMSG 9

/* Device or resource busy. */
#define EBUSY 10

/* Operation canceled. */
#define ECANCELED 11

/* No child processes. */
#define ECHILD 12

/* Connection aborted. */
#define ECONNABORTED 13

/* Connection refused. */
#define ECONNREFUSED 14

/* Connection reset. */
#define ECONNRESET 15

/* Resource deadlock would occur. */
#define EDEADLK 16

/* Destination address required. */
#define EDESTADDRREQ 17

/* Mathematics argument out of domain of function. */
/* EDOM 18 Defined in ansi */

/* Reserved. */
#define EDQUOT 19

/* File exists. */
#define EEXIST 20

/* Bad address. */
#define EFAULT 21

/* File too large. */
#define EFBIG 22

/* Host is unreachable. */
#define EHOSTUNREACH 23

/* Identifier removed. */
#define EIDRM 24

/* Illegal byte sequence. */
/* EILSEQ 25 defined in ansi */

/* Operation in progress. */
#define EINPROGRESS 26

/* Interrupted function. */
#define EINTR 27

/* Invalid argument. */
#define EINVAL 28

/* I/O error. */
#define EIO 29

/* Socket is connected. */
#define EISCONN 30

/* Is a directory. */
#define EISDIR 31

/* Too many levels of symbolic links. */
#define ELOOP 32

/* File descriptor value too large. */
#define EMFILE 33

/* Too many links. */
#define EMLINK 34

/* Message too large. */
#define EMSGSIZE 35

/* Reserved. */
#define EMULTIHOP 36

/* Filename too long. */
#define ENAMETOOLONG 37

/* Network is down. */
#define ENETDOWN 38

/* Connection aborted by network. */
#define ENETRESET 39

/* Network unreachable. */
#define ENETUNREACH 40

/* Too many files open in system. */
#define ENFILE 41

/* No buffer space available. */
#define ENOBUFS 42

/* No message is available on the STREAM head read queue. */
#define ENODATA 43

/* No such device. */
#define ENODEV 44

/* No such file or directory. */
#define ENOENT 45

/* Executable file format error. */
#define ENOEXEC 46

/* No locks available. */
#define ENOLCK 47

/* Reserved. */
#define ENOLINK 48

/* Not enough space. */
#define ENOMEM 49

/* No message of the desired type. */
#define ENOMSG 50

/* Protocol not available. */
#define ENOPROTOOPT 51

/* No space left on device. */
#define ENOSPC 52

/* No STREAM resources. */
#define ENOSR 53

/* Not a STREAM. */
#define ENOSTR 54

/* Function not supported. */
#define ENOSYS 55

/* The socket is not connected. */
#define ENOTCONN 56

/* Not a directory. */
#define ENOTDIR 57

/* Directory not empty. */
#define ENOTEMPTY 58

/* State not recoverable. */
#define ENOTRECOVERABLE 59

/* Not a socket. */
#define ENOTSOCK 60

/* Not supported (may be the same value as [EOPNOTSUPP]). */
#define ENOTSUP 61

/* Operation not supported on socket (may be the same value as [ENOTSUP]). */
#define EOPNOTSUPP 61

/* Inappropriate I/O control operation. */
#define ENOTTY 62

/* No such device or address. */
#define ENXIO 63

/* Value too large to be stored in data type. */
#define EOVERFLOW 64

/* Previous owner died. */
#define EOWNERDEAD 65

/* Operation not permitted. */
#define EPERM 66

/* Broken pipe. */
#define EPIPE 67

/* Protocol error. */
#define EPROTO 68

/* Protocol not supported. */
#define EPROTONOSUPPORT 69

/* Protocol wrong type for socket. */
#define EPROTOTYPE 70

/* Result too large. */
/* ERANGE 71 defined in ansi */

/* Read-only file system. */
#define EROFS 72

/* Invalid seek. */
#define ESPIPE 73

/* No such process. */
#define ESRCH 74

/* Reserved. */
#define ESTALE 75

/* Stream ioctl() timeout. */
#define ETIME 76

/* Connection timed out. */
#define ETIMEDOUT 77

/* Text file busy. */
#define ETXTBSY 78

/* Cross-device link. */
#define EXDEV 79
