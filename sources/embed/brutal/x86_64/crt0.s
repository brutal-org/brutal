section .text

extern _entry

global _start:function (_start.end - _start)
_start:
    and rsp, 0xFFFFFFFFFFFFFFF0
    call _entry
    ud2
.end:
