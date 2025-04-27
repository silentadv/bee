global bee_print
global bee_exit

; __bee_print__
; input => rdi (const string*)
; output => void
bee_print:
    extern bee_strlen
    call bee_strlen
    mov rdx, rax
    mov rsi, rdi

    mov rax, 1
    mov rdi, 1
    syscall
    ret

; __bee_exit__
; input => rdi (int)
; output => void
bee_exit:
   mov rax, 60
   syscall
   ret
