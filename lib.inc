section .text
 
 
; Принимает код возврата и завершает текущий процесс
; DONE. PRETESTED
exit: 
    mov		rax, 60
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
	
	push	rdi
	call	string_length
	pop		rdi

	mov		rax, 1 ; type of syscall
	mov		rsi, rdi ; string pointer -> rsi
	mov		rdi, 1 ; stdout descriptor

	syscall

    ret

; Принимает код символа и выводит его в stdout
print_char: 

    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
	mov		rdi, 0xA
	call	print_char
    ret

; Выводит беззнаковое 8-байтовое число в десятичном формате 
; Совет: выделите место в стеке и храните там результаты деления
; Не забудьте перевести цифры в их ASCII коды.
print_uint:
    xor rax, rax
    ret

; Выводит знаковое 8-байтовое число в десятичном формате 
print_int:
    xor rax, rax
    ret

; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
string_equals:
    xor rax, rax
    ret

; Читает один символ из stdin и возвращает его. Возвращает 0 если достигнут конец потока
read_char:
	section: .bss
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