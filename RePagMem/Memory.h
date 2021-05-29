/****************************************************************************
  Memory.h
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

#pragma once
//---------------------------------------------------------------------------
#define EX_VM_KEINSPEICHER 90
#define VULONG (PULONG)VMBlock(4)
using VMEMORY = const void*;
using VMBLOCK = char*;
using VMBLOCKA = unsigned char*;
using VMSEITE = char*;
using VMFREI = void**;
using EXCEPTION = unsigned char;
using BIT128 = unsigned char[16];
using BIT256 = unsigned char[32];
using PBIT128 = BIT128*;
using PBIT256 = BIT256*;
using BIT128A = __declspec(align(16)) BIT128;
using BIT256A = __declspec(align(32)) BIT256;

extern VMEMORY vmStandard;
extern DWORD dwEineSpeicherSeite;
extern DWORD dwDoppelSpeicherSeite;
//---------------------------------------------------------------------------
typedef struct STSpeicher
{
  VMBLOCK vbName;
  VMEMORY vmMemory;
} STSpeicher;
//---------------------------------------------------------------------------
typedef struct STBlock
{
  VMBLOCK vbAdresse;
  ULONG ulBytes;
} STBlock;
typedef STBlock* VMEINTRAG;
//---------------------------------------------------------------------------
typedef struct STSeiten
{
  ULONG ulBelegt;
  ULONG ulReserve;
  VMSEITE vsVorherige;
  VMSEITE vsNachste;
} STSeiten;
typedef STSeiten* VMSEITENKOPF;
//---------------------------------------------------------------------------
struct STSuchBlock
{
  BYTE ucAlign;
  ULONG ulBytes;
  VMSEITE vsTabelle;
  VMSEITE vsInhalt;
  ULONG ulInhaltBelegt;
  VMEINTRAG veEintrag;
  VMEINTRAG veVorheriger;
  VMEINTRAG veLetzterEintragDoppelSeite;
};
//---------------------------------------------------------------------------
struct STSuchFrei
{
  VMSEITE vsTabelle;
  VMSEITE vsInhalt;
  VMEINTRAG veEintrag;
  VMEINTRAG veLetzterEintrag;
  VMEINTRAG veLetzterEintragDoppelSeite;
  ULONG ulScrollSprung;
  VMEINTRAG veScrollEintrag;
  BYTE ucAdresse;
};
//---------------------------------------------------------------------------

#include "TCompSys.h"
struct STVirtualSpeicher
{
  VMSEITE vsErsteTabelle;
  VMSEITE vsLetzteTabelle;
  VMSEITE vsReserveTabelle;
  VMSEITE vsErsteInhalt;
  VMSEITE vsLetzteInhaltKlein;
  VMSEITE vsLetzteInhaltGross;
  VMSEITE vsReserveInhalt;
  VMSEITE vsFrei;
  VMFREI vfFrei;
  VMFREI vfBlock;
  STTHVMFrei thvmFrei;
  CRITICAL_SECTION csTabelle;
  CRITICAL_SECTION csFrei;
  bool bAuslagern;
  bool bMapped;
};
typedef STVirtualSpeicher* VMSPEICHER;

//---------------------------------------------------------------------------
VMEMORY __vectorcall InitVirtualMemA(_In_ bool bAuslagern);
void __vectorcall CPUID(SYSTEM_INFO& stSystem_Info);
void* __vectorcall ZeroMem(_In_ void* vbMemory, _In_ unsigned long ulBytes);
//---------------------------------------------------------------------------
__declspec(dllexport) VMEMORY __vectorcall InitVirtualMem(_In_ bool bAuslagern);
__declspec(dllexport) void __vectorcall FreeVirtualMem(_In_ VMEMORY vmSpeicher);
__declspec(dllexport) VMBLOCK __vectorcall VMBlock(_In_ VMEMORY vmSpeicher, _In_ unsigned long ulBytes);
__declspec(dllexport) VMBLOCK __vectorcall VMBlockS(_In_ VMEMORY vmSpeicher, _In_ unsigned long ulBytes);
__declspec(dllexport) VMBLOCKA __vectorcall VMBlockA(_In_ VMEMORY vmSpeicher, _In_ unsigned long ulBytes, _In_ unsigned char ucAlign);
__declspec(dllexport) VMBLOCK __vectorcall VMBlock(_In_ unsigned long ulBytes);
__declspec(dllexport) VMBLOCK __vectorcall VMBlockS(_In_ unsigned long ulBytes);
__declspec(dllexport) VMBLOCKA __vectorcall VMBlockA(_In_ unsigned long ulBytes, unsigned char ucAlign);
__declspec(dllexport) void __vectorcall VMFrei(_In_ VMEMORY vmSpeicher, _In_ void* pvAdresse);
__declspec(dllexport) void __vectorcall VMFrei(_In_ void* pvAdresse);
__declspec(dllexport) void __vectorcall VMFreiS(_In_ void* pvAdresse);
__declspec(dllexport) void __vectorcall VMFreiS(_In_ VMEMORY vmSpeicher, _In_ void* pvAdresse);
//---------------------------------------------------------------------------
template <typename XTyp>
XTyp* __vectorcall VMBlock(XTyp* xTyp)
{
  return (XTyp*)VMBlock(sizeof(XTyp));
}
//---------------------------------------------------------------------------
template <typename XTyp>
XTyp* __vectorcall VMBlock(VMEMORY vmSpeicher, XTyp* xTyp)
{
  return (XTyp*)VMBlock(vmSpeicher, sizeof(XTyp));
}
//---------------------------------------------------------------------------
template <typename XTyp>
XTyp* __vectorcall VMBlockS(XTyp* xTyp)
{
  return (XTyp*)VMBlockS(sizeof(XTyp));
}
//---------------------------------------------------------------------------
template <typename XTyp>
XTyp* __vectorcall VMBlockS(VMEMORY vmSpeicher, XTyp* xTyp)
{
  return (XTyp*)VMBlockS(vmSpeicher, sizeof(XTyp));
}
//---------------------------------------------------------------------------
template <typename XTyp>
void __vectorcall VMFreiV(XTyp xTyp)
{
  VMFrei(xTyp->COFreiV(), xTyp);
}
//---------------------------------------------------------------------------
template <typename XTyp>
void __vectorcall VMFreiVS(XTyp xTyp)
{
  VMFreiS(xTyp->COFreiV(), xTyp);
}
//---------------------------------------------------------------------------