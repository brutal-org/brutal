section .text

extern _entry

global _start:function (_start.end - _start)
_start:
    call _entry
    ud2
.end:
