[bits 16]               ; We are working in real mode, 16-bit mode
[org 0x7c00]            ; Boot sector starts at 0x7c00 (standard boot sector address)

start:
    ; Simple bootloader that does nothing but halt the system
    mov ah, 0x0e        ; 
    mov al, 'H'         ; Character to print
    int 0x10            ; Call BIOS interrupt 0x10 to print a character
    mov al, 'i'
    int 0x10
    mov al, ' '
    int 0x10
    mov al, 'B'
    int 0x10
    mov al, 'o'
    int 0x10
    mov al, 'o'
    int 0x10
    mov al, 't'
    int 0x10
    mov al, '!'
    int 0x10

    ; Infinite loop to halt the system
halt:
    jmp halt            ; Jump to itself, halting the system

; Set the magic number (0x55AA) at the end of the boot sector
times 510 - ($ - $$) db 0 ; Fill the gap to make 512 bytes total (from the start to the magic number)
dw 0xAA55             ; Magic number (0x55AA)

