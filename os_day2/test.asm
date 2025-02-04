; A minimal bootloader
org 0x7C00         ; BIOS loads the bootloader at 0x7C00
cli                ; Disable interrupts
hlt                ; Halt the CPU

times 510-($-$$) db 0  ; Fill remaining bytes with zeros
dw 0xAA55              ; Boot signature
