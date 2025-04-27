global bee_strlen

; __bee_strlen__
; input => rdi (const string*)
; output => rax (usize)
bee_strlen:
    mov rsi, rdi
    xor rax, rax
.bee_strlen_loop:
    cmp byte [rsi], 0
    je .bee_strlen_done
    inc rax
    inc rsi
    jmp .bee_strlen_loop
.bee_strlen_done:
    ret
