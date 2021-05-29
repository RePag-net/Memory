;****************************************************************************
;  Memory_86.asm
;  For more information see https://github.com/RePag-net/Memory
;****************************************************************************
;
;****************************************************************************
; The MIT License(MIT)
;
;  Copyright(c) 2021 René Pagel
;
;  Permission is hereby granted, free of charge, to any person obtaining a copy
;  of this softwareand associated documentation files(the "Software"), to deal
;  in the Software without restriction, including without limitation the rights
;  to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
;  copies of the Software, and to permit persons to whom the Software is
;  furnished to do so, subject to the following conditions :
;
;  The above copyright noticeand this permission notice shall be included in all
;  copies or substantial portions of the Software.
;
;  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
;  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;  SOFTWARE.
;******************************************************************************

.686P
.XMM
INCLUDE listing.inc
.MODEL FLAT
INCLUDELIB LIBCMTD
INCLUDELIB OLDNAMES

PUBLIC dwEAX_CPUID_EAX_1
PUBLIC dwEBX_CPUID_EAX_1
PUBLIC dwECX_CPUID_EAX_1
PUBLIC dwEDX_CPUID_EAX_1

PUBLIC _SSE3
PUBLIC _AES
PUBLIC _AVX
PUBLIC _RDRAND
PUBLIC _MMX
PUBLIC _SSE
PUBLIC _SSE2

.DATA?
dwEineSpeicherSeite DWORD ?
dwDoppelSpeicherSeite DWORD ?
dbIntel DB ?

dwEAX_CPUID_EAX_1 DWORD ?
dwEBX_CPUID_EAX_1 DWORD ?
dwECX_CPUID_EAX_1 DWORD ?
dwEDX_CPUID_EAX_1 DWORD ?

dwEBX_CPUID_EAX_7 DWORD ?
dwECX_CPUID_EAX_7 DWORD ?

.DATA
;---ECX----
_SSE3 equ 0
_AES equ 25
_AVX equ 28
_RDRAND equ 30
;---EDX----
_MMX equ 23
_SSE equ 25
_SSE2 equ 26
;---EBX---
_AVX2 equ 5
_RDSEED equ 18
_SHA equ 29

CS_Prozessor SEGMENT EXECUTE
;----------------------------------------------------------------------------
stSystemInfo = 4
?CPUID@@YQXAAU_SYSTEM_INFO@@@Z PROC
    sub esp, 4
		push ebx

    mov dword ptr stSystemInfo[esp], ecx
		mov dword ptr [dbIntel], 00h
		xor eax, eax
		cpuid
		cmp ebx, 756e6547h
		jne short Speicher
		cmp ecx, 6c65746eh
		jne short Speicher
		cmp edx, 49656e69h
		jne short Speicher
		mov dword ptr [dbIntel], 01h

	Speicher:
    mov ecx, dword ptr stSystemInfo[esp]
    mov ecx, dword ptr [ecx+4]
		mov dword ptr [dwEineSpeicherSeite], ecx
		imul ecx, 02h
		mov dword ptr [dwDoppelSpeicherSeite], ecx

		mov eax, 01h
		cpuid
		mov dword ptr [dwEAX_CPUID_EAX_1], eax
		mov dword ptr [dwEBX_CPUID_EAX_1], ebx
		mov dword ptr [dwECX_CPUID_EAX_1], ecx
		mov dword ptr [dwEDX_CPUID_EAX_1], edx

    mov eax, 07h
    xor ecx, ecx
		cpuid
		mov dword ptr [dwEBX_CPUID_EAX_7], ebx
		mov dword ptr [dwECX_CPUID_EAX_7], ecx

		pop	ebx
    add esp, 4
		ret 0
?CPUID@@YQXAAU_SYSTEM_INFO@@@Z ENDP
;----------------------------------------------------------------------------
?Is_SSE2@@YQ_NXZ PROC
		xor eax, eax
		bt dwEDX_CPUID_EAX_1, _SSE2
		jne short Ende
		mov al, 01h

	Ende:
		ret 0
?Is_SSE2@@YQ_NXZ ENDP
;----------------------------------------------------------------------------
?Is_RNGenerator@@YQDXZ PROC
		bt dwEBX_CPUID_EAX_7, _RDSEED
		je short RDSEED1

		bt dwECX_CPUID_EAX_1, _RDRAND
		je short RDRAND1
		xor eax, eax
		jmp short Ende

	RDSEED1:
		mov eax, 01h
		jmp short Ende

	RDRAND1:
		mov eax, 0ffh
	
	Ende:
		ret 0
?Is_RNGenerator@@YQDXZ ENDP
;----------------------------------------------------------------------------
?Is_AES@@YQ_NXZ PROC
		xor eax, eax
		bt dwECX_CPUID_EAX_1, _AES
		jne short Ende
		mov eax, 01h

	Ende:
		ret 0
?Is_AES@@YQ_NXZ ENDP
;----------------------------------------------------------------------------
?Is_AVX@@YQ_NXZ PROC
		xor eax, eax
		bt dwECX_CPUID_EAX_1, _AVX
		jne short Ende
		mov eax, 01h

	Ende:
		ret 0
?Is_AVX@@YQ_NXZ ENDP
;----------------------------------------------------------------------------
?Is_AVX2@@YQ_NXZ PROC
		xor eax, eax
		bt dwEBX_CPUID_EAX_7, _AVX2
		jne short Ende
		mov eax, 01h

	Ende:
		ret 0
?Is_AVX2@@YQ_NXZ ENDP
;----------------------------------------------------------------------------
CS_Prozessor ENDS
;----------------------------------------------------------------------------
CS_Memory SEGMENT PARA PRIVATE FLAT EXECUTE
;----------------------------------------------------------------------------
?ZeroMem@@YQPAXPAXK@Z PROC PUBLIC
		test ecx, ecx
		jz short Ende
		test edx, edx
		jz short Ende    

		pxor mm0, mm0
		xorps xmm0, xmm0

		bt dwECX_CPUID_EAX_1, _AVX
		jnc short Grosse_16
		vxorps ymm0, ymm0, ymm0

	Grosse_32:
		cmp edx, 32
		jb short Grosse_16
		vmovups ymmword ptr [ecx], ymm0
		sub edx, 32
		add ecx, 32
		jmp short Grosse_32

	Grosse_16:
		cmp edx, 16
		jb short Grosse_8
		movups xmmword ptr [ecx], xmm0
		sub edx, 16
		add ecx, 16
		jmp short Grosse_16

	Grosse_8:
		cmp edx, 8
		jb short Grosse_4
		movq qword ptr [ecx], mm0
		sub edx, 8
		add ecx, 8
		jmp short Grosse_8

	Grosse_4:
		cmp edx, 4
		jb short Grosse_2
		mov dword ptr [ecx], 0
		sub edx, 4
		add ecx, 4
		jmp short Grosse_4

	Grosse_2:
		cmp edx, 2
		jb short Grosse_1
		mov word ptr [ecx], 0
		sub edx, 2
		add ecx, 2
		jmp short Grosse_2

	Grosse_1:
		cmp edx, 1
		jb short Grosse_0
		mov byte ptr [ecx], 0

	Grosse_0:
		emms

	Ende:
		ret	0
?ZeroMem@@YQPAXPAXK@Z ENDP
;----------------------------------------------------------------------------
_Text SEGMENT
esp_Bytes = 4
s_vsSeiteAlt = 12

vsNachsteSeite = 12
vsVorherigeSeite = 8
ulBelegt = 0

stSuchBlock_veEintrag = 20
stSuchBlock_vsTabelle = 8
?VMBlock_ScrollEintrage_VonEnde@@YQXAAUSTSuchBlock@@PAD@Z PROC PUBLIC
    sub esp, esp_Bytes
		push ebx
		push esi
		push edi

		mov eax, ecx
    mov ebx, dword ptr stSuchBlock_vsTabelle[eax]
    mov dword ptr s_vsSeiteAlt[esp], edx

	Anfang:
    cmp ebx, dword ptr s_vsSeiteAlt[esp]
		je short Ende

    mov ecx, dword ptr ulBelegt[ebx]
		cmp ecx, dword ptr [dwDoppelSpeicherSeite]
    jl short Kurzer
    sub ecx, 08h

  Kurzer:
	  mov edi, ebx
    add edi, ecx
		mov esi, edi
    sub esi, 08h

    sub ecx, 10h
    shr ecx, 03h

	CopyEintrag:
		movq xmm0, qword ptr [esi]
		movq qword ptr [edi], xmm0
		mov edi, esi
		sub esi, 08h
		loop short CopyEintrag

		cmp dword ptr vsNachsteSeite[ebx], 0h
		jne short Vorherige
  
	  add dword ptr ulBelegt[ebx], 08h

	Vorherige:
	  mov ebx, dword ptr vsVorherigeSeite[ebx]
		mov dword ptr stSuchBlock_vsTabelle[eax], ebx

		mov ecx, dword ptr [dwDoppelSpeicherSeite]
		sub ecx, 04h 
		add ecx, ebx
		mov esi, dword ptr [ecx]

		mov edx, dword ptr vsNachsteSeite[ebx]  
		mov dword ptr [edx + 14h], esi

		sub ecx, 04h
		mov esi, dword ptr [ecx]
		mov dword ptr [edx + 10h], esi
		jmp short Anfang

	Ende:
		pop	edi
		pop	esi
		pop	ebx
    add esp, esp_Bytes
		ret 0
?VMBlock_ScrollEintrage_VonEnde@@YQXAAUSTSuchBlock@@PAD@Z ENDP
_Text ENDS
;----------------------------------------------------------------------------
_Text SEGMENT
vsNachsteSeite = 12
ulBelegt = 0

stSuchBlock_veEintrag = 20
stSuchBlock_vsTabelle = 8
stSuchBlock_ulBytes = 4
?VMBlock_ScrollEintrage_Einfugen@@YQXAAUSTSuchBlock@@@Z PROC PUBLIC
		push esi
		push edi

    mov eax, ecx
		mov edx, dword ptr stSuchBlock_vsTabelle[eax]

    mov ecx, dword ptr ulBelegt[edx]
    cmp ecx, dword ptr [dwDoppelSpeicherSeite]
    jne short Kurzer
    sub ecx, 08h

  Kurzer:
	  mov edi, edx
    add edi, ecx
		mov esi, edi
    sub esi, 08h

		mov ecx, esi
    sub ecx, dword ptr stSuchBlock_veEintrag[eax]
		test ecx, ecx
		je short KeinScroll
    shr ecx, 03h

	CopyEintrag:
		movq xmm0, qword ptr [esi]
		movq qword ptr [edi], xmm0
		mov edi, esi
		sub esi, 08h
		loop short CopyEintrag

	KeinScroll:
		mov ecx, dword ptr [esi + 04h]
		sub ecx, dword ptr stSuchBlock_ulBytes[eax]
		mov dword ptr [edi + 04h], ecx
		mov dword ptr [edi], 0h

		cmp dword ptr vsNachsteSeite[edx], 0h
		jne short Ende
	  add dword ptr ulBelegt[edx], 08h

	Ende:
		pop	edi
		pop	esi
		ret 0
?VMBlock_ScrollEintrage_Einfugen@@YQXAAUSTSuchBlock@@@Z ENDP
_Text ENDS
;----------------------------------------------------------------------------
_Text SEGMENT
stSuchBlock_veEintrag = 20
stSuchBlock_vsTabelle = 8
?LetzterEintrag@@YQ_NAAUSTSuchBlock@@@Z PROC PUBLIC
		push ebx

		xor eax, eax
		mov ebx, dword ptr stSuchBlock_vsTabelle[ecx]
		mov edx, dword ptr [ebx]
		sub edx, 08h
		add edx, ebx
		cmp edx, dword ptr stSuchBlock_veEintrag[ecx]
		jne short Ende
		mov al, 01h

	Ende:
		pop	ebx
		ret 0
?LetzterEintrag@@YQ_NAAUSTSuchBlock@@@Z ENDP
_Text ENDS
;----------------------------------------------------------------------------
_Text SEGMENT
vsNachsteSeite = 12
ulBelegt = 0

stSuchFrei_veScrollEintrag = 24
stSuchFrei_ulScrollSprung = 20
stSuchFrei_vsTabelle = 0
?VMFrei_ScrollEintrage_1@@YQXAAUSTSuchFrei@@@Z PROC PUBLIC
		push ebx
		push esi
		push edi

		mov ebx, dword ptr stSuchFrei_vsTabelle[ecx]
		mov edx, dword ptr stSuchFrei_ulScrollSprung[ecx]

    mov edi, dword ptr stSuchFrei_veScrollEintrag[ecx]
    mov esi, edi
    add esi, edx

    mov ecx, ebx
    add ecx, dword ptr ulBelegt[ebx]
    sub ecx, esi

		cmp edx, 8
		je short Scroll_8

		cmp ecx, 10h
		jb short Scroll_8
	CopyEintrag_16:
		movdqu xmm0, xmmword ptr [esi]
		movdqu xmmword ptr [edi], xmm0
		add edi, 16
		add esi, 16
		sub ecx, 16
		cmp ecx, 8
		jg short CopyEintrag_16
		je short Scroll_8

		mov eax, dword ptr vsNachsteSeite[ebx]
		test eax, eax
		jne short Ende
		jmp short Nullsetzen_8

	Scroll_8:
		shr ecx, 3
	CopyEintrag_8:
		movq xmm0, qword ptr [esi]
		movq qword ptr [edi], xmm0
		mov edi, esi
		add esi, 8
		loop short CopyEintrag_8

		mov eax, dword ptr vsNachsteSeite[ebx]
		test eax, eax
		jne short Ende

		cmp edx, 16
		jl short Nullsetzen_8
		je short Nullsetzen_16
		sub edi, 8

	Nullsetzen_16:
		sub edi, 8

	Nullsetzen_8:
	  pxor xmm0, xmm0
    mov ecx, edx
    shr ecx, 3
  CopyEintrag_0:
    movq qword ptr [edi], xmm0
		add edi, 8
		loop short CopyEintrag_0

	Ende:
		pop	edi
		pop	esi
		pop	ebx
		ret 0
?VMFrei_ScrollEintrage_1@@YQXAAUSTSuchFrei@@@Z ENDP
_Text ENDS
;----------------------------------------------------------------------------
_Text SEGMENT
vsNachsteSeite = 12
ulBelegt = 0

stSuchFrei_ulScrollSprung = 20
stSuchFrei_vsTabelle = 0
?VMFrei_ScrollEintrage_2@@YQXAAUSTSuchFrei@@@Z PROC PUBLIC
		push ebx
		push esi
		push edi

    mov ebx, dword ptr stSuchFrei_vsTabelle[ecx]
		mov edx, dword ptr stSuchFrei_ulScrollSprung[ecx]

		mov edi, ebx
    add edi, 10h
    mov esi, edi
    add esi, edx

    mov ecx, dword ptr ulBelegt[ebx]
    sub ecx, 10h
    sub ecx, edx

		cmp edx, 8
		je short Scroll_8

		cmp ecx, 10h
		jb short Scroll_8
	CopyEintrag_16:
		movdqu xmm0, xmmword ptr [esi]
		movdqu xmmword ptr [edi], xmm0
		add edi, 16
		add esi, 16
		sub ecx, 16
		cmp ecx, 8
		jg short CopyEintrag_16
		je short Scroll_8

		mov eax, dword ptr vsNachsteSeite[ebx]
		test eax, eax
		jne short Ende
		jmp short Nullsetzen_8

	Scroll_8:
		shr ecx, 3
	CopyEintrag_8:
		movq xmm0, qword ptr [esi]
		movq qword ptr [edi], xmm0
		mov edi, esi
		add esi, 8
		loop short CopyEintrag_8

		mov eax, dword ptr vsNachsteSeite[ebx]
		test eax, eax
		jne short Ende

		cmp edx, 16
		jl short Nullsetzen_8
		je short Nullsetzen_16
		sub edi, 8

	Nullsetzen_16:
		sub edi, 8

	Nullsetzen_8:
	  pxor xmm0, xmm0
    mov ecx, edx
    shr ecx, 3
  CopyEintrag_0:
    movq qword ptr [edi], xmm0
		add edi, 8
		loop CopyEintrag_0

	Ende:
		pop	edi
		pop	esi
		pop	ebx
		ret 0
?VMFrei_ScrollEintrage_2@@YQXAAUSTSuchFrei@@@Z ENDP
_Text ENDS
CS_Memory ENDS
;----------------------------------------------------------------------------
END

