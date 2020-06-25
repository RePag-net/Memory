/****************************************************************************
  TCompSyS.cpp
  For more information see https://github.com/RePag/Memory
****************************************************************************/

/****************************************************************************
  The MIT License(MIT)

  Copyright(c) 2020 René Pagel

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
//---------------------------------------------------------------------------
#define __vmSpeicher ((VMSPEICHER)((STTHVMFrei*)lpvParam)->vmSpeicher)
//---------------------------------------------------------------------------
DWORD WINAPI thFrei(_In_ LPVOID lpvParam)
{
  WaitForSingleObject(((STTHVMFrei*)lpvParam)->heFertig, INFINITE);
  EnterCriticalSection(&__vmSpeicher->csFrei);
  while(!((STTHVMFrei*)lpvParam)->bEnde){
    do{
      LeaveCriticalSection(&__vmSpeicher->csFrei);
      VMFrei(__vmSpeicher, *__vmSpeicher->vfFrei);
      EnterCriticalSection(&__vmSpeicher->csFrei);
    }
    while(++__vmSpeicher->vfFrei < __vmSpeicher->vfBlock);
    __vmSpeicher->vfFrei = (VMFREI)&__vmSpeicher->vsFrei[16];
    __vmSpeicher->vfBlock = (VMFREI)&__vmSpeicher->vsFrei[16];
    do{
      LeaveCriticalSection(&__vmSpeicher->csFrei);
      WaitForSingleObject(((STTHVMFrei*)lpvParam)->heFertig, INFINITE);
      EnterCriticalSection(&__vmSpeicher->csFrei);
    }
    while(__vmSpeicher->vfFrei == __vmSpeicher->vfBlock);
  }
  CloseHandle(((STTHVMFrei*)lpvParam)->heFertig);
  return NULL;
}
//---------------------------------------------------------------------------