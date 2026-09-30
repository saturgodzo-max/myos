
bits 32                         ; 32-битный режим

section .multiboot
    align 4
    dd 0x1BADB002               ; Магическое число Multiboot
    dd 0x00000003               ; Флаги (выравнивание модулей + карта памяти)
    dd -(0x1BADB002 + 0x00000003) ; Контрольная сумма

section .text
global _start
extern kernel_main

_start:
    cli                         ; Отключаем прерывания
    mov esp, stack_top          ; Инициализируем указатель стека (выровненный)

    call kernel_main            ; Переходим в Си код

.halt:
    hlt                         ; На случай выхода из Си
    jmp .halt

section .bss
align 16                        ; Жесткое выравнивание стека для GCC
stack_bottom:
resb 16384                      ; Выделяем 16 КБ под стек
stack_top:
