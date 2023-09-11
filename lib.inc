SYS_READ   equ     0          ; read text from stdin
SYS_WRITE  equ     1          ; write text to stdout
SYS_EXIT   equ     60         ; terminate the program
STDIN      equ     0          ; standard input
STDOUT     equ     1		  ; standard output

section .data
numbers: db '0123456789'

section .text
 
; Принимает код возврата и завершает текущий процесс
; DONE. PRETESTED
exit:
    mov     rax, SYS_EXIT
    syscall
    ret


; Принимает указатель на нуль-терминированную строку, возвращает её длину
; DONE. PRETESTED
string_length:
    xor		rax, rax

	.loop:
		cmp		byte [rdi + rax], 0 ; check if the current symbol is \0
		je		.exit
		inc		rax
		jmp		.loop

	.exit:
		ret

; Принимает указатель на нуль-терминированную строку, выводит её в stdout
; DONE. PRETESTED	
print_string:

    push    rdi
    call    string_length
    pop     rdi

	mov		rdx, rax		; string_length function returns length in rax
							; so we put rax value to rdx
    mov     rax, SYS_WRITE
    mov     rsi, rdi		; string pointer -> rsi
    mov     rdi, STDOUT

    syscall

    ret


; Принимает код символа и выводит его в stdout
; DONE. PRETESTED
print_char:
	push	rdi
	mov		rax, SYS_WRITE
	mov		rsi, rsp
	mov		rdx, 1			; len
	mov		rdi, STDOUT

	syscall

	pop rdi

    ret


; Переводит строку (выводит символ с кодом 0xA)
; DONE. PRETESTED
print_newline:
	mov		rdi, 0xA
	call	print_char
    ret

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
	mov		rax, rdi
	xor		rcx, rcx				; sign-counter
	mov		rbx, 10					; devisor -> rbx

	dec		rsp
	mov		byte [rsp], 0			; 0 -> stack for string to be 0-terminated

	.loop:
		xor		rdx, rdx
		div		rbx					; rax / rbx -> result in rax, remainder in rdx
		add		rdx, '0'			; convert digit in rdx to its ASCII code
		dec		rsp
		mov		[rsp], dl			; dl -- the smallest part of rdx
		inc		rcx
		test	rax, rax
		jne		.loop

	mov		rdi, rsp				; decimal number pointer -> rdi
	push	rcx						; save rcx before function calling
	call	print_string
	pop		rcx

	add		rsp, rcx				; deallocating memory for deecimal number on stack
	inc		rsp

	ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    xor rax, rax
    ret

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
; ? DONE. PRETESTED ? TODO allocate 24 bytes instead of 32
string_equals:
	; stack:
	;	len(str1) -- [rsp]
	;	len(str2) -- [rsp + 8]
	;	str1_pointer -- [rsp + 16]
	;	str2_pointer -- [rsp + 24]

    sub		rsp, 32  ; Allocate 32 bytes on stack for local variables
	mov		[rsp + 16], rdi ; Store first string pointer
	mov		[rsp + 24], rsi ; Store second string pointer

	; Calculating length of the first string (its pointer is in rdi)
	call	string_length
	mov		[rsp], rax

	; Calculating length of the second string
	mov		rdi, [rsp + 24]
	call	string_length

	; Comparing lengths
	; rax - second string length, [rsp] -- first string length
	cmp		rax, [rsp]
	jne		.fail ; return 0 if lengths are not equal

	xor		rax, rax
	.loop:
		; getting current characters from each string
		mov		rdx, [rsp + 16] ; rdx -- str1_pointer
		mov		rbx, [rsp + 24] ; rbx -- str2_pointer

		mov		cl, byte [rdx + rax] ; cl -- the smallest part of rcx, current str1 character
		mov		sil, byte [rbx + rax] ; sil -- the smallest part of rsi, current str2 character

		cmp		cl, sil
		jne		.fail

		inc		rax
		cmp		rax, [rsp]
		je		.success
		jmp		.loop

	.success:
		mov		rax, 1
		add		rsp, 32 ; Deallocate the space on the stack
		ret

	.fail:
		mov		rax, 0
		add		rsp, 32 ; Deallocate the space on the stack
		ret


; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
	section .bss
		.buffer: resb 1 ; buffer to store the input characted 

    mov		rax, 0 ; syscall type
	mov		rdi, 0 ; stdin
	mov		rsi, .buffer
	mov		rdx, 1 ; number of bytes to read

	syscall

	mov		al, buffer ; al -- the smallest part of rax 

    ret 

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор

read_word:
    ret
 

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
    xor rax, rax
    ret




; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
parse_int:
    xor rax, rax
    ret 

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
    xor rax, rax
    ret
