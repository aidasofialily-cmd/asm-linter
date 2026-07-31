section .text
global _start

_start:
    sub rsp, 8
    mov eax, 1
    xor ebx, ebx
    int 0x80
