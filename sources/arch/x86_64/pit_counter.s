global pit_read_counter

PIT_MODE_COMMAND equ 0x43
PIT_DATA_PORT0 equ 0x40

pit_read_counter:
    mov al, 0
    out PIT_MODE_COMMAND, al 

    in al, PIT_DATA_PORT0
    mov ah, al 

    in al, PIT_DATA_PORT0 
    rol ax, 8
    ret