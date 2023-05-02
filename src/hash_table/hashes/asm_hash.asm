default		rel

global		asm_hash_murmur:function

section		.text

;====================================================================================================
; Calculate hash function using SIMD
;====================================================================================================
; Entry:	RDI	- (64-aligned) address of string
; Exit:		RAX	- hash value
; Destroys:	
;====================================================================================================
asm_hash_murmur:
		mov			rax,			0x8B72E9FB7FAA60FD
		vpbroadcastq		zmm0,			rax				; Seed in zmm0
		mov			rax,			0xC6A4A7935BD1E995
		vpbroadcastq		zmm1,			rax				; Multiplicand in zmm1
		vpxorq			zmm2,			zmm2,			zmm2	; Zero in zmm2
		; tmp = mult * 64
		vpshldq			zmm3,			zmm1,			zmm2,		6

		; hash = seed ^ (mult * 64)
		vpxorq			zmm0,			zmm0,			zmm3

		; chars (zmm3) = [rdi] * mult
		vpmullq			zmm3,			zmm1, ZWORD 		[rdi]
		; chars ^= chars >> 47
		vpshrdq			zmm4,			zmm3,			zmm2,		47
		vpxorq			zmm3,			zmm3,			zmm4
		; chars *= mult
		vpmullq			zmm3,			zmm3,			zmm1

		; Load permutation mask into zmm4
		align 64
		vmovdqa64		zmm4, ZWORD 		[PermuteMask]
		
		; Incorporate first 7*8 = 56 characters into hash
		%rep 7
		; hash ^= char
		vpxorq			zmm0,			zmm0,			zmm3
		; hash *= mult
		vpmullq			zmm0,			zmm0,			zmm1
		; next char
		vpermq			zmm3,			zmm3,			zmm4
		%endrep

		; Incorporate remaining 8 characters
		; hash ^= char
		vpxorq			zmm0,			zmm0,			zmm3
		; hash *= mult
		vpmullq			zmm0,			zmm0,			zmm1

		; Last mix of hash
		; hash ^= hash >> 47
		vpshrdq			zmm3,			zmm0,			zmm2,		47
		vpxorq			zmm0,			zmm0,			zmm3
		; hash *= mult
		vpmullq			zmm0,			zmm0,			zmm1
		; hash ^= hash >> 47
		vpshrdq			zmm3,			zmm0,			zmm2,		47
		vpxorq			zmm0,			zmm0,			zmm3

		vzeroupper
		vmovq			rax,			xmm0
		ret
;====================================================================================================
		

section		.rodata

align 64,	db 0
PermuteMask	dq 1
		dq 2
		dq 3
		dq 4
		dq 5
		dq 6
		dq 7
		dq 0
