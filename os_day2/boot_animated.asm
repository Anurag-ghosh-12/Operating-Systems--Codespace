; Bootloader - Moving animation in VGA text mode
BITS 16           ; Real mode
ORG 0x7C00        ; BIOS loads bootloader at 0x7C00

start:
    ; Clear the screen
    mov ax, 0x0600
    mov bh, 0x07        ; Black background, white text
    mov cx, 0x0000      ; Start of screen
    mov dx, 0x184F      ; End of screen (80x25 mode)
    int 0x10            ; BIOS interrupt for video services

    ; Set up variables
    mov si, 0xB800      ; Video memory address
    mov di, si
    mov bl, '*'         ; Character to display
    mov bh, 0x1E        ; Attribute: Blue background, yellow text

loop:
    ; Write character to video memory
    mov [si], bl        ; Character
    mov [si + 1], bh    ; Attribute

    ; Delay for animation
    call delay

    ; Clear the previous position
    mov [si], ' '
    mov [si + 1], 0x07  ; Default white text on black background

    ; Update position
    add si, 2           ; Move to the next character position
    cmp si, 0xB8FA      ; Check if we've reached the end of the line
    jl loop             ; If not, continue

    ; Reset position
    mov si, 0xB800
    jmp loop            ; Loop indefinitely

delay:
    mov cx, 0xFFFF      ; Simple delay loop
del_loop:
    dec cx
    jnz del_loop
    ret

times 510-($-$$) db 0  ; Fill remaining bytes with zeros
dw 0xAA55              ; Boot signature

