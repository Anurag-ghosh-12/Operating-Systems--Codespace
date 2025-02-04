[BITS 16]
[ORG 0x7C00]         

start:
    mov si, message  

print_loop:
    lodsb            ; Load the next character
    cmp al, 0        
    je stop          
    mov ah, 0x0E     
    int 0x10         ; Call BIOS interrupt to print the character in AL
    jmp print_loop   

stop:
    hlt              

message db "Hello, Anurag", 0  

times 510 - ($ - $$) db 0  ; Fill the rest bytes boot sector with zeros
dw 0xAA55                  ; (boot signature)

