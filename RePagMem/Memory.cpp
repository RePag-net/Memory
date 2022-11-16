/****************************************************************************
	Memory.cpp
	For more information see https://github.com/RePag-net/Memory
****************************************************************************/

/****************************************************************************
	The MIT License(MIT)

	Copyright(c) 2022 René Pagel

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this softwareand associated documentation files(the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions :

	The above copyright noticeand this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
******************************************************************************/

#include "pch.h"
#ifdef ENTWICKLUNG
#define INLINE
#else
#define INLINE __forceinline
#endif
//---------------------------------------------------------------------------

void __vectorcall VMBlock_ScrollEintrage_VonEnde(STSuchBlock& stSuchBlock, VMSEITE vsSeite_Alt);
void __vectorcall VMBlock_ScrollEintrage_Einfugen(STSuchBlock& stSuchBlock);
bool __vectorcall LetzterEintrag(STSuchBlock& stSuchBlock);

void __vectorcall VMFrei_ScrollEintrage_1(STSuchFrei& stSuchFrei);
void __vectorcall VMFrei_ScrollEintrage_2(STSuchFrei& stSuchFrei);
//---------------------------------------------------------------------------
DWORD dwEineSpeicherSeite, dwDoppelSpeicherSeite;
EXCEPTION exFehler_KeinSpeicher = EX_VM_KEINSPEICHER;
//---------------------------------------------------------------------------
#ifndef _64bit
#define _vmSpeicher ((VMSPEICHER)vmSpeicher)
#define _Frei ((VMSEITENKOPF)_vmSpeicher->vsFrei)
#define _ErsterEintragNachsteSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste[16])
#define _ZweiterEintragNachsteSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste[24])
#define _DritterEintragNachsteSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste[32])
#define _TabelleNachsteSeite ((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste
#define _TabelleNachsteNachsteSeite ((VMSEITENKOPF)((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste)->vsNachste
#define _TabelleVorherigeSeite ((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsVorherige
#define _LetzterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[((VMSEITENKOPF)stSuchFrei.vsTabelle)->ulBelegt - 8])
#define _LetzterEintragDoppelSeite (VMEINTRAG)&stSuchFrei.vsTabelle[dwDoppelSpeicherSeite - 8]
#define _LetzterEintragNachsteSeite ((VMEINTRAG)((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste + (((VMSEITENKOPF)((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste)->ulBelegt - 8) / 8)
#define _Tabelle ((VMSEITENKOPF)stSuchFrei.vsTabelle)
#define _Inhalt ((VMSEITENKOPF)stSuchFrei.vsInhalt)
#define _ErsterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[16])
#define _ZweiterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[24])
#define _DritterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[32])
#define _NachsterEintrag (stSuchFrei.veEintrag + 1)
#define _UberNachsterEintrag (stSuchFrei.veEintrag + 2)
#define _VorherigerEintrag (stSuchFrei.veEintrag - 1)
#define _VorVorherigerEintrag (stSuchFrei.veEintrag - 2)
#define _LetzterEintragSeite ((VMEINTRAG)&stSuchFrei.vsTabelle[dwDoppelSpeicherSeite - 8])
#define _VorletzterEintragSeite ((VMEINTRAG)&stSuchFrei.vsTabelle[dwDoppelSpeicherSeite - 16])
#define _VorVorletzterEintragSeite ((VMEINTRAG)&stSuchFrei.vsTabelle[dwDoppelSpeicherSeite - 24])
#define _LetzterEintragVorherigeSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsVorherige[dwDoppelSpeicherSeite - 8])
#define _VorLetzterEintragVorherigeSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsVorherige[dwDoppelSpeicherSeite - 16])
#define _VorVorLetzterEintragVorherigeSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsVorherige[dwDoppelSpeicherSeite - 24])
#define _LetzterEintragInhaltsSeite (VMBLOCK)&stSuchFrei.vsInhalt[dwDoppelSpeicherSeite - stSuchFrei.veEintrag->ulBytes]
#define _ErsterEintragInhaltsSeite (VMBLOCK)&stSuchFrei.vsInhalt[16]
#else
#define _vmSpeicher ((VMSPEICHER)vmSpeicher)
#define _Frei ((VMSEITENKOPF)_vmSpeicher->vsFrei)
#define _ErsterEintragNachsteSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste[32])
#define _ZweiterEintragNachsteSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste[48])
#define _DritterEintragNachsteSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste[64])
#define _TabelleNachsteSeite ((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste
#define _TabelleNachsteNachsteSeite ((VMSEITENKOPF)((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste)->vsNachste
#define _TabelleVorherigeSeite ((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsVorherige
#define _LetzterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[((VMSEITENKOPF)stSuchFrei.vsTabelle)->ulBelegt - 16])
#define _LetzterEintragDoppelSeite (VMEINTRAG)&stSuchFrei.vsTabelle[dwDoppelSpeicherSeite - 16]
#define _LetzterEintragNachsteSeite ((VMEINTRAG)((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste + (((VMSEITENKOPF)((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste)->ulBelegt - 16) / 16)
#define _Tabelle ((VMSEITENKOPF)stSuchFrei.vsTabelle)
#define _Inhalt ((VMSEITENKOPF)stSuchFrei.vsInhalt)
#define _ErsterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[32])
#define _ZweiterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[48])
#define _DritterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[64])
#define _NachsterEintrag (stSuchFrei.veEintrag + 1)
#define _UberNachsterEintrag (stSuchFrei.veEintrag + 2)
#define _VorherigerEintrag (stSuchFrei.veEintrag - 1)
#define _VorVorherigerEintrag (stSuchFrei.veEintrag - 2)
#define _LetzterEintragSeite ((VMEINTRAG)&stSuchFrei.vsTabelle[dwDoppelSpeicherSeite - 16])
#define _VorletzterEintragSeite ((VMEINTRAG)&stSuchFrei.vsTabelle[dwDoppelSpeicherSeite - 32])
#define _VorVorletzterEintragSeite ((VMEINTRAG)&stSuchFrei.vsTabelle[dwDoppelSpeicherSeite - 48])
#define _LetzterEintragVorherigeSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsVorherige[dwDoppelSpeicherSeite - 16])
#define _VorLetzterEintragVorherigeSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsVorherige[dwDoppelSpeicherSeite - 32])
#define _VorVorLetzterEintragVorherigeSeite ((VMEINTRAG)&((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsVorherige[dwDoppelSpeicherSeite - 48])
#define _LetzterEintragInhaltsSeite (VMBLOCK)&stSuchFrei.vsInhalt[dwDoppelSpeicherSeite - stSuchFrei.veEintrag->ulBytes]
#define _ErsterEintragInhaltsSeite (VMBLOCK)&stSuchFrei.vsInhalt[32]
#endif
//---------------------------------------------------------------------------
VMEMORY __vectorcall InitVirtualMemA(_In_ bool bAuslagern)
{
	try{ return InitVirtualMem(bAuslagern); }
	catch(EXCEPTION exFehler){ throw exFehler; return NULL; }
}
//---------------------------------------------------------------------------
VMEMORY __vectorcall InitVirtualMem(_In_ bool bAuslagern)
{
	VMSEITE vsTabelle, vsInhalt;
	if(!(vsTabelle = (VMSEITE)VirtualAlloc(nullptr, dwDoppelSpeicherSeite, MEM_COMMIT, PAGE_READWRITE))) throw exFehler_KeinSpeicher;
	VirtualLock(vsTabelle, dwDoppelSpeicherSeite);

		if(!(vsInhalt = (VMSEITE)VirtualAlloc(NULL, dwDoppelSpeicherSeite, MEM_COMMIT, PAGE_READWRITE))) throw exFehler_KeinSpeicher;
#ifndef _64bit
		((VMSPEICHER)&vsInhalt[16])->bMapped = false;
#else
		((VMSPEICHER)&vsInhalt[32])->bMapped = false;
#endif

	VMSEITE vsFrei;
	if(!(vsFrei = (VMSEITE)VirtualAlloc(NULL, dwDoppelSpeicherSeite, MEM_COMMIT, PAGE_READWRITE))) throw exFehler_KeinSpeicher;
	VirtualLock(vsFrei, dwDoppelSpeicherSeite);

#ifndef _64bit
	((VMSEITENKOPF)vsFrei)->ulBelegt = 16;
#else
	((VMSEITENKOPF)vsFrei)->ulBelegt = 32;
#endif
	((VMSEITENKOPF)vsFrei)->ulReserve = 0;
	((VMSEITENKOPF)vsFrei)->vsVorherige = nullptr;
	((VMSEITENKOPF)vsFrei)->vsNachste = nullptr;
#ifndef _64bit
	((VMSPEICHER)&vsInhalt[16])->vsFrei = vsFrei;
#else
	((VMSPEICHER)&vsInhalt[32])->vsFrei = vsFrei;
#endif

#ifndef _64bit
	((VMSPEICHER)&vsInhalt[16])->bAuslagern = bAuslagern;
	((VMSPEICHER)&vsInhalt[16])->vsNeueTabelle = nullptr;
#else
	((VMSPEICHER)&vsInhalt[32])->bAuslagern = bAuslagern;
	((VMSPEICHER)&vsInhalt[32])->vsNeueTabelle = nullptr;
#endif

#ifndef _64bit
	((VMSEITENKOPF)vsTabelle)->ulBelegt = 32;
#else
	((VMSEITENKOPF)vsTabelle)->ulBelegt = 64;
#endif
	((VMSEITENKOPF)vsTabelle)->ulReserve = 0;
	((VMSEITENKOPF)vsTabelle)->vsVorherige = nullptr;
	((VMSEITENKOPF)vsTabelle)->vsNachste = nullptr;
#ifndef _64bit
	((VMSPEICHER)&vsInhalt[16])->vsErsteTabelle = vsTabelle;
	((VMSPEICHER)&vsInhalt[16])->vsLetzteTabelle = vsTabelle;
#else
	((VMSPEICHER)&vsInhalt[32])->vsErsteTabelle = vsTabelle;
	((VMSPEICHER)&vsInhalt[32])->vsLetzteTabelle = vsTabelle;
#endif

#ifndef _64bit
	((VMSEITENKOPF)vsInhalt)->ulBelegt = 16 + sizeof(STVirtualSpeicher);
#else
	((VMSEITENKOPF)vsInhalt)->ulBelegt = 32 + sizeof(STVirtualSpeicher);
#endif
	((VMSEITENKOPF)vsInhalt)->ulReserve = 0;
	((VMSEITENKOPF)vsInhalt)->vsVorherige = nullptr;
	((VMSEITENKOPF)vsInhalt)->vsNachste = nullptr;
#ifndef _64bit
	((VMSPEICHER)&vsInhalt[16])->vsErsteInhalt = vsInhalt;
	((VMSPEICHER)&vsInhalt[16])->vsLetzteInhaltKlein = vsInhalt;
	((VMSPEICHER)&vsInhalt[16])->vsLetzteInhaltGross = nullptr;
	((VMSPEICHER)&vsInhalt[16])->vfFrei = (VMFREI)&vsFrei[16];
	((VMSPEICHER)&vsInhalt[16])->vfBlock = (VMFREI)&vsFrei[16];
#else
	((VMSPEICHER)&vsInhalt[32])->vsErsteInhalt = vsInhalt;
	((VMSPEICHER)&vsInhalt[32])->vsLetzteInhaltKlein = vsInhalt;
	((VMSPEICHER)&vsInhalt[32])->vsLetzteInhaltGross = nullptr;
	((VMSPEICHER)&vsInhalt[32])->vfFrei = (VMFREI)&vsFrei[32];
	((VMSPEICHER)&vsInhalt[32])->vfBlock = (VMFREI)&vsFrei[32];
#endif

#ifndef _64bit
	((VMEINTRAG)&vsTabelle[16])->ulBytes = sizeof(STVirtualSpeicher);
	((VMEINTRAG)&vsTabelle[16])->vbAdresse = (VMBLOCK)&vsInhalt[16];

	((VMEINTRAG)&vsTabelle[24])->ulBytes = dwDoppelSpeicherSeite - 16 - sizeof(STVirtualSpeicher);
	((VMEINTRAG)&vsTabelle[24])->vbAdresse = nullptr;

	InitializeCriticalSectionAndSpinCount(&((VMSPEICHER)&vsInhalt[16])->csTabelle, 1000);
	InitializeCriticalSectionAndSpinCount(&((VMSPEICHER)&vsInhalt[16])->csFrei, 1000);

	((VMSPEICHER)&vsInhalt[16])->thvmFrei.vmSpeicher = ((VMSPEICHER)&vsInhalt[16]);
	((VMSPEICHER)&vsInhalt[16])->thvmFrei.bEnde = false;
	((VMSPEICHER)&vsInhalt[16])->thvmFrei.heFertig = CreateEvent(NULL, false, false, NULL);
	((VMSPEICHER)&vsInhalt[16])->thvmFrei.hthFrei = CreateThread(NULL, 0, thFrei, &((VMSPEICHER)&vsInhalt[16])->thvmFrei, CREATE_SUSPENDED, NULL);
	SetThreadPriority(((VMSPEICHER)&vsInhalt[16])->thvmFrei.hthFrei, THREAD_PRIORITY_LOWEST);
	ResumeThread(((VMSPEICHER)&vsInhalt[16])->thvmFrei.hthFrei);

	return ((VMSPEICHER)&vsInhalt[16]);
#else
	((VMEINTRAG)&vsTabelle[32])->ulBytes = sizeof(STVirtualSpeicher);
	((VMEINTRAG)&vsTabelle[32])->vbAdresse = (VMBLOCK)&vsInhalt[32];

	((VMEINTRAG)&vsTabelle[48])->ulBytes = dwDoppelSpeicherSeite - 32 - sizeof(STVirtualSpeicher);
	((VMEINTRAG)&vsTabelle[48])->vbAdresse = nullptr;

	InitializeCriticalSectionAndSpinCount(&((VMSPEICHER)&vsInhalt[32])->csTabelle, 1000);
	InitializeCriticalSectionAndSpinCount(&((VMSPEICHER)&vsInhalt[32])->csFrei, 1000);

	((VMSPEICHER)&vsInhalt[32])->thvmFrei.vmSpeicher = ((VMSPEICHER)&vsInhalt[32]);
	((VMSPEICHER)&vsInhalt[32])->thvmFrei.bEnde = false;
	((VMSPEICHER)&vsInhalt[32])->thvmFrei.heFertig = CreateEvent(NULL, false, false, NULL);
	((VMSPEICHER)&vsInhalt[32])->thvmFrei.hthFrei = CreateThread(NULL, 0, thFrei, &((VMSPEICHER)&vsInhalt[32])->thvmFrei, CREATE_SUSPENDED, NULL);
	SetThreadPriority(((VMSPEICHER)&vsInhalt[32])->thvmFrei.hthFrei, THREAD_PRIORITY_LOWEST);
	ResumeThread(((VMSPEICHER)&vsInhalt[32])->thvmFrei.hthFrei);

	return ((VMSPEICHER)&vsInhalt[32]);
#endif
}
//---------------------------------------------------------------------------
void __vectorcall FreeVirtualMem(_In_ VMEMORY vmSpeicher)
{
	void* pvIterator = nullptr;

	_vmSpeicher->thvmFrei.bEnde = true;
	_vmSpeicher->vfBlock++;
	SetEvent(_vmSpeicher->thvmFrei.heFertig);
	WaitForSingleObject(_vmSpeicher->thvmFrei.hthFrei, 10000);
	CloseHandle(_vmSpeicher->thvmFrei.hthFrei);
	DeleteCriticalSection(&_vmSpeicher->csFrei);

	VMSEITE vsTabelle, vsLoschen;
	vsTabelle = _vmSpeicher->vsErsteTabelle;
	while(vsTabelle){
		vsLoschen = vsTabelle;
		vsTabelle = ((VMSEITENKOPF)vsLoschen)->vsNachste;
		VirtualFree(vsLoschen, 0, MEM_RELEASE);
	}

	while(_Frei->vsVorherige){ _vmSpeicher->vsFrei = _Frei->vsVorherige; }
	while(_vmSpeicher->vsFrei){
		vsLoschen = _vmSpeicher->vsFrei;
		_vmSpeicher->vsFrei = ((VMSEITENKOPF)vsLoschen)->vsNachste;
		VirtualFree(vsLoschen, 0, MEM_RELEASE);
	}

	DeleteCriticalSection(&_vmSpeicher->csTabelle);

	VMSEITE vsInhalt = _vmSpeicher->vsLetzteInhaltKlein;
	while(((VMSEITENKOPF)vsInhalt)->vsNachste){ vsInhalt = ((VMSEITENKOPF)vsInhalt)->vsNachste; }
	while(vsInhalt){
		vsLoschen = vsInhalt;
		vsInhalt = ((VMSEITENKOPF)vsLoschen)->vsVorherige;
		VirtualFree(vsLoschen, 0, MEM_RELEASE);
	}
}
//---------------------------------------------------------------------------
INLINE VMBLOCK __vectorcall GrosserEineSeite(_In_ VMEMORY vmSpeicher, _In_ unsigned long ulBytes)
{
#ifndef _64bit
	VMSEITE vsNeueInhalt; ULONG ulBytes_Seite = dwEineSpeicherSeite * ((ulBytes + 16) / dwEineSpeicherSeite + 1);
#else
	VMSEITE vsNeueInhalt; ULONG ulBytes_Seite = dwEineSpeicherSeite * ((ulBytes + 32) / dwEineSpeicherSeite + 1);
#endif
	if(!(vsNeueInhalt = (VMSEITE)VirtualAlloc(NULL, ulBytes_Seite, MEM_COMMIT, PAGE_READWRITE))){ LeaveCriticalSection(&_vmSpeicher->csTabelle); throw exFehler_KeinSpeicher; }
	if(!_vmSpeicher->bAuslagern) VirtualLock(vsNeueInhalt, ulBytes_Seite);

#ifndef _64bit
	((VMSEITENKOPF)vsNeueInhalt)->ulBelegt = 16 + ulBytes;
#else
	((VMSEITENKOPF)vsNeueInhalt)->ulBelegt = 32 + ulBytes;
#endif
	((VMSEITENKOPF)vsNeueInhalt)->ulReserve = 0;
	if(_vmSpeicher->vsLetzteInhaltGross){
		((VMSEITENKOPF)vsNeueInhalt)->vsVorherige = _vmSpeicher->vsLetzteInhaltGross;
		((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltGross)->vsNachste = vsNeueInhalt;
	}
	else{
		((VMSEITENKOPF)vsNeueInhalt)->vsVorherige = _vmSpeicher->vsLetzteInhaltKlein;
		((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste = vsNeueInhalt;
	}
	((VMSEITENKOPF)vsNeueInhalt)->vsNachste = nullptr;
	_vmSpeicher->vsLetzteInhaltGross = vsNeueInhalt;

#ifndef _64bit
	VMBLOCK vbAdresse = (VMBLOCK)&vsNeueInhalt[16];
#else
	VMBLOCK vbAdresse = (VMBLOCK)&vsNeueInhalt[32];
#endif
	LeaveCriticalSection(&_vmSpeicher->csTabelle);
	return vbAdresse;
}
//---------------------------------------------------------------------------
INLINE void __vectorcall NeueInhaltSeite(_In_ VMEMORY vmSpeicher, _In_ STSuchBlock& stSuchBlock)
{
	VMSEITE vsNeueInhalt;
	if(!(vsNeueInhalt = (VMSEITE)VirtualAlloc(NULL, dwDoppelSpeicherSeite, MEM_COMMIT, PAGE_READWRITE))){ LeaveCriticalSection(&_vmSpeicher->csTabelle); throw exFehler_KeinSpeicher; }
	if(!_vmSpeicher->bAuslagern) VirtualLock(vsNeueInhalt, dwDoppelSpeicherSeite);

#ifndef _64bit
	((VMSEITENKOPF)vsNeueInhalt)->ulBelegt = 16;
#else
	((VMSEITENKOPF)vsNeueInhalt)->ulBelegt = 32;
#endif

	((VMSEITENKOPF)vsNeueInhalt)->vsNachste = nullptr;
	((VMSEITENKOPF)vsNeueInhalt)->vsVorherige = _vmSpeicher->vsLetzteInhaltKlein;
	if(((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste){
		((VMSEITENKOPF)vsNeueInhalt)->vsNachste = ((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste;
		((VMSEITENKOPF)((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste)->vsVorherige = vsNeueInhalt;
	}
	((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste = vsNeueInhalt;
	_vmSpeicher->vsLetzteInhaltKlein = vsNeueInhalt;
	stSuchBlock.vsInhalt = _vmSpeicher->vsLetzteInhaltKlein;
}
//---------------------------------------------------------------------------
INLINE void __vectorcall NeueTabelleSeite(_In_ VMEMORY vmSpeicher, _In_ STSuchBlock& stSuchBlock)
{
	if(!(_vmSpeicher->vsNeueTabelle = (VMSEITE)VirtualAlloc(NULL, dwDoppelSpeicherSeite, MEM_COMMIT, PAGE_READWRITE))){ LeaveCriticalSection(&_vmSpeicher->csTabelle); throw exFehler_KeinSpeicher; }
	VirtualLock(_vmSpeicher->vsNeueTabelle, dwDoppelSpeicherSeite);

#ifndef _64bit
	((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt = 16;
#else
	((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt = 32;
#endif

	((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->vsNachste = nullptr;
	((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->vsVorherige = stSuchBlock.vsTabelle;
	((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste = _vmSpeicher->vsNeueTabelle;
	_vmSpeicher->vsLetzteTabelle = _vmSpeicher->vsNeueTabelle;
}
//---------------------------------------------------------------------------
INLINE void __vectorcall ScrollEintrage_VonEnde(_In_ VMEMORY vmSpeicher, _In_ STSuchBlock& stSuchBlock)
{
	VMSEITE vsSeite_Aktuell = stSuchBlock.vsTabelle;
	stSuchBlock.vsTabelle = _vmSpeicher->vsLetzteTabelle;
	if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
		NeueTabelleSeite(vmSpeicher, stSuchBlock);
#ifndef _64bit
		((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->ulBytes = ((VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 8])->ulBytes;
		((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->vbAdresse = nullptr;
		((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 8;
#else
		((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->ulBytes = ((VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 16])->ulBytes;
		((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->vbAdresse = nullptr;
		((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 16;
#endif
}
	VMBlock_ScrollEintrage_VonEnde(stSuchBlock, vsSeite_Aktuell);
}
//---------------------------------------------------------------------------
INLINE VMBLOCK __vectorcall VMEintragGleichOderGrosser(_In_ VMEMORY vmSpeicher, _In_ STSuchBlock& stSuchBlock, _In_ VMBLOCK vbAdresse)
{
	if(stSuchBlock.veEintrag->ulBytes == stSuchBlock.ulBytes){
#ifndef _64bit
		if(!stSuchBlock.veVorheriger) vbAdresse = (VMBLOCK)&stSuchBlock.vsInhalt[16];
#else
		if(!stSuchBlock.veVorheriger) vbAdresse = (VMBLOCK)&stSuchBlock.vsInhalt[32];
#endif
		else vbAdresse = stSuchBlock.veVorheriger->vbAdresse + stSuchBlock.veVorheriger->ulBytes;

#ifndef _64bit
		if(stSuchBlock.ucAlign && (ULONG)vbAdresse % stSuchBlock.ucAlign) return nullptr;
#else 
		if(stSuchBlock.ucAlign && (unsigned long long)vbAdresse % stSuchBlock.ucAlign) return nullptr;
#endif

		if(LetzterEintrag(stSuchBlock)){
			if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
				if(!((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste){
					NeueTabelleSeite(vmSpeicher, stSuchBlock);
					NeueInhaltSeite(vmSpeicher, stSuchBlock);
#ifndef _64bit
					((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->ulBytes = dwDoppelSpeicherSeite - 16;
					((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->vbAdresse = nullptr;
					((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 8;
#else
					((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->ulBytes = dwDoppelSpeicherSeite - 32;
					((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->vbAdresse = nullptr;
					((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 16;
#endif
					stSuchBlock.vsInhalt = ((VMSEITENKOPF)stSuchBlock.vsInhalt)->vsVorherige;
				}
			}
			else{
				NeueInhaltSeite(vmSpeicher, stSuchBlock);

				stSuchBlock.veEintrag++;
				stSuchBlock.veEintrag->vbAdresse = nullptr;
#ifndef _64bit
				stSuchBlock.veEintrag->ulBytes = dwDoppelSpeicherSeite - 16;
				((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
				stSuchBlock.veEintrag->ulBytes = dwDoppelSpeicherSeite - 32;
				((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
				stSuchBlock.veEintrag--;
				stSuchBlock.vsInhalt = ((VMSEITENKOPF)stSuchBlock.vsInhalt)->vsVorherige;
			}
		}
		ZeroMem(vbAdresse, stSuchBlock.ulBytes);
		((VMSEITENKOPF)stSuchBlock.vsInhalt)->ulBelegt += stSuchBlock.ulBytes;
		stSuchBlock.veEintrag->vbAdresse = vbAdresse;
		LeaveCriticalSection(&_vmSpeicher->csTabelle);
	}
	else if(stSuchBlock.veEintrag->ulBytes > stSuchBlock.ulBytes){
		ULONG ulBytes_Align, ulBytes_Eintrag;
		ulBytes_Eintrag = stSuchBlock.veEintrag->ulBytes;

#ifndef _64bit
		if(!stSuchBlock.veVorheriger) vbAdresse = (VMBLOCK)&stSuchBlock.vsInhalt[16];
#else
		if(!stSuchBlock.veVorheriger) vbAdresse = (VMBLOCK)&stSuchBlock.vsInhalt[32];
#endif
		else vbAdresse = stSuchBlock.veVorheriger->vbAdresse + stSuchBlock.veVorheriger->ulBytes;

		if(stSuchBlock.ucAlign){
			ulBytes_Align = stSuchBlock.veEintrag->ulBytes;
			while(ulBytes_Align >= stSuchBlock.ulBytes){
#ifndef _64bit
				if(!((ULONG)vbAdresse % stSuchBlock.ucAlign)) break;
#else 
				if(!((unsigned long long)vbAdresse % stSuchBlock.ucAlign)) break;
#endif
				else{ vbAdresse++; ulBytes_Align--; }
			}
			if(ulBytes_Align < stSuchBlock.ulBytes) return nullptr;
			else if(ulBytes_Align == stSuchBlock.veEintrag->ulBytes) stSuchBlock.ucAlign = 0;
		}

		if(LetzterEintrag(stSuchBlock)){
			if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
				if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste){
					ScrollEintrage_VonEnde(vmSpeicher, stSuchBlock);

					if(stSuchBlock.ucAlign){
						if(ulBytes_Align == stSuchBlock.ulBytes){
							stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
#ifndef _64bit
							stSuchBlock.veEintrag = ((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16]);
#else
							stSuchBlock.veEintrag = ((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[32]);
#endif
						}
						else{
							ScrollEintrage_VonEnde(vmSpeicher, stSuchBlock);
							stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
#ifndef _64bit
							stSuchBlock.veEintrag = ((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[16]);
							((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[24])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
							((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[24])->vbAdresse = nullptr;
#else
							stSuchBlock.veEintrag = ((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[32]);
							((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[48])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
							((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[48])->vbAdresse = nullptr;
#endif
						}
					}
					else{
#ifndef _64bit
						((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[16])->ulBytes = ulBytes_Eintrag - stSuchBlock.ulBytes;
						((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[16])->vbAdresse = nullptr;
#else
						((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[32])->ulBytes = ulBytes_Eintrag - stSuchBlock.ulBytes;
						((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[32])->vbAdresse = nullptr;
#endif
					}
				}
				else{
					NeueTabelleSeite(vmSpeicher, stSuchBlock);

					if(stSuchBlock.ucAlign){
						if(ulBytes_Align == stSuchBlock.ulBytes){
							stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
#ifndef _64bit
							stSuchBlock.veEintrag = ((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16]);
							((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 8;
#else
							stSuchBlock.veEintrag = ((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32]);
							((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 16;
#endif
							NeueInhaltSeite(vmSpeicher, stSuchBlock);
#ifndef _64bit
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[24])->ulBytes = dwDoppelSpeicherSeite - 16;
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[24])->vbAdresse = nullptr;
							((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 8;
#else
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[48])->ulBytes = dwDoppelSpeicherSeite - 32;
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[48])->vbAdresse = nullptr;
							((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 16;
#endif
							stSuchBlock.vsInhalt = ((VMSEITENKOPF)stSuchBlock.vsInhalt)->vsVorherige;
						}
						else{
							stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
#ifndef _64bit
							stSuchBlock.veEintrag = ((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16]);
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[24])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[24])->vbAdresse = nullptr;
							((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 16;
#else
							stSuchBlock.veEintrag = ((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32]);
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[48])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[48])->vbAdresse = nullptr;
							((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 32;
#endif
						}
					}
					else{
#ifndef _64bit
						((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->ulBytes = stSuchBlock.veEintrag->ulBytes - stSuchBlock.ulBytes;
						((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->vbAdresse = nullptr;
						((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 8;
#else
						((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->ulBytes = stSuchBlock.veEintrag->ulBytes - stSuchBlock.ulBytes;
						((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->vbAdresse = nullptr;
						((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 16;
#endif
					}
				}
			}
			else{
				if(stSuchBlock.ucAlign){
					if(ulBytes_Align == stSuchBlock.ulBytes){
						stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
						stSuchBlock.veEintrag++;
#ifndef _64bit
						((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
						((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
						NeueInhaltSeite(vmSpeicher, stSuchBlock);

						if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
							NeueTabelleSeite(vmSpeicher, stSuchBlock);
#ifndef _64bit
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->ulBytes = dwDoppelSpeicherSeite - 16;
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->vbAdresse = nullptr;
							((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 8;
#else
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->ulBytes = dwDoppelSpeicherSeite - 32;
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->vbAdresse = nullptr;
							((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 16;
#endif
						}
						else{
							stSuchBlock.veEintrag++;
#ifndef _64bit
							stSuchBlock.veEintrag->ulBytes = dwDoppelSpeicherSeite - 16;
							stSuchBlock.veEintrag->vbAdresse = nullptr;
							((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
							stSuchBlock.veEintrag->ulBytes = dwDoppelSpeicherSeite - 32;
							stSuchBlock.veEintrag->vbAdresse = nullptr;
							((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
							stSuchBlock.veEintrag--;
						}
						stSuchBlock.vsInhalt = ((VMSEITENKOPF)stSuchBlock.vsInhalt)->vsVorherige;
					}
					else{
						stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
						stSuchBlock.veEintrag++;
#ifndef _64bit
						((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
						((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
						if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
							NeueTabelleSeite(vmSpeicher, stSuchBlock);
#ifndef _64bit
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->vbAdresse = nullptr;
							((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 8;
#else
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
							((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->vbAdresse = nullptr;
							((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 16;
#endif
						}
						else{
							stSuchBlock.veEintrag++;
							stSuchBlock.veEintrag->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
							stSuchBlock.veEintrag->vbAdresse = nullptr;
							stSuchBlock.veEintrag--;
#ifndef _64bit
							((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
							((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
						}
					}
				}
				else{
					stSuchBlock.veEintrag++;
					stSuchBlock.veEintrag->ulBytes = ulBytes_Eintrag - stSuchBlock.ulBytes;
					stSuchBlock.veEintrag->vbAdresse = nullptr;
					stSuchBlock.veEintrag--;
#ifndef _64bit
					((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
					((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
				}
			}
		}
		else{
			if(stSuchBlock.ucAlign){
				if(ulBytes_Align == stSuchBlock.ulBytes){
					if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite) ScrollEintrage_VonEnde(vmSpeicher, stSuchBlock);
					VMBlock_ScrollEintrage_Einfugen(stSuchBlock);

					stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
					stSuchBlock.veEintrag++;
				}
				else{
					if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite) ScrollEintrage_VonEnde(vmSpeicher, stSuchBlock);
					VMBlock_ScrollEintrage_Einfugen(stSuchBlock);
					if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite) ScrollEintrage_VonEnde(vmSpeicher, stSuchBlock);
					VMBlock_ScrollEintrage_Einfugen(stSuchBlock);

					stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
					stSuchBlock.veEintrag++;
#ifndef _64bit
					if(stSuchBlock.veEintrag == ((VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 8])){
						((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[16])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
						((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[16])->vbAdresse = nullptr;
#else
					if(stSuchBlock.veEintrag == ((VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 16])){
						((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[32])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
						((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[32])->vbAdresse = nullptr;
#endif
					}
					else{
						stSuchBlock.veEintrag++;
						stSuchBlock.veEintrag->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
						stSuchBlock.veEintrag->vbAdresse = nullptr;
						stSuchBlock.veEintrag--;
					}
				}
			}
			else{
				if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite) ScrollEintrage_VonEnde(vmSpeicher, stSuchBlock);
				VMBlock_ScrollEintrage_Einfugen(stSuchBlock);
			}
		}
		ZeroMem(vbAdresse, stSuchBlock.ulBytes);
		((VMSEITENKOPF)stSuchBlock.vsInhalt)->ulBelegt += stSuchBlock.ulBytes;
		stSuchBlock.veEintrag->ulBytes = stSuchBlock.ulBytes;
		stSuchBlock.veEintrag->vbAdresse = vbAdresse;
		LeaveCriticalSection(&_vmSpeicher->csTabelle);
		}
	return vbAdresse;
	}
//---------------------------------------------------------------------------
VMBLOCK __vectorcall VMBlock(_In_ VMEMORY vmSpeicher, _In_ unsigned long ulBytes, _In_ unsigned char ucAlign)
{
	if(!ulBytes) return nullptr;
	if(!vmSpeicher) vmSpeicher = vmStandard;
	EnterCriticalSection(&_vmSpeicher->csTabelle);

#ifndef _64bit
	if(ulBytes >= dwEineSpeicherSeite - 16) return GrosserEineSeite(vmSpeicher, ulBytes);
#else
	if(ulBytes >= dwEineSpeicherSeite - 32) return GrosserEineSeite(vmSpeicher, ulBytes);
#endif
	else{
		VMBLOCK vbAdresse;
		STSuchBlock stSuchBlock;
		stSuchBlock.ulBytes = ulBytes;
		stSuchBlock.ucAlign = ucAlign;
		stSuchBlock.vsTabelle = _vmSpeicher->vsErsteTabelle;
		stSuchBlock.vsInhalt = _vmSpeicher->vsErsteInhalt;
		stSuchBlock.veVorheriger = nullptr;
#ifndef _64bit
		stSuchBlock.ulInhaltBelegt = 16;
		stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[16];
		stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 8];
#else
		stSuchBlock.ulInhaltBelegt = 32;
		stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[32];
		stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 16];
#endif

		do{
			do{
				while(stSuchBlock.veEintrag->vbAdresse){
Kleiner:
					stSuchBlock.ulInhaltBelegt += stSuchBlock.veEintrag->ulBytes;
					if(stSuchBlock.ulInhaltBelegt == dwDoppelSpeicherSeite){
						stSuchBlock.vsInhalt = ((VMSEITENKOPF)stSuchBlock.vsInhalt)->vsNachste; stSuchBlock.veVorheriger = nullptr;
#ifndef _64bit
						stSuchBlock.ulInhaltBelegt = 16;
#else 
						stSuchBlock.ulInhaltBelegt = 32;
#endif
					}
					else stSuchBlock.veVorheriger = stSuchBlock.veEintrag;

					if(stSuchBlock.veEintrag == stSuchBlock.veLetzterEintragDoppelSeite){
						stSuchBlock.vsTabelle = ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste;

#ifndef _64bit
						stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[16];
						stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 8];
#else
						stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[32];
						stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 16];
#endif
					}
					else stSuchBlock.veEintrag++;
				}

				vbAdresse = nullptr;
				if(vbAdresse = VMEintragGleichOderGrosser(vmSpeicher, stSuchBlock, vbAdresse)) return vbAdresse;
				else if(!(((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste) && LetzterEintrag(stSuchBlock)) break;
				goto Kleiner;
			}
			while(TRUE);

			NeueInhaltSeite(vmSpeicher, stSuchBlock);

			if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
				NeueTabelleSeite(vmSpeicher, stSuchBlock);
				stSuchBlock.vsTabelle = ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste;

#ifndef _64bit
				((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->ulBytes = dwDoppelSpeicherSeite - 16;
				((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->vbAdresse = nullptr;
				((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 8;

				stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[16];
				stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 8];
#else
				((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->ulBytes = dwDoppelSpeicherSeite - 32;
				((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->vbAdresse = nullptr;
				((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 16;

				stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[32];
				stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 16];
#endif
				stSuchBlock.veVorheriger = nullptr;
			}
			else{
				stSuchBlock.veEintrag++;
				stSuchBlock.veEintrag->vbAdresse = nullptr;
#ifndef _64bit
				((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
				stSuchBlock.veEintrag->ulBytes = dwDoppelSpeicherSeite - 16;
#else 
				((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
				stSuchBlock.veEintrag->ulBytes = dwDoppelSpeicherSeite - 32;
#endif
				stSuchBlock.veVorheriger = nullptr;
			}
		}
		while(TRUE);
	}
}
//---------------------------------------------------------------------------
VMBLOCK __vectorcall VMBlockS(_In_ VMEMORY vmSpeicher, _In_ unsigned long ulBytes, _In_ unsigned char ucAlign)
{
	if(!ulBytes) return nullptr;
	if(!vmSpeicher) vmSpeicher = vmStandard;
	EnterCriticalSection(&_vmSpeicher->csTabelle);

#ifndef _64bit
	if(ulBytes >= dwEineSpeicherSeite - 16) return GrosserEineSeite(vmSpeicher, ulBytes);
#else
	if(ulBytes >= dwEineSpeicherSeite - 32) return GrosserEineSeite(vmSpeicher, ulBytes);
#endif
	else{
		VMBLOCK vbAdresse;
		STSuchBlock stSuchBlock;
		stSuchBlock.ulBytes = ulBytes;
		stSuchBlock.ucAlign = ucAlign;
		stSuchBlock.ulInhaltBelegt = 0;
		stSuchBlock.vsTabelle = _vmSpeicher->vsLetzteTabelle;
		stSuchBlock.vsInhalt = _vmSpeicher->vsLetzteInhaltKlein;
		stSuchBlock.veVorheriger = nullptr;

Kleiner:
#ifndef _64bit
		stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 8];
		stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt - 8];

		if(((VMSEITENKOPF)stSuchBlock.vsInhalt)->ulBelegt == 16) stSuchBlock.veVorheriger = nullptr;
		else if(stSuchBlock.veEintrag == ((VMEINTRAG)&stSuchBlock.vsTabelle[16]))
			stSuchBlock.veVorheriger = ((VMEINTRAG) & ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsVorherige[dwDoppelSpeicherSeite - 8]);
#else
		stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 16];
		stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt - 16];

		stSuchBlock.ulInhaltBelegt = ((VMSEITENKOPF)stSuchBlock.vsInhalt)->ulBelegt;
		if(((VMSEITENKOPF)stSuchBlock.vsInhalt)->ulBelegt == 32) stSuchBlock.veVorheriger = nullptr;
		else if(stSuchBlock.veEintrag == ((VMEINTRAG)&stSuchBlock.vsTabelle[32]))
			stSuchBlock.veVorheriger = ((VMEINTRAG) & ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsVorherige[dwDoppelSpeicherSeite - 16]);
#endif
		else stSuchBlock.veVorheriger = (stSuchBlock.veEintrag - 1);

		do{
			do{
				vbAdresse = nullptr;
				if(vbAdresse = VMEintragGleichOderGrosser(vmSpeicher, stSuchBlock, vbAdresse)) return vbAdresse;
				else if(!(((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste) && LetzterEintrag(stSuchBlock)) break;
				goto Kleiner;
			}
			while(TRUE);

			NeueInhaltSeite(vmSpeicher, stSuchBlock);

			if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
				NeueTabelleSeite(vmSpeicher, stSuchBlock);
				stSuchBlock.vsTabelle = ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste;
#ifndef _64bit
				((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->ulBytes = dwDoppelSpeicherSeite - 16;
				((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[16])->vbAdresse = nullptr;
				((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 8;

				stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[16];
				stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 8];
#else
				((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->ulBytes = dwDoppelSpeicherSeite - 32;
				((VMEINTRAG)&_vmSpeicher->vsNeueTabelle[32])->vbAdresse = nullptr;
				((VMSEITENKOPF)_vmSpeicher->vsNeueTabelle)->ulBelegt += 16;

				stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[32];
				stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 16];
#endif
				stSuchBlock.veVorheriger = nullptr;
			}
			else{
				stSuchBlock.veEintrag++;
				stSuchBlock.veEintrag->vbAdresse = nullptr;
#ifndef _64bit
				((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
				stSuchBlock.veEintrag->ulBytes = dwDoppelSpeicherSeite - 16;
#else 
				((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
				stSuchBlock.veEintrag->ulBytes = dwDoppelSpeicherSeite - 32;
#endif
				stSuchBlock.veVorheriger = nullptr;
			}
			}
		while(TRUE);
		}
}
//---------------------------------------------------------------------------
INLINE void __vectorcall VMFrei_Tabelle(_In_ VMEMORY vmSpeicher, _In_ STSuchFrei& stSuchFrei)
{
	_vmSpeicher->vsLetzteTabelle = stSuchFrei.vsTabelle;
  VirtualFree(_TabelleNachsteSeite, 0, MEM_RELEASE);
	_TabelleNachsteSeite = nullptr;
}
//---------------------------------------------------------------------------
INLINE void __vectorcall VMFrei_ScrollEintrage(_In_ VMEMORY vmSpeicher, _In_ STSuchFrei& stSuchFrei)
{
	VMFrei_ScrollEintrage_1(stSuchFrei);

	while(_TabelleNachsteSeite){
		stSuchFrei.vsTabelle = _TabelleNachsteSeite;
#ifndef _64bit
		stSuchFrei.ucAdresse = (BYTE)(stSuchFrei.ulScrollSprung / 8);
		if(stSuchFrei.ucAdresse < (_Tabelle->ulBelegt - 16) / 8){
#else
		stSuchFrei.ucAdresse = (BYTE)(stSuchFrei.ulScrollSprung / 16);
		if(stSuchFrei.ucAdresse < (_Tabelle->ulBelegt - 32) / 16){
#endif
			if(stSuchFrei.ucAdresse == 3){
				*_VorVorLetzterEintragVorherigeSeite = *_ErsterEintrag;
				*_VorLetzterEintragVorherigeSeite = *_ZweiterEintrag;
				*_LetzterEintragVorherigeSeite = *_DritterEintrag;
			}
			else if(stSuchFrei.ucAdresse == 2){
				*_VorLetzterEintragVorherigeSeite = *_ErsterEintrag;
				*_LetzterEintragVorherigeSeite = *_ZweiterEintrag;
			}
			else{
				*_LetzterEintragVorherigeSeite = *_ErsterEintrag;
			}

			VMFrei_ScrollEintrage_2(stSuchFrei);
		}
#ifndef _64bit
		else if(stSuchFrei.ucAdresse == (_Tabelle->ulBelegt - 16) / 8){
#else
		else if(stSuchFrei.ucAdresse == (_Tabelle->ulBelegt - 32) / 16){
#endif
			if(stSuchFrei.ucAdresse == 3){
				*_VorVorLetzterEintragVorherigeSeite = *_ErsterEintrag;
				*_VorLetzterEintragVorherigeSeite = *_ZweiterEintrag;
				*_LetzterEintragVorherigeSeite = *_DritterEintrag;
				stSuchFrei.ulScrollSprung = 0;
			}
			else if(stSuchFrei.ucAdresse == 2){
				*_VorLetzterEintragVorherigeSeite = *_ErsterEintrag;
				*_LetzterEintragVorherigeSeite = *_ZweiterEintrag;
				stSuchFrei.ulScrollSprung = 0;
			}
			else{
				*_LetzterEintragVorherigeSeite = *_ErsterEintrag;
				stSuchFrei.ulScrollSprung = 0;
			}
			stSuchFrei.vsTabelle = _Tabelle->vsVorherige;
			VMFrei_Tabelle(vmSpeicher, stSuchFrei);
		}
		else{
			if(stSuchFrei.ucAdresse == 2){
				*_VorLetzterEintragVorherigeSeite = *_ErsterEintrag;
#ifndef _64bit
				stSuchFrei.ulScrollSprung = 8;
#else
				stSuchFrei.ulScrollSprung = 16;
#endif
			}
			else{
				*_VorVorLetzterEintragVorherigeSeite = *_ErsterEintrag;
#ifndef _64bit
				if((_Tabelle->ulBelegt - 16) / 8 == 2){
#else
				if((_Tabelle->ulBelegt - 32) / 16 == 2){
#endif
					*_VorLetzterEintragVorherigeSeite = *_ZweiterEintrag;
#ifndef _64bit
					stSuchFrei.ulScrollSprung = 8;
				}
				else stSuchFrei.ulScrollSprung = 16;
#else
					stSuchFrei.ulScrollSprung = 16;
				}
				else stSuchFrei.ulScrollSprung = 32;
#endif
			}
			stSuchFrei.vsTabelle = _Tabelle->vsVorherige;
			VMFrei_Tabelle(vmSpeicher, stSuchFrei);
		}
	}
	_Tabelle->ulBelegt -= stSuchFrei.ulScrollSprung;
}
//---------------------------------------------------------------------------
void __vectorcall VMFrei(_In_ VMEMORY vmSpeicher, _In_ STSuchFrei& stSuchFrei)
{
	stSuchFrei.vsInhalt = _vmSpeicher->vsLetzteInhaltKlein;

	do{
		if(stSuchFrei.veEintrag->vbAdresse >= stSuchFrei.vsInhalt && stSuchFrei.veEintrag->vbAdresse <= stSuchFrei.vsInhalt + dwDoppelSpeicherSeite){
			_Inhalt->ulBelegt -= stSuchFrei.veEintrag->ulBytes;

#ifndef _64bit
			if(_Inhalt->ulBelegt == 16){
#else
			if(_Inhalt->ulBelegt == 32){
#endif
				if(stSuchFrei.veEintrag->ulBytes < dwEineSpeicherSeite - 32){
					if(stSuchFrei.veEintrag->vbAdresse == _ErsterEintragInhaltsSeite){
						if(stSuchFrei.veEintrag == _ErsterEintrag){
							if(_NachsterEintrag == _LetzterEintrag){
								if(_LetzterEintragVorherigeSeite->vbAdresse){
									stSuchFrei.veEintrag->vbAdresse = nullptr;
									stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
#ifndef _64bit
									_Tabelle->ulBelegt -= 8;
#else
									_Tabelle->ulBelegt -= 16;
#endif
									break;
								}
								else{
									stSuchFrei.vsTabelle = _TabelleVorherigeSeite;
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
								}
							}
							else{
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
								stSuchFrei.ulScrollSprung = 16;
#else
								stSuchFrei.ulScrollSprung = 32;
#endif
							}
						}
						else if(stSuchFrei.veEintrag == _LetzterEintragDoppelSeite){
							if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								if(_VorherigerEintrag->vbAdresse){
									stSuchFrei.veEintrag->vbAdresse = nullptr;
									stSuchFrei.veEintrag->ulBytes += _ErsterEintragNachsteSeite->ulBytes;
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
									break;
								}
								else{
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
#ifndef _64bit
									_Tabelle->ulBelegt -= 8;
#else
									_Tabelle->ulBelegt -= 16;
#endif
								}
							}
							else if(_ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*stSuchFrei.veEintrag = *_ZweiterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else{
								*stSuchFrei.veEintrag = *_ZweiterEintragNachsteSeite;
								stSuchFrei.vsTabelle = _TabelleNachsteSeite;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
								stSuchFrei.ulScrollSprung = 16;
#else
								stSuchFrei.ulScrollSprung = 32;
#endif
							}
						}
						else if(stSuchFrei.veEintrag == _VorletzterEintragSeite){
							if(!_TabelleNachsteSeite){
								if(_VorherigerEintrag->vbAdresse){
									stSuchFrei.veEintrag->vbAdresse = nullptr;
									stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
#ifndef _64bit
									_Tabelle->ulBelegt -= 8;
#else
									_Tabelle->ulBelegt -= 16;
#endif
									break;
								}
								else{
#ifndef _64bit
									_Tabelle->ulBelegt -= 16;
#else
									_Tabelle->ulBelegt -= 32;
#endif
								}
							}
							else if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*stSuchFrei.veEintrag = *_ErsterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
#ifndef _64bit
								_Tabelle->ulBelegt -= 8;
#else
								_Tabelle->ulBelegt -= 16;
#endif
							}
							else if(_ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*stSuchFrei.veEintrag = *_ErsterEintragNachsteSeite;
								*_NachsterEintrag = *_ZweiterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else{
								*stSuchFrei.veEintrag = *_ErsterEintragNachsteSeite;
								*_NachsterEintrag = *_ZweiterEintragNachsteSeite;
								stSuchFrei.vsTabelle = _TabelleNachsteSeite;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
								stSuchFrei.ulScrollSprung = 16;
#else
								stSuchFrei.ulScrollSprung = 32;
#endif
							}
						}
						else{
							if(_NachsterEintrag == _LetzterEintrag){
								if(_VorherigerEintrag->vbAdresse){
									stSuchFrei.veEintrag->vbAdresse = nullptr;
									stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
#ifndef _64bit
									_Tabelle->ulBelegt -= 8;
#else
									_Tabelle->ulBelegt -= 16;
#endif
									break;
								}
								else{
#ifndef _64bit
									_Tabelle->ulBelegt -= 16;
#else
									_Tabelle->ulBelegt -= 32;
#endif
								}
							}
							else{
								stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
#ifndef _64bit
								stSuchFrei.ulScrollSprung = 16;
#else
								stSuchFrei.ulScrollSprung = 32;
#endif
							}
						}
					}
					else if(stSuchFrei.veEintrag->vbAdresse == _LetzterEintragInhaltsSeite){
						if(stSuchFrei.veEintrag == _ErsterEintrag){
							if(_NachsterEintrag == _LetzterEintrag){
								*_LetzterEintragVorherigeSeite = *_ZweiterEintrag;
								stSuchFrei.vsTabelle = _TabelleVorherigeSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else{
								*_LetzterEintragVorherigeSeite = *_ZweiterEintrag;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 16;
	#else
								stSuchFrei.ulScrollSprung = 32;
	#endif
							}
						}
						else if(stSuchFrei.veEintrag == _LetzterEintragDoppelSeite){
							if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_VorherigerEintrag = *_ErsterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
								_Tabelle->ulBelegt -= 8;
	#else
								_Tabelle->ulBelegt -= 16;
	#endif
							}
							else if(_ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_VorherigerEintrag = *_ErsterEintragNachsteSeite;
								*stSuchFrei.veEintrag = *_ZweiterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else{
								*_VorherigerEintrag = *_ErsterEintragNachsteSeite;
								*stSuchFrei.veEintrag = *_ZweiterEintragNachsteSeite;
								stSuchFrei.vsTabelle = _TabelleNachsteSeite;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 16;
	#else
								stSuchFrei.ulScrollSprung = 32;
	#endif
							}
						}
						else{
							stSuchFrei.veScrollEintrag = _VorherigerEintrag;
	#ifndef _64bit
							stSuchFrei.ulScrollSprung = 16;
	#else
							stSuchFrei.ulScrollSprung = 32;
	#endif	
						}
					}
					else{
						if(stSuchFrei.veEintrag == _ErsterEintrag){
							if(_NachsterEintrag == _LetzterEintrag){
								if(_VorLetzterEintragVorherigeSeite->vbAdresse){
									_LetzterEintragVorherigeSeite->ulBytes += stSuchFrei.veEintrag->ulBytes;
									_LetzterEintragVorherigeSeite->ulBytes += _ZweiterEintrag->ulBytes;
									stSuchFrei.vsTabelle = _TabelleVorherigeSeite;
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
									break;
								}
								else{
									stSuchFrei.vsTabelle = _TabelleVorherigeSeite;
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
									_Tabelle->ulBelegt -= 8;
	#else
									_Tabelle->ulBelegt -= 16;
	#endif
								}
							}
							else if(_DritterEintrag == _LetzterEintrag){
								*_LetzterEintragVorherigeSeite = *_DritterEintrag;
								stSuchFrei.vsTabelle = _TabelleVorherigeSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else{
								*_LetzterEintragVorherigeSeite = *_DritterEintrag;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 24;
	#else
								stSuchFrei.ulScrollSprung = 48;
	#endif
							}
						}
						else if(stSuchFrei.veEintrag == _ZweiterEintrag){
							if(_NachsterEintrag == _LetzterEintrag){
								if(_LetzterEintragVorherigeSeite->vbAdresse){
									_VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
									_VorherigerEintrag->ulBytes += _NachsterEintrag->ulBytes;
	#ifndef _64bit
									_Tabelle->ulBelegt -= 16;
	#else
									_Tabelle->ulBelegt -= 32;
	#endif
									break;
								}
								else{
									stSuchFrei.vsTabelle = _TabelleVorherigeSeite;
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
								}
							}
							else{
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 24;
	#else
								stSuchFrei.ulScrollSprung = 48;
	#endif
							}
						}
						else if(stSuchFrei.veEintrag == _LetzterEintragDoppelSeite){
							if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								if(_VorVorherigerEintrag->vbAdresse){
									_VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
									_VorherigerEintrag->ulBytes += _ErsterEintragNachsteSeite->ulBytes;
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
									_Tabelle->ulBelegt -= 8;
	#else
									_Tabelle->ulBelegt -= 16;
	#endif
									break;
								}
								else{
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
									_Tabelle->ulBelegt -= 16;
	#else
									_Tabelle->ulBelegt -= 32;
	#endif
								}
							}
							else if(_ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_VorherigerEintrag = *_ZweiterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
								_Tabelle->ulBelegt -= 8;
	#else
								_Tabelle->ulBelegt -= 16;
	#endif
							}
							else if(_DritterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_VorherigerEintrag = *_ZweiterEintragNachsteSeite;
								*stSuchFrei.veEintrag = *_DritterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else{
								*_VorherigerEintrag = *_ZweiterEintragNachsteSeite;
								*stSuchFrei.veEintrag = *_DritterEintragNachsteSeite;
								stSuchFrei.vsTabelle = _TabelleNachsteSeite;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 24;
	#else
								stSuchFrei.ulScrollSprung = 48;
	#endif
							}
						}
						else if(stSuchFrei.veEintrag == _VorletzterEintragSeite){
							if(!_TabelleNachsteSeite){
								if(_VorVorherigerEintrag->vbAdresse){
									_VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
									_VorherigerEintrag->ulBytes += _NachsterEintrag->ulBytes;
	#ifndef _64bit
									_Tabelle->ulBelegt -= 16;
	#else
									_Tabelle->ulBelegt -= 32;
	#endif
									break;
								}
								else{
	#ifndef _64bit
									_Tabelle->ulBelegt -= 24;
	#else
									_Tabelle->ulBelegt -= 48;
	#endif
								}
							}
							else if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_VorherigerEintrag = *_ErsterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
								_Tabelle->ulBelegt -= 16;
	#else
								_Tabelle->ulBelegt -= 32;
	#endif
							}
							else if(_ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_VorherigerEintrag = *_ErsterEintragNachsteSeite;
								*stSuchFrei.veEintrag = *_ZweiterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
								_Tabelle->ulBelegt -= 8;
	#else
								_Tabelle->ulBelegt -= 16;
	#endif
							}
							else if(_DritterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_VorherigerEintrag = *_ErsterEintragNachsteSeite;
								*stSuchFrei.veEintrag = *_ZweiterEintragNachsteSeite;
								*_NachsterEintrag = *_DritterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else{
								*_VorherigerEintrag = *_ErsterEintragNachsteSeite;
								*stSuchFrei.veEintrag = *_ZweiterEintragNachsteSeite;
								*_NachsterEintrag = *_DritterEintragNachsteSeite;
								stSuchFrei.vsTabelle = _TabelleNachsteSeite;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 24;
	#else
								stSuchFrei.ulScrollSprung = 48;
	#endif
							}
						}
						else if(stSuchFrei.veEintrag == _VorVorletzterEintragSeite){
							if(_NachsterEintrag == _LetzterEintrag){
								if(_VorVorherigerEintrag->vbAdresse){
									_VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
									_VorherigerEintrag->ulBytes += _NachsterEintrag->ulBytes;
	#ifndef _64bit
									_Tabelle->ulBelegt -= 16;
	#else
									_Tabelle->ulBelegt -= 32;
	#endif
									break;
								}
								else{
	#ifndef _64bit
									_Tabelle->ulBelegt -= 24;
	#else
									_Tabelle->ulBelegt -= 48;
	#endif
								}
							}
							else if(!_TabelleNachsteSeite){
								*_VorherigerEintrag = *_LetzterEintragSeite;
	#ifndef _64bit
								_Tabelle->ulBelegt -= 24;
	#else
								_Tabelle->ulBelegt -= 48;
	#endif
							}
							else if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_VorherigerEintrag = *_LetzterEintragSeite;
								*stSuchFrei.veEintrag = *_ErsterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
								_Tabelle->ulBelegt -= 16;
	#else
								_Tabelle->ulBelegt -= 32;
	#endif
							}
							else if(_ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_VorherigerEintrag = *_LetzterEintragSeite;
								*stSuchFrei.veEintrag = *_ErsterEintragNachsteSeite;
								*_NachsterEintrag = *_ZweiterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
								_Tabelle->ulBelegt -= 8;
	#else
								_Tabelle->ulBelegt -= 16;
	#endif
							}
							else if(_DritterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_VorherigerEintrag = *_LetzterEintragSeite;
								*stSuchFrei.veEintrag = *_ErsterEintragNachsteSeite;
								*_NachsterEintrag = *_ZweiterEintragNachsteSeite;
								*_LetzterEintragSeite = *_DritterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else{
								*_VorherigerEintrag = *_LetzterEintragSeite;
								*stSuchFrei.veEintrag = *_ErsterEintragNachsteSeite;
								*_NachsterEintrag = *_ZweiterEintragNachsteSeite;
								*_LetzterEintragSeite = *_DritterEintragNachsteSeite;
								stSuchFrei.vsTabelle = _TabelleNachsteSeite;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 24;
	#else
								stSuchFrei.ulScrollSprung = 48;
	#endif
							}
						}
						else{
							if(_NachsterEintrag == _LetzterEintrag){
								if(_VorVorherigerEintrag->vbAdresse){
									_VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
									_VorherigerEintrag->ulBytes += _NachsterEintrag->ulBytes;
	#ifndef _64bit
									_Tabelle->ulBelegt -= 16;
	#else
									_Tabelle->ulBelegt -= 32;
	#endif
									break;
								}
								else{
	#ifndef _64bit
									_Tabelle->ulBelegt -= 24;
	#else
									_Tabelle->ulBelegt -= 48;
	#endif
								}
							}
							else{
								stSuchFrei.veScrollEintrag = _VorherigerEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 24;
	#else
								stSuchFrei.ulScrollSprung = 48;
	#endif
							}
						}
					}
				}
				if(stSuchFrei.ulScrollSprung) VMFrei_ScrollEintrage(vmSpeicher, stSuchFrei);

				if(_Inhalt->vsNachste){
					((VMSEITENKOPF)_Inhalt->vsNachste)->vsVorherige = ((VMSEITENKOPF)_Inhalt)->vsVorherige;
					((VMSEITENKOPF)_Inhalt->vsVorherige)->vsNachste = ((VMSEITENKOPF)_Inhalt)->vsNachste;
				}
				else ((VMSEITENKOPF)_Inhalt->vsVorherige)->vsNachste = nullptr;

				if(stSuchFrei.vsInhalt == _vmSpeicher->vsLetzteInhaltKlein)	_vmSpeicher->vsLetzteInhaltKlein = ((VMSEITENKOPF)_Inhalt)->vsVorherige;
				VirtualFree(stSuchFrei.vsInhalt, 0, MEM_RELEASE);

				LeaveCriticalSection(&_vmSpeicher->csTabelle);
				return;

			}
			else{
				if(stSuchFrei.veEintrag->vbAdresse == _ErsterEintragInhaltsSeite){
					if(stSuchFrei.veEintrag == _ErsterEintrag){
						stSuchFrei.veEintrag->vbAdresse = nullptr;
						if(_ZweiterEintrag == _LetzterEintrag){
							stSuchFrei.veEintrag->ulBytes += _ZweiterEintrag->ulBytes;
	#ifndef _64bit
							_Tabelle->ulBelegt -= 8;
	#else
							_Tabelle->ulBelegt -= 16;
	#endif
						}
						else if(!_ZweiterEintrag->vbAdresse){
							stSuchFrei.veEintrag->ulBytes += _ZweiterEintrag->ulBytes;
							stSuchFrei.veScrollEintrag = _ZweiterEintrag;
#ifndef _64bit
							stSuchFrei.ulScrollSprung = 8;
#else
							stSuchFrei.ulScrollSprung = 16;
	#endif
						}
					}
					else if(stSuchFrei.veEintrag == _LetzterEintragDoppelSeite){
						stSuchFrei.veEintrag->vbAdresse = nullptr;
						if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
							stSuchFrei.veEintrag->ulBytes += _ErsterEintragNachsteSeite->ulBytes;
							VMFrei_Tabelle(vmSpeicher, stSuchFrei);
						}
						else if(!_ErsterEintragNachsteSeite->vbAdresse){
							stSuchFrei.veEintrag->ulBytes += _ErsterEintragNachsteSeite->ulBytes;
							stSuchFrei.vsTabelle = _TabelleNachsteSeite;
							stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
							stSuchFrei.ulScrollSprung = 8;
#else
							stSuchFrei.ulScrollSprung = 16;
#endif
						}
					}
					else if(stSuchFrei.veEintrag == _VorletzterEintragSeite){
						stSuchFrei.veEintrag->vbAdresse = nullptr;
						if(!_TabelleNachsteSeite){
							stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
	#ifndef _64bit
							_Tabelle->ulBelegt -= 8;
	#else
							_Tabelle->ulBelegt -= 16;
	#endif
						}
						else if(!_NachsterEintrag->vbAdresse){
							stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
							*_NachsterEintrag = *_ErsterEintragNachsteSeite;
							if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else{
								stSuchFrei.vsTabelle = _TabelleNachsteSeite;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 8;
	#else
								stSuchFrei.ulScrollSprung = 16;
	#endif
							}
						}
					}
					else{
						stSuchFrei.veEintrag->vbAdresse = nullptr;
						if(_NachsterEintrag == _LetzterEintrag){
							stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
	#ifndef _64bit
							_Tabelle->ulBelegt -= 8;
	#else
							_Tabelle->ulBelegt -= 16;
	#endif
						}
						else if(!_NachsterEintrag->vbAdresse){
							stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
							stSuchFrei.veScrollEintrag = _NachsterEintrag;
	#ifndef _64bit
							stSuchFrei.ulScrollSprung = 8;
	#else
							stSuchFrei.ulScrollSprung = 16;
	#endif
						}
					}
				}
				else if(stSuchFrei.veEintrag->vbAdresse == _LetzterEintragInhaltsSeite){
					if(stSuchFrei.veEintrag == _ErsterEintrag){
						if(!_LetzterEintragVorherigeSeite->vbAdresse){
							_LetzterEintragVorherigeSeite->ulBytes += stSuchFrei.veEintrag->ulBytes;
							stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
#ifndef _64bit
							stSuchFrei.ulScrollSprung = 8;
#else
							stSuchFrei.ulScrollSprung = 16;
#endif					
						}
						else stSuchFrei.veEintrag->vbAdresse = nullptr;
					}
					else if(stSuchFrei.veEintrag == _LetzterEintragDoppelSeite){
						if(!_VorherigerEintrag->vbAdresse){
							_VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
							if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								*_LetzterEintragSeite = *_ErsterEintragNachsteSeite;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else{
								*_LetzterEintragSeite = *_ErsterEintragNachsteSeite;
								stSuchFrei.vsTabelle = _TabelleNachsteSeite;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
								stSuchFrei.ulScrollSprung = 8;
#else
								stSuchFrei.ulScrollSprung = 16;
#endif
							}
						}
						else stSuchFrei.veEintrag->vbAdresse = nullptr;
					}
					else{
						if(!_VorherigerEintrag->vbAdresse){
							_VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
							stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
#ifndef _64bit
							stSuchFrei.ulScrollSprung = 8;
#else
							stSuchFrei.ulScrollSprung = 16;
#endif
						}
						else stSuchFrei.veEintrag->vbAdresse = nullptr;
					}
				}
				else{
					if(stSuchFrei.veEintrag == _ErsterEintrag){
						if(!_LetzterEintragVorherigeSeite->vbAdresse){
							_LetzterEintragVorherigeSeite->ulBytes += stSuchFrei.veEintrag->ulBytes;
							if(_ZweiterEintrag == _LetzterEintrag){
								_LetzterEintragVorherigeSeite->ulBytes += _ZweiterEintrag->ulBytes;
								stSuchFrei.vsTabelle = _Tabelle->vsVorherige;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else if(!_ZweiterEintrag->vbAdresse){
								_LetzterEintragVorherigeSeite->ulBytes += _ZweiterEintrag->ulBytes;
								stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 16;
	#else
								stSuchFrei.ulScrollSprung = 32;
	#endif
							}
							else{
								stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 8;
	#else
								stSuchFrei.ulScrollSprung = 16;
	#endif
							}
						}
						else{
							stSuchFrei.veEintrag->vbAdresse = nullptr;
							if(_ZweiterEintrag == _LetzterEintrag){
								stSuchFrei.veEintrag->ulBytes += _ZweiterEintrag->ulBytes;
	#ifndef _64bit
								_Tabelle->ulBelegt -= 8;
	#else
								_Tabelle->ulBelegt -= 16;
	#endif
							}
							else if(!_ZweiterEintrag->vbAdresse){
								stSuchFrei.veEintrag->ulBytes += _ZweiterEintrag->ulBytes;
								stSuchFrei.veScrollEintrag = _ZweiterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 8;
	#else
								stSuchFrei.ulScrollSprung = 16;
	#endif							
							}
						}
					}
					else if(stSuchFrei.veEintrag == _LetzterEintragDoppelSeite){
						if(!_VorherigerEintrag->vbAdresse){
							_VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
							if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								_VorherigerEintrag->ulBytes += _ErsterEintragNachsteSeite->ulBytes;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
								_Tabelle->ulBelegt -= 8;
	#else
								_Tabelle->ulBelegt -= 16;
	#endif
							}
							else if(!_ErsterEintragNachsteSeite->vbAdresse){
								_VorherigerEintrag->ulBytes += _ErsterEintragNachsteSeite->ulBytes;
								*stSuchFrei.veEintrag = *_ZweiterEintragNachsteSeite;
								if(_ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
								}
								else{
									stSuchFrei.vsTabelle = _TabelleNachsteSeite;
									stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
									stSuchFrei.ulScrollSprung = 16;
	#else
									stSuchFrei.ulScrollSprung = 32;
	#endif
								}
							}
							else{
								*stSuchFrei.veEintrag = *_ErsterEintragNachsteSeite;
								stSuchFrei.vsTabelle = _TabelleNachsteSeite;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 8;
	#else
								stSuchFrei.ulScrollSprung = 16;
	#endif
							}
						}
						else{
							stSuchFrei.veEintrag->vbAdresse = nullptr;
							if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
								stSuchFrei.veEintrag->ulBytes += _ErsterEintragNachsteSeite->ulBytes;
								VMFrei_Tabelle(vmSpeicher, stSuchFrei);
							}
							else if(!_ErsterEintragNachsteSeite->vbAdresse){
								stSuchFrei.veEintrag->ulBytes += _ErsterEintragNachsteSeite->ulBytes;
								stSuchFrei.vsTabelle = _TabelleNachsteSeite;
								stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 8;
	#else
								stSuchFrei.ulScrollSprung = 16;
	#endif
							}
						}
					}
					else if(stSuchFrei.veEintrag == _VorletzterEintragSeite){
						if(!_VorherigerEintrag->vbAdresse){
							_VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
							if(!_TabelleNachsteSeite){
								_VorherigerEintrag->ulBytes += _NachsterEintrag->ulBytes;
	#ifndef _64bit
								_Tabelle->ulBelegt -= 16;
	#else
								_Tabelle->ulBelegt -= 32;
	#endif
							}
							else if(!_NachsterEintrag->vbAdresse){
								_VorherigerEintrag->ulBytes += _NachsterEintrag->ulBytes;
								*stSuchFrei.veEintrag = *_ErsterEintragNachsteSeite;
								if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
	#ifndef _64bit
									_Tabelle->ulBelegt -= 8;
	#else
									_Tabelle->ulBelegt -= 16;
	#endif
								}
								else if(_ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
									*_LetzterEintragSeite = *_ZweiterEintragNachsteSeite;
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
								}
								else{
									*_LetzterEintragSeite = *_ZweiterEintragNachsteSeite;
									stSuchFrei.vsTabelle = _TabelleNachsteSeite;
									stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
									stSuchFrei.ulScrollSprung = 16;
	#else
									stSuchFrei.ulScrollSprung = 32;
	#endif
								}
							}
							else{
								stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 8;
	#else
								stSuchFrei.ulScrollSprung = 16;
	#endif
							}
						}
						else{
							stSuchFrei.veEintrag->vbAdresse = nullptr;
							if(!_TabelleNachsteSeite){
								stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
	#ifndef _64bit
								_Tabelle->ulBelegt -= 8;
	#else
								_Tabelle->ulBelegt -= 16;
	#endif
							}
							else if(!_NachsterEintrag->vbAdresse){
								stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
								*_LetzterEintragSeite = *_ErsterEintragNachsteSeite;
								if(_ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
									VMFrei_Tabelle(vmSpeicher, stSuchFrei);
								}
								else{
									stSuchFrei.vsTabelle = _TabelleNachsteSeite;
									stSuchFrei.veScrollEintrag = _ErsterEintrag;
	#ifndef _64bit
									stSuchFrei.ulScrollSprung = 8;
	#else
									stSuchFrei.ulScrollSprung = 16;
	#endif
								}
							}
						}
					}
					else{
						if(!_VorherigerEintrag->vbAdresse){
							_VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
							if(_NachsterEintrag == _LetzterEintrag){
								_VorherigerEintrag->ulBytes += _NachsterEintrag->ulBytes;
	#ifndef _64bit
								_Tabelle->ulBelegt -= 16;
	#else
								_Tabelle->ulBelegt -= 32;
	#endif
							}
							else if(!_NachsterEintrag->vbAdresse){
								_VorherigerEintrag->ulBytes += _NachsterEintrag->ulBytes;
								stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 16;
	#else
								stSuchFrei.ulScrollSprung = 32;
	#endif
							}
							else{
								stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 8;
	#else
								stSuchFrei.ulScrollSprung = 16;
	#endif
							}
						}
						else{
							stSuchFrei.veEintrag->vbAdresse = nullptr;
							if(_NachsterEintrag == _LetzterEintrag){
								stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
	#ifndef _64bit
								_Tabelle->ulBelegt -= 8;
	#else
								_Tabelle->ulBelegt -= 16;
	#endif
							}
							else if(!_NachsterEintrag->vbAdresse){
								stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
								stSuchFrei.veScrollEintrag = _NachsterEintrag;
	#ifndef _64bit
								stSuchFrei.ulScrollSprung = 8;
	#else
								stSuchFrei.ulScrollSprung = 16;
	#endif
							}
						}
					}
				}
				if(stSuchFrei.ulScrollSprung) VMFrei_ScrollEintrage(vmSpeicher, stSuchFrei);

				LeaveCriticalSection(&_vmSpeicher->csTabelle);
				return;
			}
		}
		stSuchFrei.vsInhalt = _Inhalt->vsVorherige;
	}
	while(stSuchFrei.vsInhalt);
	}
//---------------------------------------------------------------------------
void __vectorcall VMFrei(_In_ VMEMORY vmSpeicher, _In_ void* pvAdresse)
{
	if(!pvAdresse) return;
	if(!vmSpeicher) vmSpeicher = vmStandard;
	EnterCriticalSection(&_vmSpeicher->csTabelle);

	STSuchFrei stSuchFrei;
	stSuchFrei.vsInhalt = ((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste;
	void* pvMapped; void* pvLoschen;
	while(stSuchFrei.vsInhalt){
#ifndef _64bit
		if(pvAdresse == (VMBLOCK)&stSuchFrei.vsInhalt[16]){
#else
		if(pvAdresse == (VMBLOCK)&stSuchFrei.vsInhalt[32]){
#endif
			((VMSEITENKOPF)_Inhalt->vsVorherige)->vsNachste = _Inhalt->vsNachste;
			if(_Inhalt->vsNachste) ((VMSEITENKOPF)_Inhalt->vsNachste)->vsVorherige = _Inhalt->vsVorherige;
			else if(_Inhalt->vsVorherige == _vmSpeicher->vsLetzteInhaltKlein) _vmSpeicher->vsLetzteInhaltGross = nullptr;
			else _vmSpeicher->vsLetzteInhaltGross = _Inhalt->vsVorherige;
			VirtualFree(stSuchFrei.vsInhalt, 0, MEM_RELEASE);

			LeaveCriticalSection(&_vmSpeicher->csTabelle);
			return;
		}
		stSuchFrei.vsInhalt = _Inhalt->vsNachste;
	}

	stSuchFrei.vsTabelle = _vmSpeicher->vsErsteTabelle;
	stSuchFrei.veEintrag = _ErsterEintrag;
	stSuchFrei.veLetzterEintrag = _LetzterEintrag;
	stSuchFrei.veLetzterEintragDoppelSeite = _LetzterEintragDoppelSeite;
	stSuchFrei.ulScrollSprung = 0;
	stSuchFrei.veScrollEintrag = nullptr;
	stSuchFrei.ucAdresse = 0;

	while(stSuchFrei.veEintrag->vbAdresse != pvAdresse){
		if(stSuchFrei.veEintrag == stSuchFrei.veLetzterEintragDoppelSeite){
			stSuchFrei.vsTabelle = _TabelleNachsteSeite;
			if(!stSuchFrei.vsTabelle){
				LeaveCriticalSection(&_vmSpeicher->csTabelle); return; }
			stSuchFrei.veEintrag = _ErsterEintrag;
			stSuchFrei.veLetzterEintrag = _LetzterEintrag;
			stSuchFrei.veLetzterEintragDoppelSeite = _LetzterEintragDoppelSeite;
		}
		else if(stSuchFrei.veEintrag == stSuchFrei.veLetzterEintrag){ 
			LeaveCriticalSection(&_vmSpeicher->csTabelle); return; }
		else stSuchFrei.veEintrag++;
	}
	VMFrei(vmSpeicher, stSuchFrei);
}
//---------------------------------------------------------------------------
VMBLOCKA __vectorcall VMBlockA(_In_ VMEMORY vmSpeicher, _In_ unsigned long ulBytes, _In_ unsigned char ucAlign)
{
	return (VMBLOCKA)::VMBlock(vmSpeicher, ulBytes, ucAlign);
}
//---------------------------------------------------------------------------
VMBLOCKA __vectorcall VMBlockA(_In_ unsigned long ulBytes, _In_ unsigned char ucAlign)
{
	return (VMBLOCKA)::VMBlock(vmStandard, ulBytes, ucAlign);
}
//---------------------------------------------------------------------------
VMBLOCK __vectorcall VMBlock(_In_ VMEMORY vmSpeicher, _In_ unsigned long ulBytes)
{
	return ::VMBlock(vmSpeicher, ulBytes, 0);
}
//---------------------------------------------------------------------------
VMBLOCK __vectorcall VMBlock(_In_ unsigned long ulBytes)
{
	return ::VMBlock(vmStandard, ulBytes, 0);
}
//---------------------------------------------------------------------------
VMBLOCK __vectorcall VMBlockS(_In_ unsigned long ulBytes)
{
	return ::VMBlockS(vmStandard, ulBytes, 0);
}
//---------------------------------------------------------------------------
VMBLOCK __vectorcall VMBlockS(_In_ VMEMORY vmSpeicher, _In_ unsigned long ulBytes)
{
	return ::VMBlockS(vmSpeicher, ulBytes, 0);
}
//---------------------------------------------------------------------------
void __vectorcall VMFrei(_In_ void* pvAdresse)
{
	VMFrei(vmStandard, pvAdresse);
}
//---------------------------------------------------------------------------
void __vectorcall VMFreiS(_In_ void* pvAdresse)
{
	VMFreiS(vmStandard, pvAdresse);
}
//---------------------------------------------------------------------------
void __vectorcall VMFreiS(_In_ VMEMORY vmSpeicher, _In_ void* pvAdresse)
{
	if(!pvAdresse) return;
	if(!vmSpeicher) vmSpeicher = vmStandard;
	EnterCriticalSection(&_vmSpeicher->csFrei);
#ifndef _64bit
	if(_vmSpeicher->vfBlock == (VMFREI)&_vmSpeicher->vsFrei[dwDoppelSpeicherSeite - 4]) VMFrei(vmSpeicher, pvAdresse);
#else
	if(_vmSpeicher->vfBlock == (VMFREI)&_vmSpeicher->vsFrei[dwDoppelSpeicherSeite - 8]) VMFrei(vmSpeicher, pvAdresse);
#endif
	else{
		*_vmSpeicher->vfBlock = pvAdresse;
		_vmSpeicher->vfBlock++;
		SetEvent(_vmSpeicher->thvmFrei.heFertig);
	}
	LeaveCriticalSection(&_vmSpeicher->csFrei);
}
//---------------------------------------------------------------------------