/****************************************************************************
  Memory.cpp
  For more information see https://github.com/RePag-net/Memory
****************************************************************************/

/****************************************************************************
  The MIT License(MIT)

  Copyright(c) 2021 René Pagel

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
VMEMORY vmStandard = nullptr;
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
#define _LetzterEintrag (VMEINTRAG)&stSuchFrei.vsTabelle[((VMSEITENKOPF)stSuchFrei.vsTabelle)->ulBelegt - 8]
#define _LetzterEintragDoppelSeite (VMEINTRAG)&stSuchFrei.vsTabelle[dwDoppelSpeicherSeite - 8]
#define _LetzterEintragNachsteSeite ((VMEINTRAG)((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste + (((VMSEITENKOPF)((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste)->ulBelegt - 8) / 8)
#define _Tabelle ((VMSEITENKOPF)stSuchFrei.vsTabelle)
#define _Inhalt ((VMSEITENKOPF)stSuchFrei.vsInhalt)
#define _ErsterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[16])
#define _ZweiterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[24])
#define _DritterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[32])
#define _NachsterEintrag (stSuchFrei.veEintrag + 1)
#define _VorherigerEintrag (stSuchFrei.veEintrag - 1)
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
#define _LetzterEintrag (VMEINTRAG)&stSuchFrei.vsTabelle[((VMSEITENKOPF)stSuchFrei.vsTabelle)->ulBelegt - 16]
#define _LetzterEintragDoppelSeite (VMEINTRAG)&stSuchFrei.vsTabelle[dwDoppelSpeicherSeite - 16]
#define _LetzterEintragNachsteSeite ((VMEINTRAG)((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste + (((VMSEITENKOPF)((VMSEITENKOPF)stSuchFrei.vsTabelle)->vsNachste)->ulBelegt - 16) / 16)
#define _Tabelle ((VMSEITENKOPF)stSuchFrei.vsTabelle)
#define _Inhalt ((VMSEITENKOPF)stSuchFrei.vsInhalt)
#define _ErsterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[32])
#define _ZweiterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[48])
#define _DritterEintrag ((VMEINTRAG)&stSuchFrei.vsTabelle[64])
#define _NachsterEintrag (stSuchFrei.veEintrag + 1)
#define _VorherigerEintrag (stSuchFrei.veEintrag - 1)
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
 catch(EXCEPTION exFehler){ throw exFehler; return nullptr; }
}
//---------------------------------------------------------------------------
VMEMORY __vectorcall InitVirtualMem(_In_ bool bAuslagern)
{
  VMSEITE vsTabelle = (VMSEITE)VirtualAlloc(nullptr, dwDoppelSpeicherSeite, MEM_RESERVE, PAGE_NOACCESS);
  if(vsTabelle == NULL) throw exFehler_KeinSpeicher;
  VirtualAlloc(vsTabelle, dwEineSpeicherSeite, MEM_COMMIT, PAGE_READWRITE);
  VirtualLock(vsTabelle, dwEineSpeicherSeite);

  VMSEITE vsInhalt;
  vsInhalt = (VMSEITE)VirtualAlloc(NULL, dwDoppelSpeicherSeite, MEM_RESERVE, PAGE_NOACCESS);
  if(vsInhalt == nullptr) throw exFehler_KeinSpeicher;
  VirtualAlloc(vsInhalt, dwEineSpeicherSeite, MEM_COMMIT, PAGE_READWRITE);
#ifndef _64bit
   ((VMSPEICHER)&vsInhalt[16])->bMapped = false;
#else
	 ((VMSPEICHER)&vsInhalt[32])->bMapped = false;
#endif
 VirtualLock(vsInhalt, dwEineSpeicherSeite);

 VMSEITE vsFrei = (VMSEITE)VirtualAlloc(NULL, dwEineSpeicherSeite, MEM_COMMIT, PAGE_READWRITE);
 if(vsFrei == nullptr) throw exFehler_KeinSpeicher;
 VirtualLock(vsFrei, dwEineSpeicherSeite);

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
 ((VMSPEICHER)&vsInhalt[16])->vsReserveTabelle = nullptr;
 ((VMSPEICHER)&vsInhalt[16])->vsReserveInhalt = nullptr;
#else
 ((VMSPEICHER)&vsInhalt[32])->bAuslagern = bAuslagern;
 ((VMSPEICHER)&vsInhalt[32])->vsReserveTabelle = nullptr;
 ((VMSPEICHER)&vsInhalt[32])->vsReserveInhalt = nullptr;
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
 ((VMSEITENKOPF)vsInhalt)->ulReserve = dwEineSpeicherSeite;
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

 ((VMEINTRAG)&vsTabelle[24])->ulBytes = dwEineSpeicherSeite - 16 - sizeof(STVirtualSpeicher);
 ((VMEINTRAG)&vsTabelle[24])->vbAdresse = nullptr;

 InitializeCriticalSectionAndSpinCount(&((VMSPEICHER)&vsInhalt[16])->csTabelle, 4000);
 InitializeCriticalSectionAndSpinCount(&((VMSPEICHER)&vsInhalt[16])->csFrei, 4000);

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

 ((VMEINTRAG)&vsTabelle[48])->ulBytes = dwEineSpeicherSeite - 32 - sizeof(STVirtualSpeicher);
 ((VMEINTRAG)&vsTabelle[48])->vbAdresse = nullptr;

 InitializeCriticalSectionAndSpinCount(&((VMSPEICHER)&vsInhalt[32])->csTabelle, 4000);
 InitializeCriticalSectionAndSpinCount(&((VMSPEICHER)&vsInhalt[32])->csFrei, 4000);

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

 VMSEITE vsInhalt = _vmSpeicher->vsLetzteInhaltKlein;
 while(((VMSEITENKOPF)vsInhalt)->vsNachste){ vsInhalt = ((VMSEITENKOPF)vsInhalt)->vsNachste; }
 while(((VMSEITENKOPF)vsInhalt)->vsVorherige && vsInhalt){
   vsLoschen = vsInhalt;
   vsInhalt = ((VMSEITENKOPF)vsLoschen)->vsVorherige;
   VirtualFree(vsLoschen, 0, MEM_RELEASE);
 }

 if(_vmSpeicher->vsReserveTabelle) VirtualFree(_vmSpeicher->vsReserveTabelle, 0, MEM_RELEASE);
 if(_vmSpeicher->vsReserveInhalt && !_vmSpeicher->bMapped) VirtualFree(_vmSpeicher->vsReserveInhalt, 0, MEM_RELEASE);

 DeleteCriticalSection(&_vmSpeicher->csTabelle);

 vsLoschen = vsInhalt;
 VirtualFree(vsLoschen, 0, MEM_RELEASE);
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
  ((VMSEITENKOPF)vsNeueInhalt)->vsNachste = NULL;
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
INLINE void __vectorcall VMBlock_ReserveTabelle_Aktivieren(VMEMORY vmSpeicher, STSuchBlock& stSuchBlock)
{
  VirtualAlloc(_vmSpeicher->vsReserveTabelle, dwEineSpeicherSeite, MEM_COMMIT, PAGE_READWRITE);
  VirtualLock(_vmSpeicher->vsReserveTabelle, dwEineSpeicherSeite);
#ifndef _64bit
  ((VMSEITENKOPF)_vmSpeicher->vsReserveTabelle)->ulBelegt = 16;
#else
	((VMSEITENKOPF)_vmSpeicher->vsReserveTabelle)->ulBelegt = 32;
#endif
  ((VMSEITENKOPF)_vmSpeicher->vsReserveTabelle)->ulReserve = 0;
  ((VMSEITENKOPF)_vmSpeicher->vsReserveTabelle)->vsNachste = nullptr;
  ((VMSEITENKOPF)_vmSpeicher->vsReserveTabelle)->vsVorherige = stSuchBlock.vsTabelle;
  ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste = _vmSpeicher->vsReserveTabelle;
  _vmSpeicher->vsLetzteTabelle = _vmSpeicher->vsReserveTabelle;
  _vmSpeicher->vsReserveTabelle = nullptr;
}
//---------------------------------------------------------------------------
INLINE void __vectorcall VMBlock_ReserveTabelle_Neu(VMEMORY vmSpeicher, STSuchBlock& stSuchBlock)
{
  VirtualAlloc(stSuchBlock.vsTabelle + dwEineSpeicherSeite, dwEineSpeicherSeite, MEM_COMMIT, PAGE_READWRITE);
  VirtualLock(stSuchBlock.vsTabelle + dwEineSpeicherSeite, dwEineSpeicherSeite);
  ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulReserve = 0;
  if(!_vmSpeicher->vsReserveTabelle) _vmSpeicher->vsReserveTabelle = (VMSEITE)VirtualAlloc(NULL, dwDoppelSpeicherSeite, MEM_RESERVE, PAGE_NOACCESS);
}
//---------------------------------------------------------------------------
INLINE void __vectorcall NeueTabelleSeite(_In_ VMEMORY vmSpeicher, _In_ STSuchBlock& stSuchBlock)
{
  VMSEITE vsSeite_Alt = stSuchBlock.vsTabelle;
  stSuchBlock.vsTabelle = _vmSpeicher->vsLetzteTabelle;

  if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
    VMBlock_ReserveTabelle_Aktivieren(vmSpeicher, stSuchBlock);

#ifndef _64bit
    ((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[16])->ulBytes = ((VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 8])->ulBytes;
    ((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[16])->vbAdresse = nullptr;
    ((VMSEITENKOPF)((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste)->ulBelegt += 8;
#else
		((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[32])->ulBytes = ((VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 16])->ulBytes;
		((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste[32])->vbAdresse = nullptr;
		((VMSEITENKOPF)((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste)->ulBelegt += 16;
#endif
  }
  else if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwEineSpeicherSeite) VMBlock_ReserveTabelle_Neu(vmSpeicher, stSuchBlock);

  VMBlock_ScrollEintrage_VonEnde(stSuchBlock, vsSeite_Alt);
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
    else if(!stSuchBlock.veVorheriger->vbAdresse) vbAdresse = (VMBLOCK)&stSuchBlock.vsInhalt[dwEineSpeicherSeite];
    else vbAdresse = stSuchBlock.veVorheriger->vbAdresse + stSuchBlock.veVorheriger->ulBytes;

#ifndef _64bit
    if(stSuchBlock.ucAlign && (ULONG)vbAdresse % stSuchBlock.ucAlign) return nullptr;
#else 
		if(stSuchBlock.ucAlign && (unsigned long long)vbAdresse % stSuchBlock.ucAlign) return nullptr;
#endif

    if(LetzterEintrag(stSuchBlock)){
      if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
        if(!((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste){
          VMBlock_ReserveTabelle_Aktivieren(vmSpeicher, stSuchBlock);
          stSuchBlock.vsTabelle = ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste;
#ifndef _64bit
          ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->ulBytes = 0;
          ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->vbAdresse = nullptr;
          ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
					((VMEINTRAG)&stSuchBlock.vsTabelle[32])->ulBytes = 0;
					((VMEINTRAG)&stSuchBlock.vsTabelle[32])->vbAdresse = nullptr;
					((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
        }
      }
      else{
        if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwEineSpeicherSeite) VMBlock_ReserveTabelle_Neu(vmSpeicher, stSuchBlock);

        stSuchBlock.veEintrag++;
        stSuchBlock.veEintrag->vbAdresse = nullptr;
        stSuchBlock.veEintrag->ulBytes = 0;
        stSuchBlock.veEintrag--;
#ifndef _64bit
        ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
				((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
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
    else if(!stSuchBlock.veVorheriger->vbAdresse) vbAdresse = (VMBLOCK)&stSuchBlock.vsInhalt[dwEineSpeicherSeite];
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
      if(ulBytes_Align < stSuchBlock.ulBytes) return NULL;
      else if(ulBytes_Align == stSuchBlock.ulBytes) stSuchBlock.ucAlign = 0;
      else if(ulBytes_Align == stSuchBlock.veEintrag->ulBytes) stSuchBlock.ucAlign = 0;
    }

    if(!LetzterEintrag(stSuchBlock)){
    Align_1:
      NeueTabelleSeite(vmSpeicher, stSuchBlock);
      VMBlock_ScrollEintrage_Einfugen(stSuchBlock);

      if(stSuchBlock.ucAlign){
        stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
        stSuchBlock.veEintrag++;
        stSuchBlock.veEintrag->ulBytes = ulBytes_Align;
        stSuchBlock.ucAlign = 0;
        goto Align_1;
      }
    }
    else{
      if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
        if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste){
          NeueTabelleSeite(vmSpeicher, stSuchBlock);
          stSuchBlock.vsTabelle = ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste;
          if(stSuchBlock.ucAlign){
            stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
#ifndef _64bit
            stSuchBlock.veEintrag = ((VMEINTRAG)&stSuchBlock.vsTabelle[16]);
            ((VMEINTRAG)&stSuchBlock.vsTabelle[24])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
            ((VMEINTRAG)&stSuchBlock.vsTabelle[24])->vbAdresse = nullptr;
						((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;// 8;
#else
						stSuchBlock.veEintrag = ((VMEINTRAG)&stSuchBlock.vsTabelle[32]);
						((VMEINTRAG)&stSuchBlock.vsTabelle[48])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
						((VMEINTRAG)&stSuchBlock.vsTabelle[48])->vbAdresse = nullptr;
						((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 32;
#endif
          }
          else{
#ifndef _64bit
            ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->ulBytes = ulBytes_Eintrag - stSuchBlock.ulBytes;
            ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->vbAdresse = nullptr;
#else
						((VMEINTRAG)&stSuchBlock.vsTabelle[32])->ulBytes = ulBytes_Eintrag - stSuchBlock.ulBytes;
						((VMEINTRAG)&stSuchBlock.vsTabelle[32])->vbAdresse = nullptr;
#endif
          }
        }
        else{ VMBlock_ReserveTabelle_Aktivieren(vmSpeicher, stSuchBlock);
          stSuchBlock.vsTabelle = ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste;
          if(stSuchBlock.ucAlign){
            stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
#ifndef _64bit
            stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[16];
            ((VMEINTRAG)&stSuchBlock.vsTabelle[24])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
            ((VMEINTRAG)&stSuchBlock.vsTabelle[24])->vbAdresse = nullptr;
            ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#else
						stSuchBlock.veEintrag = (VMEINTRAG)& stSuchBlock.vsTabelle[32];
						((VMEINTRAG)&stSuchBlock.vsTabelle[48])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
						((VMEINTRAG)&stSuchBlock.vsTabelle[48])->vbAdresse = nullptr;
						((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 32;
#endif
          }
          else{
#ifndef _64bit
            ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->ulBytes = ulBytes_Eintrag - stSuchBlock.ulBytes;
            ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->vbAdresse = nullptr;
            ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
						((VMEINTRAG)&stSuchBlock.vsTabelle[32])->ulBytes = ulBytes_Eintrag - stSuchBlock.ulBytes;
						((VMEINTRAG)&stSuchBlock.vsTabelle[32])->vbAdresse = nullptr;
						((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
          }
        }
      }
      else{
        if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwEineSpeicherSeite) 
          VMBlock_ReserveTabelle_Neu(vmSpeicher, stSuchBlock);
        if(stSuchBlock.ucAlign){
          stSuchBlock.veEintrag->ulBytes -= ulBytes_Align;
          stSuchBlock.veEintrag++;
#ifndef _64bit
          ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
					((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif

          if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite){
            if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste){
              NeueTabelleSeite(vmSpeicher, stSuchBlock);
              stSuchBlock.vsTabelle = ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste;

#ifndef _64bit
              ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
              ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->vbAdresse = nullptr;
#else
							((VMEINTRAG)&stSuchBlock.vsTabelle[32])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
							((VMEINTRAG)&stSuchBlock.vsTabelle[32])->vbAdresse = nullptr;
#endif
            }
            else{ VMBlock_ReserveTabelle_Aktivieren(vmSpeicher, stSuchBlock);
              stSuchBlock.vsTabelle = ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste;

#ifndef _64bit
              ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
              ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->vbAdresse = nullptr;
              ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
							((VMEINTRAG)&stSuchBlock.vsTabelle[32])->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
							((VMEINTRAG)&stSuchBlock.vsTabelle[32])->vbAdresse = nullptr;
							((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
            }
          }
          else{
            if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwEineSpeicherSeite) VMBlock_ReserveTabelle_Neu(vmSpeicher, stSuchBlock);
            stSuchBlock.veEintrag++;
            stSuchBlock.veEintrag->vbAdresse = nullptr;
            stSuchBlock.veEintrag->ulBytes = ulBytes_Align - stSuchBlock.ulBytes;
            stSuchBlock.veEintrag--;
#ifndef _64bit
            ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
						((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
          }
        }
        else{
          stSuchBlock.veEintrag++;
          stSuchBlock.veEintrag->vbAdresse = nullptr;
          stSuchBlock.veEintrag->ulBytes = ulBytes_Eintrag - stSuchBlock.ulBytes;
          stSuchBlock.veEintrag--;
#ifndef _64bit
          ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
					((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
        }
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
INLINE void NeueInhaltSeite(_In_ VMEMORY vmSpeicher, _In_ STSuchBlock& stSuchBlock)
{
  if(!_vmSpeicher->vsReserveInhalt){
    VirtualAlloc(stSuchBlock.vsInhalt + dwEineSpeicherSeite, dwEineSpeicherSeite, MEM_COMMIT, PAGE_READWRITE);
    if(!_vmSpeicher->bAuslagern) VirtualLock(stSuchBlock.vsInhalt + dwEineSpeicherSeite, dwEineSpeicherSeite);
    ((VMSEITENKOPF)stSuchBlock.vsInhalt)->ulReserve = 0;
    stSuchBlock.veEintrag->ulBytes += dwEineSpeicherSeite;

    if(!_vmSpeicher->bMapped){
      if(!(_vmSpeicher->vsReserveInhalt = (VMSEITE)VirtualAlloc(NULL, dwDoppelSpeicherSeite, MEM_RESERVE, PAGE_NOACCESS))){ LeaveCriticalSection(&_vmSpeicher->csTabelle); throw exFehler_KeinSpeicher; }
    }
  }
  else{
    VirtualAlloc(_vmSpeicher->vsReserveInhalt, dwEineSpeicherSeite, MEM_COMMIT, PAGE_READWRITE);
    if(!_vmSpeicher->bAuslagern) VirtualLock(_vmSpeicher->vsReserveInhalt, dwEineSpeicherSeite);
    ((VMSEITENKOPF)_vmSpeicher->vsReserveInhalt)->ulReserve = 0;

    if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwDoppelSpeicherSeite && stSuchBlock.veEintrag->ulBytes){
      VMBlock_ReserveTabelle_Aktivieren(vmSpeicher, stSuchBlock);
      stSuchBlock.vsTabelle = ((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste;

#ifndef _64bit
      ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->vbAdresse = nullptr;
      ((VMEINTRAG)&stSuchBlock.vsTabelle[16])->ulBytes = dwEineSpeicherSeite - 16;
      stSuchBlock.veEintrag = ((VMEINTRAG)&stSuchBlock.vsTabelle[16]);
      ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else
			((VMEINTRAG)&stSuchBlock.vsTabelle[32])->vbAdresse = nullptr;
			((VMEINTRAG)&stSuchBlock.vsTabelle[32])->ulBytes = dwEineSpeicherSeite - 32;
			stSuchBlock.veEintrag = ((VMEINTRAG)&stSuchBlock.vsTabelle[32]);
			((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
    }
    else{
      if(((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt == dwEineSpeicherSeite && stSuchBlock.veEintrag->ulBytes) VMBlock_ReserveTabelle_Neu(vmSpeicher, stSuchBlock);
      if(stSuchBlock.veEintrag->ulBytes){
        stSuchBlock.veEintrag++;
        stSuchBlock.veEintrag->vbAdresse = nullptr;
#ifndef _64bit
        ((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 8;
#else 
				((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt += 16;
#endif
      }
#ifndef _64bit
      stSuchBlock.veEintrag->ulBytes = dwEineSpeicherSeite - 16;
#else
			stSuchBlock.veEintrag->ulBytes = dwEineSpeicherSeite - 32;
#endif
    }
    stSuchBlock.veVorheriger = nullptr;

#ifndef _64bit
    ((VMSEITENKOPF)_vmSpeicher->vsReserveInhalt)->ulBelegt = 16;
#else
		((VMSEITENKOPF)_vmSpeicher->vsReserveInhalt)->ulBelegt = 32;
#endif
    ((VMSEITENKOPF)_vmSpeicher->vsReserveInhalt)->vsNachste = nullptr;
    ((VMSEITENKOPF)_vmSpeicher->vsReserveInhalt)->vsVorherige = _vmSpeicher->vsLetzteInhaltKlein;
    if(((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste){
      ((VMSEITENKOPF)_vmSpeicher->vsReserveInhalt)->vsNachste = ((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste;
      ((VMSEITENKOPF)((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste)->vsVorherige = _vmSpeicher->vsReserveInhalt;
    }
    ((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste = _vmSpeicher->vsReserveInhalt;
    _vmSpeicher->vsLetzteInhaltKlein = _vmSpeicher->vsReserveInhalt;
    _vmSpeicher->vsReserveInhalt = nullptr;
    stSuchBlock.vsInhalt = _vmSpeicher->vsLetzteInhaltKlein;
  }
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

        stSuchBlock.ulInhaltBelegt += stSuchBlock.veEintrag->ulBytes;
        if(stSuchBlock.ulInhaltBelegt == dwDoppelSpeicherSeite){
          stSuchBlock.vsInhalt = ((VMSEITENKOPF)stSuchBlock.vsInhalt)->vsNachste;	stSuchBlock.veVorheriger = nullptr;
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
      while(TRUE);

      NeueInhaltSeite(vmSpeicher, stSuchBlock);
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
  else{ VMBLOCK vbAdresse;
    STSuchBlock stSuchBlock;
    stSuchBlock.ulBytes = ulBytes;
    stSuchBlock.ucAlign = ucAlign;
    stSuchBlock.vsTabelle = _vmSpeicher->vsErsteTabelle;
    stSuchBlock.vsInhalt = _vmSpeicher->vsLetzteInhaltKlein;
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

    stSuchBlock.vsTabelle = _vmSpeicher->vsLetzteTabelle;

#ifndef _64bit
    stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 8];
    stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt - 8];

    if(stSuchBlock.veEintrag == ((VMEINTRAG)&stSuchBlock.vsTabelle[16])) stSuchBlock.veVorheriger = ((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsVorherige[dwDoppelSpeicherSeite - 8]);
#else
		stSuchBlock.veLetzterEintragDoppelSeite = (VMEINTRAG)&stSuchBlock.vsTabelle[dwDoppelSpeicherSeite - 16];
		stSuchBlock.veEintrag = (VMEINTRAG)&stSuchBlock.vsTabelle[((VMSEITENKOPF)stSuchBlock.vsTabelle)->ulBelegt - 16];

		if(stSuchBlock.veEintrag == ((VMEINTRAG)&stSuchBlock.vsTabelle[32])) stSuchBlock.veVorheriger = ((VMEINTRAG)&((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsVorherige[dwDoppelSpeicherSeite - 16]);
#endif
		else stSuchBlock.veVorheriger = (stSuchBlock.veEintrag - 1);
    stSuchBlock.ulInhaltBelegt = ((VMSEITENKOPF)stSuchBlock.vsInhalt)->ulBelegt - stSuchBlock.veEintrag->ulBytes;

    do{
      do{
        vbAdresse = nullptr;
        if(vbAdresse = VMEintragGleichOderGrosser(vmSpeicher, stSuchBlock, vbAdresse)) return vbAdresse;
        else if(!(((VMSEITENKOPF)stSuchBlock.vsTabelle)->vsNachste) && LetzterEintrag(stSuchBlock)) break;
      }
      while(TRUE);

      NeueInhaltSeite(vmSpeicher, stSuchBlock);
    }
    while(TRUE);
  }
}
//---------------------------------------------------------------------------
INLINE void __vectorcall VMFrei_ReserveTabelle(_In_ VMEMORY vmSpeicher, _In_ STSuchFrei& stSuchFrei)
{
  _vmSpeicher->vsLetzteTabelle = stSuchFrei.vsTabelle;
  if(_vmSpeicher->vsReserveTabelle) VirtualFree(_TabelleNachsteSeite, 0, MEM_RELEASE);
  else{ VirtualAlloc(_TabelleNachsteSeite, dwDoppelSpeicherSeite, MEM_RESERVE, PAGE_NOACCESS); _vmSpeicher->vsReserveTabelle = _TabelleNachsteSeite; }
  _TabelleNachsteSeite = NULL;
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
        _VorVorLetzterEintragVorherigeSeite->ulBytes = _ErsterEintrag->ulBytes;
        _VorVorLetzterEintragVorherigeSeite->vbAdresse = _ErsterEintrag->vbAdresse;
        _VorLetzterEintragVorherigeSeite->ulBytes = _ZweiterEintrag->ulBytes;
        _VorLetzterEintragVorherigeSeite->vbAdresse = _ZweiterEintrag->vbAdresse;
        _LetzterEintragVorherigeSeite->ulBytes = _DritterEintrag->ulBytes;
        _LetzterEintragVorherigeSeite->vbAdresse = _DritterEintrag->vbAdresse;
      }
      else if(stSuchFrei.ucAdresse == 2){
        _VorLetzterEintragVorherigeSeite->ulBytes = _ErsterEintrag->ulBytes;
        _VorLetzterEintragVorherigeSeite->vbAdresse = _ErsterEintrag->vbAdresse;
        _LetzterEintragVorherigeSeite->ulBytes = _ZweiterEintrag->ulBytes;
        _LetzterEintragVorherigeSeite->vbAdresse = _ZweiterEintrag->vbAdresse;
      }
      else{
        _LetzterEintragVorherigeSeite->ulBytes = _ErsterEintrag->ulBytes;
        _LetzterEintragVorherigeSeite->vbAdresse = _ErsterEintrag->vbAdresse;
      }

      VMFrei_ScrollEintrage_2(stSuchFrei);
    }
#ifndef _64bit
    else if(stSuchFrei.ucAdresse == (_Tabelle->ulBelegt - 16) / 8){
#else
		else if (stSuchFrei.ucAdresse == (_Tabelle->ulBelegt - 32) / 16) {
#endif
      if(stSuchFrei.ucAdresse == 3){
        _VorVorLetzterEintragVorherigeSeite->ulBytes = _ErsterEintrag->ulBytes;
        _VorVorLetzterEintragVorherigeSeite->vbAdresse = _ErsterEintrag->vbAdresse;
        _VorLetzterEintragVorherigeSeite->ulBytes = _ZweiterEintrag->ulBytes;
        _VorLetzterEintragVorherigeSeite->vbAdresse = _ZweiterEintrag->vbAdresse;
        _LetzterEintragVorherigeSeite->ulBytes = _DritterEintrag->ulBytes;
        _LetzterEintragVorherigeSeite->vbAdresse = _DritterEintrag->vbAdresse;
        stSuchFrei.ulScrollSprung = 0;
      }
      else if(stSuchFrei.ucAdresse == 2){
        _VorLetzterEintragVorherigeSeite->ulBytes = _ErsterEintrag->ulBytes;
        _VorLetzterEintragVorherigeSeite->vbAdresse = _ErsterEintrag->vbAdresse;
        _LetzterEintragVorherigeSeite->ulBytes = _ZweiterEintrag->ulBytes;
        _LetzterEintragVorherigeSeite->vbAdresse = _ZweiterEintrag->vbAdresse;
        stSuchFrei.ulScrollSprung = 0;
      }
      else{
        _LetzterEintragVorherigeSeite->ulBytes = _ErsterEintrag->ulBytes;
        _LetzterEintragVorherigeSeite->vbAdresse = _ErsterEintrag->vbAdresse;
        stSuchFrei.ulScrollSprung = 0;
      }

      stSuchFrei.vsTabelle = _Tabelle->vsVorherige;
      VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
    }
    else{
      if(stSuchFrei.ucAdresse == 2){
        _VorLetzterEintragVorherigeSeite->ulBytes = _ErsterEintrag->ulBytes;
        _VorLetzterEintragVorherigeSeite->vbAdresse = _ErsterEintrag->vbAdresse;
#ifndef _64bit
        stSuchFrei.ulScrollSprung = 8;
#else
				stSuchFrei.ulScrollSprung = 16;
#endif
      }
      else{
        _VorVorLetzterEintragVorherigeSeite->ulBytes = _ErsterEintrag->ulBytes;
        _VorVorLetzterEintragVorherigeSeite->vbAdresse = _ErsterEintrag->vbAdresse;
#ifndef _64bit
        if((_Tabelle->ulBelegt - 16) / 8 == 2){
#else
				if ((_Tabelle->ulBelegt - 32) / 16 == 2) {
#endif
          _VorLetzterEintragVorherigeSeite->ulBytes = _ZweiterEintrag->ulBytes;
          _VorLetzterEintragVorherigeSeite->vbAdresse = _ZweiterEintrag->vbAdresse;
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
      VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
    }
  }
  _Tabelle->ulBelegt -= stSuchFrei.ulScrollSprung;
}
//---------------------------------------------------------------------------
INLINE void __vectorcall VMFrei(_In_ VMEMORY vmSpeicher, _In_ STSuchFrei& stSuchFrei, _In_ void* pvAdresse)
{
  stSuchFrei.vsInhalt = _vmSpeicher->vsLetzteInhaltKlein;

  do{
    if(stSuchFrei.veEintrag->vbAdresse >= stSuchFrei.vsInhalt && stSuchFrei.veEintrag->vbAdresse <= stSuchFrei.vsInhalt + dwDoppelSpeicherSeite){
      _Inhalt->ulBelegt -= stSuchFrei.veEintrag->ulBytes;
#ifndef _64bit
      if(_Inhalt->vsVorherige && _Inhalt->ulBelegt == 16){
#else
			if(_Inhalt->vsVorherige && _Inhalt->ulBelegt == 32){
#endif
        if(stSuchFrei.veEintrag->vbAdresse == _ErsterEintragInhaltsSeite){
          if(stSuchFrei.veEintrag == _LetzterEintragDoppelSeite){
            if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
              stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
#ifndef _64bit
              if(!_VorherigerEintrag->vbAdresse) _Tabelle->ulBelegt -= 8;
#else
							if(!_VorherigerEintrag->vbAdresse) _Tabelle->ulBelegt -= 16;
#endif
              VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
              stSuchFrei.ulScrollSprung = 0;
            }
            else{
              stSuchFrei.veEintrag->vbAdresse = _ZweiterEintragNachsteSeite->vbAdresse;
              stSuchFrei.veEintrag->ulBytes = _ZweiterEintragNachsteSeite->ulBytes;

              if(!_TabelleNachsteNachsteSeite && _ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                stSuchFrei.ulScrollSprung = 0;
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
          }
          else{
            if(stSuchFrei.veEintrag == _ErsterEintrag){
              if(!_TabelleNachsteSeite && _ZweiterEintrag == stSuchFrei.veLetzterEintrag){
                if(!_LetzterEintragVorherigeSeite->vbAdresse){
                  stSuchFrei.vsTabelle = _Tabelle->vsVorherige;
                  VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                  stSuchFrei.ulScrollSprung = 0;
                }
                else{
                  stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
#ifndef _64bit
                  _Tabelle->ulBelegt -= 8;
#else
									_Tabelle->ulBelegt -= 16;
#endif
                  stSuchFrei.ulScrollSprung = 0;
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
            else{
              if(!_TabelleNachsteSeite && _NachsterEintrag == stSuchFrei.veLetzterEintrag){
                stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
                _NachsterEintrag->ulBytes = 0;

#ifndef _64bit 
                if(!_VorherigerEintrag->vbAdresse) _Tabelle->ulBelegt -= 16;
                else _Tabelle->ulBelegt -= 8;
#else
								if (!_VorherigerEintrag->vbAdresse) _Tabelle->ulBelegt -= 32;
								else _Tabelle->ulBelegt -= 16;
#endif
                stSuchFrei.ulScrollSprung = 0;
              }
              else if(stSuchFrei.veEintrag == _VorletzterEintragSeite){
                _VorletzterEintragSeite->ulBytes = _ErsterEintragNachsteSeite->ulBytes;
                _VorletzterEintragSeite->vbAdresse = _ErsterEintragNachsteSeite->vbAdresse;

                if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                  _NachsterEintrag->ulBytes = 0;
#ifndef _64bit
                  _Tabelle->ulBelegt -= 8;
#else
									_Tabelle->ulBelegt -= 16;
#endif
                  VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                  stSuchFrei.ulScrollSprung = 0;
                }
                else{
                  _LetzterEintragSeite->ulBytes = _ZweiterEintragNachsteSeite->ulBytes;
                  _LetzterEintragSeite->vbAdresse = _ZweiterEintragNachsteSeite->vbAdresse;

                  if(!_TabelleNachsteNachsteSeite && _ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                    VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                    stSuchFrei.ulScrollSprung = 0;
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
        }
        else if(stSuchFrei.veEintrag->vbAdresse == _LetzterEintragInhaltsSeite){
          if(stSuchFrei.veEintrag == _LetzterEintragDoppelSeite){
            _VorletzterEintragSeite->vbAdresse = _ErsterEintragNachsteSeite->vbAdresse;
            _VorletzterEintragSeite->ulBytes = _ErsterEintragNachsteSeite->ulBytes;

            if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
              stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
#ifndef _64bit
              _Tabelle->ulBelegt -= 8;
#else
							_Tabelle->ulBelegt -= 16;
#endif

              VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
              stSuchFrei.ulScrollSprung = 0;
            }
            else{
              stSuchFrei.veEintrag->vbAdresse = _ZweiterEintragNachsteSeite->vbAdresse;
              stSuchFrei.veEintrag->ulBytes = _ZweiterEintragNachsteSeite->ulBytes;

              if(!_TabelleNachsteNachsteSeite && _ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                stSuchFrei.ulScrollSprung = 0;
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
          }
          else{
            if(stSuchFrei.veEintrag == _ErsterEintrag){
              _LetzterEintragVorherigeSeite->vbAdresse = _ZweiterEintrag->vbAdresse;
              _LetzterEintragVorherigeSeite->ulBytes = _ZweiterEintrag->ulBytes;

              if(!_TabelleNachsteSeite && _ZweiterEintrag == _LetzterEintrag){
                stSuchFrei.vsTabelle = _Tabelle->vsVorherige;
                VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                stSuchFrei.ulScrollSprung = 0;
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
            else{
              stSuchFrei.veEintrag--;
              stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
#ifndef _64bit
              stSuchFrei.ulScrollSprung = 16;
#else
							stSuchFrei.ulScrollSprung = 32;
#endif
            }
          }
        }
        else{
          if(stSuchFrei.veEintrag == _LetzterEintragDoppelSeite){
            if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
              stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
              stSuchFrei.veEintrag--;
              stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
              stSuchFrei.veEintrag--;
#ifndef _64bit
              if(!stSuchFrei.veEintrag->vbAdresse) _Tabelle->ulBelegt -= 16;
              else _Tabelle->ulBelegt -= 8;
#else
							if (!stSuchFrei.veEintrag->vbAdresse) _Tabelle->ulBelegt -= 32;
							else _Tabelle->ulBelegt -= 16;
#endif
              VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
              stSuchFrei.ulScrollSprung = 0;
            }
            else{
              _VorletzterEintragSeite->vbAdresse = _ZweiterEintragNachsteSeite->vbAdresse;
              _VorletzterEintragSeite->ulBytes = _ZweiterEintragNachsteSeite->ulBytes;

              if(!_TabelleNachsteNachsteSeite && _ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
#ifndef _64bit
                _Tabelle->ulBelegt -= 8;
#else
								_Tabelle->ulBelegt -= 16;
#endif
                VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                stSuchFrei.ulScrollSprung = 0;
              }
              else{
                _LetzterEintragSeite->vbAdresse = _DritterEintragNachsteSeite->vbAdresse;
                _LetzterEintragSeite->ulBytes = _DritterEintragNachsteSeite->ulBytes;

                if(!_TabelleNachsteNachsteSeite && _DritterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                  VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                  stSuchFrei.ulScrollSprung = 0;
                }
                else{
                  stSuchFrei.vsTabelle = _TabelleNachsteSeite;
                  stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
                  stSuchFrei.ulScrollSprung = 24;
#else
									stSuchFrei.ulScrollSprung = 48;
#endif
                }
              }
            }
          }
          else{
            if(stSuchFrei.veEintrag == _ErsterEintrag){
              if(!_TabelleNachsteSeite && _ZweiterEintrag == stSuchFrei.veLetzterEintrag){
                stSuchFrei.vsTabelle = _Tabelle->vsVorherige;
                VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                stSuchFrei.ulScrollSprung = 0;

                _LetzterEintragSeite->ulBytes = 0;
#ifndef _64bit
                if(!_VorletzterEintragSeite->vbAdresse) _Tabelle->ulBelegt -= 8;
#else
								if(!_VorletzterEintragSeite->vbAdresse) _Tabelle->ulBelegt -= 16;
#endif
              }
              else{
                _LetzterEintragVorherigeSeite->vbAdresse = _DritterEintrag->vbAdresse;
                _LetzterEintragVorherigeSeite->ulBytes = _DritterEintrag->ulBytes;

                if(!_TabelleNachsteSeite && _DritterEintrag == stSuchFrei.veLetzterEintrag){
                  stSuchFrei.vsTabelle = _Tabelle->vsVorherige;
                  VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                  stSuchFrei.ulScrollSprung = 0;
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
            }
            else{
              if(stSuchFrei.veEintrag == _ZweiterEintrag){
                if(!_TabelleNachsteSeite && _DritterEintrag == stSuchFrei.veLetzterEintrag){
                  if(!_LetzterEintragVorherigeSeite->vbAdresse){
                    stSuchFrei.vsTabelle = _Tabelle->vsVorherige;
                    VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                  }
                  else{
                    stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
                    _DritterEintrag->ulBytes = 0;
                    _ErsterEintrag->ulBytes = 0;
#ifndef _64bit
                    _Tabelle->ulBelegt -= 16;
#else
										_Tabelle->ulBelegt -= 32;
#endif
                  }
                  stSuchFrei.ulScrollSprung = 0;
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
              else{
                if(stSuchFrei.veEintrag == _VorletzterEintragSeite){
                  if(!_TabelleNachsteSeite && _NachsterEintrag == stSuchFrei.veLetzterEintrag){
                    stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
                    _NachsterEintrag->ulBytes = 0;
                    _VorherigerEintrag->ulBytes = 0;
                    stSuchFrei.veEintrag -= 2;
#ifndef _64bit
                    if(!stSuchFrei.veEintrag->vbAdresse) _Tabelle->ulBelegt -= 24;
                    else _Tabelle->ulBelegt -= 16;
#else
										if(!stSuchFrei.veEintrag->vbAdresse) _Tabelle->ulBelegt -= 48;
										else _Tabelle->ulBelegt -= 32;
#endif
                    stSuchFrei.ulScrollSprung = 0;
                  }
                  else{
                    _VorVorletzterEintragSeite->ulBytes = _ErsterEintragNachsteSeite->ulBytes;
                    _VorVorletzterEintragSeite->vbAdresse = _ErsterEintragNachsteSeite->vbAdresse;

                    if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                      stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
                      _NachsterEintrag->ulBytes = 0;
#ifndef _64bit
                      _Tabelle->ulBelegt -= 16;
#else
											_Tabelle->ulBelegt -= 32;
#endif

                      VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                      stSuchFrei.ulScrollSprung = 0;
                    }
                    else{
                      _VorletzterEintragSeite->ulBytes = _ZweiterEintragNachsteSeite->ulBytes;
                      _VorletzterEintragSeite->vbAdresse = _ZweiterEintragNachsteSeite->vbAdresse;

                      if(!_TabelleNachsteNachsteSeite && _ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                        _NachsterEintrag->ulBytes = 0;
#ifndef _64bit
                        _Tabelle->ulBelegt -= 8;
#else
												_Tabelle->ulBelegt -= 16;
#endif

                        VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                        stSuchFrei.ulScrollSprung = 0;
                      }
                      else{
                        _LetzterEintragSeite->ulBytes = _DritterEintragNachsteSeite->ulBytes;
                        _LetzterEintragSeite->vbAdresse = _DritterEintragNachsteSeite->vbAdresse;

                        if(!_TabelleNachsteNachsteSeite && _DritterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                          VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                          stSuchFrei.ulScrollSprung = 0;
                        }
                        else{
                          stSuchFrei.vsTabelle = _TabelleNachsteSeite;
                          stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
                          stSuchFrei.ulScrollSprung = 24;
#else
													stSuchFrei.ulScrollSprung = 48;
#endif
                        }
                      }
                    }
                  }
                }
                else{
                  if(!_TabelleNachsteSeite && _NachsterEintrag == stSuchFrei.veLetzterEintrag){
                    stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
                    _NachsterEintrag->ulBytes = 0;
                    _VorherigerEintrag->ulBytes = 0;
                    stSuchFrei.veEintrag -= 2;
#ifndef _64bit
                    if(!stSuchFrei.veEintrag->vbAdresse) _Tabelle->ulBelegt -= 24;
                    else _Tabelle->ulBelegt -= 16;
#else
										if(!stSuchFrei.veEintrag->vbAdresse) _Tabelle->ulBelegt -= 48;
										else _Tabelle->ulBelegt -= 32;
#endif
                    stSuchFrei.ulScrollSprung = 0;
                  }
                  else{
                    stSuchFrei.veEintrag--;
                    stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
#ifndef _64bit
                    stSuchFrei.ulScrollSprung = 24;
#else
										stSuchFrei.ulScrollSprung = 48;
#endif
                  }
                }
              }
            }
          }
        }

        if(stSuchFrei.ulScrollSprung) VMFrei_ScrollEintrage(vmSpeicher, stSuchFrei);

        ((VMSEITENKOPF)_Inhalt->vsVorherige)->vsNachste = _Inhalt->vsNachste;
        if(_Inhalt->vsNachste) ((VMSEITENKOPF)_Inhalt->vsNachste)->vsVorherige = _Inhalt->vsVorherige;
        if(_vmSpeicher->vsLetzteInhaltKlein == stSuchFrei.vsInhalt){
          _vmSpeicher->vsLetzteInhaltKlein = _Inhalt->vsVorherige;
          if(!_vmSpeicher->vsReserveInhalt){
            _vmSpeicher->vsReserveInhalt = (VMSEITE)VirtualAlloc(NULL, dwDoppelSpeicherSeite, MEM_RESERVE, PAGE_NOACCESS);
          }
        }
        VirtualFree(stSuchFrei.vsInhalt, 0, MEM_RELEASE);

        if(_Tabelle->ulBelegt == dwEineSpeicherSeite){
          VirtualAlloc(stSuchFrei.vsTabelle + dwEineSpeicherSeite, dwEineSpeicherSeite, MEM_RESERVE, PAGE_NOACCESS);
          _Tabelle->ulReserve = 0;
          VirtualFree(_vmSpeicher->vsReserveTabelle, 0, MEM_RELEASE);
          _vmSpeicher->vsReserveTabelle = nullptr;
        }
        LeaveCriticalSection(&_vmSpeicher->csTabelle);
        return;
      }
      else break;
    }
    stSuchFrei.vsInhalt = _Inhalt->vsVorherige;
  }
  while(stSuchFrei.vsInhalt);

  if(stSuchFrei.veEintrag == _LetzterEintragDoppelSeite){
    if(stSuchFrei.veEintrag->vbAdresse == _ErsterEintragInhaltsSeite){
      if(!_ErsterEintragNachsteSeite->vbAdresse){
        stSuchFrei.veEintrag->ulBytes += _ErsterEintragNachsteSeite->ulBytes;
        stSuchFrei.veEintrag->vbAdresse = nullptr;

        stSuchFrei.vsTabelle = _Tabelle->vsNachste;
        stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
        stSuchFrei.ulScrollSprung = 8;
#else
				stSuchFrei.ulScrollSprung = 16;
#endif
      }
      else{
        stSuchFrei.veEintrag->vbAdresse = nullptr;
        stSuchFrei.ulScrollSprung = 0;
      }
    }
    else{
      if(stSuchFrei.veEintrag->vbAdresse == _LetzterEintragInhaltsSeite){
        if(!_VorletzterEintragSeite->vbAdresse){
          _VorletzterEintragSeite->ulBytes += stSuchFrei.veEintrag->ulBytes;
          stSuchFrei.veEintrag->vbAdresse = _ErsterEintragNachsteSeite->vbAdresse;
          stSuchFrei.veEintrag->ulBytes = _ErsterEintragNachsteSeite->ulBytes;

          if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
            VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
            stSuchFrei.ulScrollSprung = 0;
          }
          else{
            stSuchFrei.vsTabelle = _Tabelle->vsNachste;
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
          stSuchFrei.ulScrollSprung = 0;
        }
      }
      else{
        if(!_VorletzterEintragSeite->vbAdresse){
          if(!_ErsterEintragNachsteSeite->vbAdresse){
            _VorletzterEintragSeite->ulBytes += stSuchFrei.veEintrag->ulBytes + _ErsterEintragNachsteSeite->ulBytes;

            if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
              VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);

              stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
#ifndef _64bit
              _Tabelle->ulBelegt -= 8;
#else
							_Tabelle->ulBelegt -= 16;
#endif
              stSuchFrei.ulScrollSprung = 0;
            }
            else{
              stSuchFrei.veEintrag->vbAdresse = _ZweiterEintragNachsteSeite->vbAdresse;
              stSuchFrei.veEintrag->ulBytes = _ZweiterEintragNachsteSeite->ulBytes;

              if(!_TabelleNachsteNachsteSeite && _ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                stSuchFrei.ulScrollSprung = 0;
              }
              else{
                stSuchFrei.vsTabelle = _Tabelle->vsNachste;
                stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
                stSuchFrei.ulScrollSprung = 16;
#else
								stSuchFrei.ulScrollSprung = 32;
#endif
              }
            }
          }
          else{
            _VorletzterEintragSeite->ulBytes += stSuchFrei.veEintrag->ulBytes;
            stSuchFrei.veEintrag->vbAdresse = _ErsterEintragNachsteSeite->vbAdresse;
            stSuchFrei.veEintrag->ulBytes = _ErsterEintragNachsteSeite->ulBytes;

            stSuchFrei.vsTabelle = _Tabelle->vsNachste;
            stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
            stSuchFrei.ulScrollSprung = 8;
#else
						stSuchFrei.ulScrollSprung = 16;
#endif
          }
        }
        else{
          if(!_ErsterEintragNachsteSeite->vbAdresse){
            stSuchFrei.veEintrag->ulBytes += _ErsterEintragNachsteSeite->ulBytes;
            stSuchFrei.veEintrag->vbAdresse = nullptr;

            if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
              VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
              stSuchFrei.ulScrollSprung = 0;
            }
            else{
              stSuchFrei.vsTabelle = _Tabelle->vsNachste;
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
            stSuchFrei.ulScrollSprung = 0;
          }
        }
      }
    }
  }
  else{
    if(stSuchFrei.veEintrag == _ErsterEintrag){
      if(stSuchFrei.veEintrag->vbAdresse == _ErsterEintragInhaltsSeite){
        if(!_ZweiterEintrag->vbAdresse){
          stSuchFrei.veEintrag->ulBytes += _ZweiterEintrag->ulBytes;
          stSuchFrei.veEintrag->vbAdresse = nullptr;

          stSuchFrei.veScrollEintrag = _ZweiterEintrag;
#ifndef _64bit
          stSuchFrei.ulScrollSprung = 8;
#else
					stSuchFrei.ulScrollSprung = 16;
#endif
        }
        else{
          stSuchFrei.veEintrag->vbAdresse = nullptr;
          stSuchFrei.ulScrollSprung = 0;
        }
      }
      else{
        if(stSuchFrei.veEintrag->vbAdresse == _LetzterEintragInhaltsSeite){
          if(!_LetzterEintragVorherigeSeite->vbAdresse){
            _LetzterEintragVorherigeSeite->ulBytes += stSuchFrei.veEintrag->ulBytes;

            stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
            stSuchFrei.ulScrollSprung = 8;
#else
						stSuchFrei.ulScrollSprung = 16;
#endif
          }
          else{
            stSuchFrei.veEintrag->vbAdresse = nullptr;
            stSuchFrei.ulScrollSprung = 0;
          }
        }
        else{
          if(!_LetzterEintragVorherigeSeite->vbAdresse){
            if(!_ZweiterEintrag->vbAdresse){
              _LetzterEintragVorherigeSeite->ulBytes += stSuchFrei.veEintrag->ulBytes + _ZweiterEintrag->ulBytes;

              if(!_TabelleNachsteSeite && _ZweiterEintrag == stSuchFrei.veLetzterEintrag){
                stSuchFrei.vsTabelle = _Tabelle->vsVorherige;
                VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                stSuchFrei.ulScrollSprung = 0;
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
            else{
              _LetzterEintragVorherigeSeite->ulBytes += stSuchFrei.veEintrag->ulBytes;
              stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
#ifndef _64bit
              stSuchFrei.ulScrollSprung = 8;
#else
							stSuchFrei.ulScrollSprung = 16;
#endif
            }
          }
          else{
            if(!_ZweiterEintrag->vbAdresse){
              stSuchFrei.veEintrag->ulBytes += _ZweiterEintrag->ulBytes;
              stSuchFrei.veEintrag->vbAdresse = nullptr;

              if(!_TabelleNachsteSeite && _ZweiterEintrag == stSuchFrei.veLetzterEintrag){
                _ZweiterEintrag->ulBytes = 0;
#ifndef _64bit
                _Tabelle->ulBelegt -= 8;
#else
								_Tabelle->ulBelegt -= 16;
#endif
                stSuchFrei.ulScrollSprung = 0;
              }
              else{
                stSuchFrei.veScrollEintrag = _ZweiterEintrag;
#ifndef _64bit
                stSuchFrei.ulScrollSprung = 8;
#else
								stSuchFrei.ulScrollSprung = 16;
#endif
              }
            }
            else{
              stSuchFrei.veEintrag->vbAdresse = nullptr;
              stSuchFrei.ulScrollSprung = 0;
            }
          }
        }
      }
    }
    else{
      if(stSuchFrei.veEintrag->vbAdresse == _ErsterEintragInhaltsSeite){
        if(!_NachsterEintrag->vbAdresse){
          stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
          stSuchFrei.veEintrag->vbAdresse = nullptr;

          if(stSuchFrei.veEintrag == _VorletzterEintragSeite){
            _LetzterEintragSeite->ulBytes = _ErsterEintragNachsteSeite->ulBytes;
            _LetzterEintragSeite->vbAdresse = _ErsterEintragNachsteSeite->vbAdresse;

            stSuchFrei.vsTabelle = _Tabelle->vsNachste;
            stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
            stSuchFrei.ulScrollSprung = 8;
#else
						stSuchFrei.ulScrollSprung = 16;
#endif
          }
          else{
            stSuchFrei.veEintrag++;
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
          stSuchFrei.ulScrollSprung = 0;
        }
      }
      else{
        if(stSuchFrei.veEintrag->vbAdresse == _LetzterEintragInhaltsSeite){
          if(!_VorherigerEintrag->vbAdresse){
            _VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
            stSuchFrei.veEintrag->vbAdresse = _NachsterEintrag->vbAdresse;
            stSuchFrei.veEintrag->ulBytes = _NachsterEintrag->ulBytes;

            if(!_TabelleNachsteSeite && _NachsterEintrag == _LetzterEintrag){
              _NachsterEintrag->ulBytes = 0;
#ifndef _64bit
              _Tabelle->ulBelegt -= 8;
#else
							_Tabelle->ulBelegt -= 16;
#endif
              stSuchFrei.ulScrollSprung = 0;
            }
            else if(stSuchFrei.veEintrag == _VorletzterEintragSeite){
              _LetzterEintragSeite->ulBytes = _ErsterEintragNachsteSeite->ulBytes;
              _LetzterEintragSeite->vbAdresse = _ErsterEintragNachsteSeite->vbAdresse;

              if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                stSuchFrei.ulScrollSprung = 0;
              }
              else{
                stSuchFrei.vsTabelle = _Tabelle->vsNachste;
                stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
                stSuchFrei.ulScrollSprung = 8;
#else
								stSuchFrei.ulScrollSprung = 16;
#endif
              }
            }
            else{
              stSuchFrei.veEintrag++;
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
            stSuchFrei.ulScrollSprung = 0;
          }
        }
        else{
          if(!_VorherigerEintrag->vbAdresse){
            if(!_NachsterEintrag->vbAdresse){
              _VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes + _NachsterEintrag->ulBytes;

              if(!_TabelleNachsteSeite && _NachsterEintrag == _LetzterEintrag){
                stSuchFrei.veEintrag->vbAdresse = nullptr; stSuchFrei.veEintrag->ulBytes = 0;
                _NachsterEintrag->ulBytes = 0;
#ifndef _64bit
                _Tabelle->ulBelegt -= 16;
#else 
								_Tabelle->ulBelegt -= 32;
#endif
                stSuchFrei.ulScrollSprung = 0;
              }
              else if(stSuchFrei.veEintrag == _VorletzterEintragSeite){
                _VorletzterEintragSeite->ulBytes = _ErsterEintragNachsteSeite->ulBytes;
                _VorletzterEintragSeite->vbAdresse = _ErsterEintragNachsteSeite->vbAdresse;

                if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                  VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);

                  _NachsterEintrag->ulBytes = 0;
#ifndef _64bit
                  _Tabelle->ulBelegt -= 8;
#else
									_Tabelle->ulBelegt -= 16;
#endif
                  stSuchFrei.ulScrollSprung = 0;
                }
                else{
                  _LetzterEintragSeite->ulBytes = _ZweiterEintragNachsteSeite->ulBytes;
                  _LetzterEintragSeite->vbAdresse = _ZweiterEintragNachsteSeite->vbAdresse;

                  if(!_TabelleNachsteNachsteSeite && _ZweiterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                    VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                    stSuchFrei.ulScrollSprung = 0;
                  }
                  else{
                    stSuchFrei.vsTabelle = _Tabelle->vsNachste;
                    stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
                    stSuchFrei.ulScrollSprung = 16;
#else
										stSuchFrei.ulScrollSprung = 32;
#endif
                  }
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
            else{
              _VorherigerEintrag->ulBytes += stSuchFrei.veEintrag->ulBytes;
              stSuchFrei.veScrollEintrag = stSuchFrei.veEintrag;
#ifndef _64bit
              stSuchFrei.ulScrollSprung = 8;
#else
							stSuchFrei.ulScrollSprung = 16;
#endif
            }
          }
          else{
            if(!_NachsterEintrag->vbAdresse){
              stSuchFrei.veEintrag->ulBytes += _NachsterEintrag->ulBytes;
              stSuchFrei.veEintrag->vbAdresse = nullptr;

              if(!_TabelleNachsteSeite && _NachsterEintrag == _LetzterEintrag){
                _NachsterEintrag->ulBytes = 0;
#ifndef _64bit
                _Tabelle->ulBelegt -= 8;
#else
								_Tabelle->ulBelegt -= 16;
#endif
                stSuchFrei.ulScrollSprung = 0;
              }
              else if(stSuchFrei.veEintrag == _VorletzterEintragSeite){
                _LetzterEintragSeite->ulBytes = _ErsterEintragNachsteSeite->ulBytes;
                _LetzterEintragSeite->vbAdresse = _ErsterEintragNachsteSeite->vbAdresse;

                if(!_TabelleNachsteNachsteSeite && _ErsterEintragNachsteSeite == _LetzterEintragNachsteSeite){
                  VMFrei_ReserveTabelle(vmSpeicher, stSuchFrei);
                  stSuchFrei.ulScrollSprung = 0;
                }
                else{
                  stSuchFrei.vsTabelle = _Tabelle->vsNachste;
                  stSuchFrei.veScrollEintrag = _ErsterEintrag;
#ifndef _64bit
                  stSuchFrei.ulScrollSprung = 8;
#else
									stSuchFrei.ulScrollSprung = 16;
#endif
                }
              }
              else{
                stSuchFrei.veEintrag++;
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
              stSuchFrei.ulScrollSprung = 0;
            }
          }
        }
      }
    }
  }
  if(stSuchFrei.ulScrollSprung) 
    VMFrei_ScrollEintrage(vmSpeicher, stSuchFrei);

  if(_Tabelle->ulBelegt == dwEineSpeicherSeite){
    VirtualAlloc(stSuchFrei.vsTabelle + dwEineSpeicherSeite, dwEineSpeicherSeite, MEM_RESERVE, PAGE_NOACCESS);
    _Tabelle->ulReserve = 0;
    VirtualFree(_vmSpeicher->vsReserveTabelle, 0, MEM_RELEASE);
    _vmSpeicher->vsReserveTabelle = NULL;
  }
  LeaveCriticalSection(&_vmSpeicher->csTabelle);
}
//---------------------------------------------------------------------------
void __vectorcall VMFrei(_In_ VMEMORY vmSpeicher, _In_ void* pvAdresse)
{
  if(!pvAdresse) return;
  if(!vmSpeicher) vmSpeicher = vmStandard;
  EnterCriticalSection(&_vmSpeicher->csTabelle);

  STSuchFrei stSuchFrei;
  stSuchFrei.vsInhalt = ((VMSEITENKOPF)_vmSpeicher->vsLetzteInhaltKlein)->vsNachste;
  while(stSuchFrei.vsInhalt){
#ifndef _64bit
    if(pvAdresse == (VMBLOCK)&stSuchFrei.vsInhalt[16]){
#else
		if(pvAdresse == (VMBLOCK)&stSuchFrei.vsInhalt[32]) {
#endif
      ((VMSEITENKOPF)_Inhalt->vsVorherige)->vsNachste = _Inhalt->vsNachste;
      if(_Inhalt->vsNachste) ((VMSEITENKOPF)_Inhalt->vsNachste)->vsVorherige = _Inhalt->vsVorherige;
      else if(_Inhalt->vsVorherige == _vmSpeicher->vsLetzteInhaltKlein) _vmSpeicher->vsLetzteInhaltGross = NULL;
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

  while(stSuchFrei.veEintrag->vbAdresse != pvAdresse){
    if(stSuchFrei.veEintrag == stSuchFrei.veLetzterEintragDoppelSeite){
      stSuchFrei.vsTabelle = _TabelleNachsteSeite;
      stSuchFrei.veEintrag = _ErsterEintrag;
      stSuchFrei.veLetzterEintrag = _LetzterEintrag;
      stSuchFrei.veLetzterEintragDoppelSeite = _LetzterEintragDoppelSeite;
    }
    else if(stSuchFrei.veEintrag == stSuchFrei.veLetzterEintrag){ LeaveCriticalSection(&_vmSpeicher->csTabelle); return; }
    else stSuchFrei.veEintrag++;
  }
  VMFrei(vmSpeicher, stSuchFrei, pvAdresse);
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
 if(_vmSpeicher->vfBlock == (VMFREI)&_vmSpeicher->vsFrei[dwEineSpeicherSeite - 4]) VMFrei(vmSpeicher, pvAdresse);
#else
 if(_vmSpeicher->vfBlock == (VMFREI)& _vmSpeicher->vsFrei[dwEineSpeicherSeite - 8]) VMFrei(vmSpeicher, pvAdresse);
#endif
 else{
  *_vmSpeicher->vfBlock = pvAdresse;
   _vmSpeicher->vfBlock++;
   SetEvent(_vmSpeicher->thvmFrei.heFertig);
 }
 LeaveCriticalSection(&_vmSpeicher->csFrei);
}                                           
//---------------------------------------------------------------------------