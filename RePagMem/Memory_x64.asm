;****************************************************************************
;  Memory_64.asm
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

INCLUDE listing.inc
INCLUDELIB OLDNAMES

.DATA?
dqi_EineSpeicherSeite QWORD ?
dqi_DoppelSpeicherSeite QWORD ?

CS_Prozessor SEGMENT EXECUTE
;----------------------------------------------------------------------------
stSystemInfo = 4
?CPUID@@YQXAEAU_SYSTEM_INFO@@@Z PROC
		mov ecx, dword ptr [rcx + 4]
		mov dword ptr [dqi_EineSpeicherSeite], ecx
		imul ecx, 02h
		mov dword ptr [dqi_DoppelSpeicherSeite], ecx

		ret
?CPUID@@YQXAEAU_SYSTEM_INFO@@@Z ENDP
;----------------------------------------------------------------------------
?Is_SSE2@@YQ_NXZ PROC
		mov rax, 01h
		cpuid
		xor rax, rax
		bt rdx, 26
		jne short Ende
		mov rax, 01h

	Ende:
		ret
?Is_SSE2@@YQ_NXZ ENDP
;----------------------------------------------------------------------------
?Is_RNGenerator@@YQDXZ PROC
		mov rax, 07h
		xor rcx, rcx
		cpuid
		bt rbx, 18
		je short RDSEED1

		bt rcx, 30
		je short RDRAND1
		xor rax, rax
		jmp short Ende

	RDSEED1:
		mov rax, 01h
		jmp short Ende

	RDRAND1:
		mov rax, 0ffh
	
	Ende:
		ret
?Is_RNGenerator@@YQDXZ ENDP
;----------------------------------------------------------------------------
?Is_AES@@YQ_NXZ PROC
		mov rax, 01h
		cpuid
		xor rax, rax
		bt rcx, 25
		mov rax, 01h

	Ende:
		ret
?Is_AES@@YQ_NXZ ENDP
;----------------------------------------------------------------------------
?Is_AVX@@YQ_NXZ PROC
		mov rax, 01h
		cpuid
		xor rax, rax
		bt rcx, 28
		jne short Ende
		mov rax, 01h

	Ende:
		ret
?Is_AVX@@YQ_NXZ ENDP
;----------------------------------------------------------------------------
?Is_AVX2@@YQ_NXZ PROC
		mov rax, 07h
		xor rcx, rcx
		cpuid
		xor rax, rax
		bt rbx, 5
		jne short Ende
		mov rax, 01h

	Ende:
		ret
?Is_AVX2@@YQ_NXZ ENDP
;----------------------------------------------------------------------------
CS_Prozessor ENDS
;----------------------------------------------------------------------------
CS_Memory SEGMENT EXECUTE
;----------------------------------------------------------------------------
?ZeroMem@@YQPEAXPEAXK@Z PROC
		test rcx, rcx
		jz short Ende
		test rdx, rdx
		jz short Ende    

		vpxor ymm0, ymm0, ymm0

	Grosse_32:
		cmp rdx, 32
		jb short Grosse_16
		vmovdqu ymmword ptr [rcx], ymm0
		sub rdx, 32
		add rcx, 32
		jmp short Grosse_32

	Grosse_16:
		cmp rdx, 16
		jb short Grosse_8
		vmovdqu xmmword ptr [rcx], xmm0
		sub rdx, 16
		add rcx, 16
		jmp short Grosse_16

	Grosse_8:
		cmp rdx, 8
		jb short Grosse_4
		mov qword ptr [rcx], 0
		sub rdx, 8
		add rcx, 8
		jmp short Grosse_8

	Grosse_4:
		cmp rdx, 4
		jb short Grosse_2
		mov dword ptr [rcx], 0
		sub rdx, 4
		add rcx, 4
		jmp short Grosse_4

	Grosse_2:
		cmp rdx, 2
		jb short Grosse_1
		mov word ptr [rcx], 0
		sub rdx, 2
		add rcx, 2
		jmp short Grosse_2

	Grosse_1:
		cmp rdx, 1
		jb short Ende
		mov byte ptr [rcx], 0

	Ende:
		ret
?ZeroMem@@YQPEAXPEAXK@Z ENDP
;----------------------------------------------------------------------------
_Text SEGMENT
VMSeitenkopf_vsNachsteSeite = 16
VMSeitenkopf_vsVorherigeSeite = 8
VMSeitenkopf_ulBelegt = 0

STSuchBlock_vsTabelle = 8
?VMBlock_ScrollEintrage_VonEnde@@YQXAEAUSTSuchBlock@@PEAD@Z PROC
		mov rax, rcx
		mov r8, qword ptr STSuchBlock_vsTabelle[rax]
		mov r9, rdx

	Anfang:
		cmp r8, r9
		je short Ende

		mov ecx, dword ptr VMSeitenkopf_ulBelegt[r8]
		cmp rcx, dqi_DoppelSpeicherSeite
		jl short Kurzer
		sub rcx, 16

	Kurzer:
		mov r11, r8
		add r11, rcx
		mov r10, r11
		sub r10, 16

		sub rcx, 32
		shr rcx, 4

	CopyEintrag:
		vmovdqu xmm0, xmmword ptr [r10]
		vmovdqu xmmword ptr [r11], xmm0
		mov r11, r10
		sub r10, 16
		loop short CopyEintrag

		mov rcx, qword ptr VMSeitenkopf_vsNachsteSeite[r8]
		test rcx, rcx
		jne short Vorherige
	
		add dword ptr VMSeitenkopf_ulBelegt[r8], 16

	Vorherige:
		mov r8, qword ptr VMSeitenkopf_vsVorherigeSeite[r8]
		mov qword ptr STSuchBlock_vsTabelle[rax], r8

		mov rcx, dqi_DoppelSpeicherSeite
		sub rcx, 8 
		add rcx, r8
		mov r10, qword ptr [rcx]

		mov rdx, qword ptr VMSeitenkopf_vsNachsteSeite[r8]  
		mov qword ptr [rdx + 40], r10

		sub rcx, 8
		mov r10, qword ptr [rcx]
		mov qword ptr [rdx + 32], r10
		jmp short Anfang

	Ende:
		ret
?VMBlock_ScrollEintrage_VonEnde@@YQXAEAUSTSuchBlock@@PEAD@Z ENDP
_Text ENDS
;----------------------------------------------------------------------------
_Text SEGMENT
VMSeitenkopf_vsNachsteSeite = 16
VMSeitenkopf_ulBelegt = 0

STSuchBlock_veEintrag = 32
STSuchBlock_vsTabelle = 8
STSuchBlock_ulBytes = 4
?VMBlock_ScrollEintrage_Einfugen@@YQXAEAUSTSuchBlock@@@Z PROC PUBLIC
		mov rax, rcx
		mov rdx, qword ptr STSuchBlock_vsTabelle[rax]

		mov ecx, dword ptr VMSeitenkopf_ulBelegt[rdx]
		cmp rcx, dqi_DoppelSpeicherSeite
		jne Kurzer
		sub rcx, 16

	Kurzer:
		mov r9, rdx
		add r9, rcx
		mov r8, r9
		sub r8, 16

		mov rcx, r8
		sub rcx, qword ptr STSuchBlock_veEintrag[rax]
		test rcx, rcx
		je KeinScroll
		shr rcx, 4

	CopyEintrag:
		vmovdqu xmm0, xmmword ptr [r8]
		vmovdqu xmmword ptr [r9], xmm0
		mov r9, r8
		sub r8, 16
		loop CopyEintrag

	KeinScroll:
		mov rcx, qword ptr [r8 + 8]
		sub ecx, dword ptr STSuchBlock_ulBytes[rax]
		mov qword ptr [r9 + 8], rcx
		mov qword ptr [r9], 0

		mov rax, qword ptr VMSeitenkopf_vsNachsteSeite[rdx]
		test rax, rax
		jne Ende
		add dword ptr VMSeitenkopf_ulBelegt[rdx], 16

	Ende:
		ret
?VMBlock_ScrollEintrage_Einfugen@@YQXAEAUSTSuchBlock@@@Z ENDP
_Text ENDS
;----------------------------------------------------------------------------
_Text SEGMENT
STSuchBlock_vsTabelle = 8
STSuchBlock_veEintrag = 32
?LetzterEintrag@@YQ_NAEAUSTSuchBlock@@@Z PROC
		xor rax, rax
		mov r8, qword ptr STSuchBlock_vsTabelle[rcx]
		mov edx, dword ptr [r8]
		sub rdx, 16
		add rdx, r8
		cmp rdx, qword ptr STSuchBlock_veEintrag[rcx]
		jne Ende
		mov al, 1

	Ende:
		ret
?LetzterEintrag@@YQ_NAEAUSTSuchBlock@@@Z ENDP
_Text ENDS
;----------------------------------------------------------------------------
_Text SEGMENT
VMSeitenkopf_vsNachsteSeite = 16
VMSeitenkopf_ulBelegt = 0
STSuchFrei_veScrollEintrag = 48
STSuchFrei_ulScrollSprung = 40
STSuchFrei_vsTabelle = 0
?VMFrei_ScrollEintrage_1@@YQXAEAUSTSuchFrei@@@Z PROC PUBLIC
		mov r8, qword ptr STSuchFrei_vsTabelle[rcx]
		mov edx, dword ptr STSuchFrei_ulScrollSprung[rcx]

		mov r10, qword ptr STSuchFrei_veScrollEintrag[rcx]
		mov r9, r10
		add r9, rdx

		mov ecx, dword ptr VMSeitenkopf_ulBelegt[r8]
		add rcx, r8
		sub rcx, r9

		cmp rdx, 16
		je short Scroll_16

	CopyEintrag_32:
		vmovdqu ymm0, ymmword ptr [r9]
		vmovdqu ymmword ptr [r10], ymm0
		add r10, 32
		add r9, 32
		sub rcx, 32
		cmp rcx, 16
		jg short CopyEintrag_32
		je short Scroll_16

		mov rax, qword ptr VMSeitenkopf_vsNachsteSeite[r8]
		test rax, rax
		jne short Ende
		jmp short Nullsetzen_16

	Scroll_16:
		shr rcx, 4
	CopyEintrag_16:
		vmovdqu xmm0, xmmword ptr [r9]
		vmovdqu xmmword ptr [r10], xmm0
		mov r10, r9
		add r9, 16
		loop short CopyEintrag_16

		mov rax, qword ptr VMSeitenkopf_vsNachsteSeite[r8]
		test rax, rax
		jne short Ende

		cmp rdx, 32
		jl short Nullsetzen_16
		je short Nullsetzen_32
		sub r10, 16

	Nullsetzen_32:
		sub r10, 16

	Nullsetzen_16:
		vpxor xmm0, xmm0, xmm0
		mov rcx, rdx
		shr rcx, 4
	CopyEintrag_0:
		vmovdqu xmmword ptr [r10], xmm0
		add r10, 16
		loop short CopyEintrag_0

	Ende:
		ret
?VMFrei_ScrollEintrage_1@@YQXAEAUSTSuchFrei@@@Z ENDP
_Text Ends
;----------------------------------------------------------------------------
_Text SEGMENT
VMSeitenkopf_vsNachsteSeite = 16
VMSeitenkopf_ulBelegt = 0
STSuchFrei_ulScrollSprung = 40
STSuchFrei_vsTabelle = 0
?VMFrei_ScrollEintrage_2@@YQXAEAUSTSuchFrei@@@Z PROC PUBLIC
		mov r8, qword ptr STSuchFrei_vsTabelle[rcx]
		mov edx, dword ptr STSuchFrei_ulScrollSprung[rcx]

		mov r10, r8
		add r10, 32
		mov r9, r10
		add r9, rdx

		mov ecx, dword ptr VMSeitenkopf_ulBelegt[r8]
		sub rcx, 32
		sub rcx, rdx

		cmp rdx, 16
		je short Scroll_16

	CopyEintrag_32:
		vmovdqu ymm0, ymmword ptr [r9]
		vmovdqu ymmword ptr [r10], ymm0
		add r10, 32
		add r9, 32
		sub rcx, 32
		cmp rcx, 16
		jg short CopyEintrag_32
		je short Scroll_16

		mov rax, qword ptr VMSeitenkopf_vsNachsteSeite[r8]
		test rax, rax
		jne short Ende
		jmp short Nullsetzen_16

	Scroll_16:
		shr rcx, 4
	CopyEintrag_16:
		vmovdqu xmm0, xmmword ptr [r9]
		vmovdqu xmmword ptr [r10], xmm0
		mov r10, r9
		add r9, 16
		loop short CopyEintrag_16

		mov rax, qword ptr VMSeitenkopf_vsNachsteSeite[r8]
		test rax, rax
		jne short Ende

		cmp rdx, 32
		jl short Nullsetzen_16
		je short Nullsetzen_32
		sub r10, 16

	Nullsetzen_32:
		sub r10, 16

	Nullsetzen_16:
		vpxor xmm0, xmm0, xmm0
		mov rcx, rdx
		shr rcx, 4
	CopyEintrag_0:
		vmovdqu xmmword ptr [r10], xmm0
		add r10, 16
		loop short CopyEintrag_0

	Ende:
		ret
?VMFrei_ScrollEintrage_2@@YQXAEAUSTSuchFrei@@@Z ENDP
_Text ENDS
;----------------------------------------------------------------------------
CS_Memory ENDS
;----------------------------------------------------------------------------
END
