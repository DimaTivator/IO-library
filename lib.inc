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
; DONE. PRETESTED
print_uint:
	mov		rax, rdi
	xor		rcx, rcx				; sign-counter
	mov		r10, 10					; devisor -> r10

	dec		rsp
	mov		byte [rsp], 0			; 0 -> stack for string to be 0-terminated

	.loop:
		xor		rdx, rdx
		div		r10					; rax / r10 -> result in rax, remainder in rdx
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
; DONE. PRETESTED
print_int:
	test	rdi, rdi
	jns		.positive

	push	rdi
	mov		rdi, '-'
	call	print_char
	pop		rdi
	neg		rdi

	.positive:
		call	print_uint
		ret

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
; DONE. TESTED
string_equals:
	; stack:
	;	len(str1) -- [rsp]
	;	str1_pointer -- [rsp + 8]
	;	str2_pointer -- [rsp + 16]

    sub		rsp, 24						; Allocating 32 bytes on stack for local variables
	mov		[rsp + 8], rdi				; Storing first string pointer
	mov		[rsp + 16], rsi				; Storing second string pointer

	call	string_length				; Str1_pointer is already in rdi
	mov		[rsp], rax

	mov		rdi, [rsp + 16]
	call	string_length

	cmp		rax, [rsp]					; Comparing lengths
										; rax - second string length, [rsp] -- first string length

	jne		.fail						; return 0 if lengths are not equal

	xor		rax, rax
	.loop:
		cmp		rax, [rsp]
		je		.success
										; getting current characters from each string
		mov		rdx, [rsp + 8]			; rdx -- str1_pointer
		mov		rdi, [rsp + 16]			; rdi -- str2_pointer

		mov		cl, byte [rdx + rax]	; cl -- the smallest part of rcx, current str1 character
		mov		sil, byte [rdi + rax]	; sil -- the smallest part of rsi, current str2 character

		cmp		cl, sil
		jne		.fail

		inc		rax
		jmp		.loop

	.success:
		mov		rax, 1
		add		rsp, 24					; Deallocating the space on the stack
		ret

	.fail:
		mov		rax, 0
		add		rsp, 24
		ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
; DONE. PRETESTED
read_char:
	dec		rsp				; Allocating space on stack for 1 byte

	mov		rax, SYS_READ
	mov		rdi, STDIN
	mov		rsi, rsp
	mov		rdx, 1			; the number of bytes to read

	syscall

	test	rax, rax
	jz		.exit

	xor	rax, rax
	mov	al, [rsp]			; al -- the smallest part of rax

	.exit:
		inc		rsp			; Deallocating space on stack
		ret

; Принимает: адрес начала буфера, размер буфера
; Читает в буфер слово из stdin, пропуская пробельные символы в начале, .
; Пробельные символы это пробел 0x20, табуляция 0x9 и перевод строки 0xA.
; Останавливается и возвращает 0 если слово слишком большое для буфера
; При успехе возвращает адрес буфера в rax, длину слова в rdx.
; При неудаче возвращает 0 в rax
; Эта функция должна дописывать к слову нуль-терминатор

read_word:
	xor		r8, r8				; string length
	dec		rsi					; buffer size -= 1 because we need to add \0 to the end of the string

	.skip_spaces:
		push	rdi
		push	rsi
		push	r8
		call	read_char
		pop		r8
		pop		rsi
		pop		rdi

		cmp		al, 0x20		; char == ' ' => skip
		je		.skip_spaces

		cmp		al, 0x9			; char == tab => skip
		je		.skip_spaces

		cmp		al, 0xA			; char == `\n` => skip
		je		.skip_spaces

	.loop:
		test	al, al
		jz		.success

		cmp		r8, rsi
		je		.fail

		cmp		al, 0x20		; char == ' ' => skip
		je		.success

		cmp		al, 0x9			; char == tab => skip
		je		.success

		cmp		al, 0xA			; char == `\n` => skip
		je		.success

		mov		[rdi + r8], al

		inc		r8				; string_length++

		push	rdi
		push	rsi
		push	r8
		call	read_char
		pop		r8
		pop		rsi
		pop		rdi

		jmp		.loop

	.success:
		mov		rax, rdi
		mov		byte [rdi + r8], 0
		mov		rdx, r8
		ret

	.fail:
		xor		rax, rax
		ret

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
parse_uint:
	xor		rax, rax				; number
	xor		rdx, rdx				; length
	xor		rcx, rcx
	mov		r10, 10
	.loop:
		mov		cl, [rdi + rdx]

		cmp		cl, 0
		je		.exit

		cmp		cl, '0'
		jl		.exit

		cmp		cl, '9'
		jg		.exit

		sub		cl, '0'				; convert digit from ascii code to numeric

		push	rdx
		mul		r10					; rax *= 10
		pop		rdx
		add		rax, rcx			; rax += current digit

		inc		rdx
		jmp		.loop

	.exit:
		ret


; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был)
; rdx = 0 если число прочитать не удалось
parse_int:
	mov		r8b, [rdi]				; first symbol (maybe sign) -> r8b

	cmp		r8b, '0'
	jl		.check_sign

	cmp		r8b, '9'
	jg		.fail

	jmp		.parse

	.check_sign:
		cmp		r8b, '+'
		je		.parse

		cmp		r8b, '-'
		je		.parse_negative

	.fail:
		xor		rdx, rdx
		ret

	.parse_negative:
		inc		rdi

	.parse:
		push	r8
		call	parse_uint
		pop		r8

		test	rdx, rdx
		jz		.fail

		cmp		r8b, '-'
		jne		.exit

		neg		rax
		inc		rdx
		ret


	.exit:
		cmp		r8b, '+'
		jne		.exit_no_sign
		inc		rdx

	.exit_no_sign:
		ret


; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0
string_copy:
	; rdi - string pointer
	; rsi - buffer pointer
	; rdx - buffer size
	xor		rax, rax				; character counter
	.loop:
		cmp		rax, rdx			; counter == buffer size => buffer size is less than string size
		je		.fail

		mov		cl, [rdi + rax]		; current character -> cl

		mov		[rsi + rax], cl		; current character -> buffer
		inc		rax

		cmp		cl, 0
		je		.success

		jmp		.loop

	.fail:
		mov		rax, 0
		ret

	.success:
		ret
