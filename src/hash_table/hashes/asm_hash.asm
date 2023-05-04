default		rel

global		asm_hash_murmur:function

section		.text

SEED		equ	0x8B72E9FB7FAA60FD
MULT		equ	0xC6A4A7935BD1E995

;====================================================================================================
; Calculate hash function using SIMD
;====================================================================================================
; Entry:	RDI	- (8-aligned) address of string
; Exit:		RAX	- hash value
; Destroys:	
;====================================================================================================
asm_hash_murmur:
		mov			r8,			MULT			; Multiplicand in r8
		mov			rax,			SEED ^ (MULT << 6)	; Hash in rax

		%assign 		offset			0
		%rep			8

		mov			rdx, QWORD		[rdi + 8*offset]	; Loaded value in rdx
		imul			rdx,			r8			; value *= MULT
		mov			rsi,			rcx
		shr			rsi,			47
		xor			rdx,			rsi			; value ^= value << 47
		imul			rdx,			r8			; value *= MULT

		xor			rax,			rdx
		imul			rax,			r8			; Incorporate character

		%assign			offset			offset + 1
		%endrep

		ret
;====================================================================================================

