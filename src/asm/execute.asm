section .text
global execute
; im going crazy
execute:
    mov eax, [esp + 4]     ; Get the function address
    call dword [eax]       ; Call the function
    add esp, 8             ; Adjust the stack pointer
    ret
