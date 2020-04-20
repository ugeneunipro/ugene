/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

/**********************************************************************
* LICENSE (http://www.opensource.org/licenses/bsd-license.php)
*
*   Copyright (c) 2005-2011, Jochen Kalmbach
*   All rights reserved.
*
*   Redistribution and use in source and binary forms, with or without modification,
*   are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
*   Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*   Neither the name of Jochen Kalmbach nor the names of its contributors may be
*   used to endorse or promote products derived from this software without
*   specific prior written permission.
*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
*   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
*   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
*   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
*   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
**********************************************************************/

#include "StackWalker.h"
#if defined (Q_OS_WIN)

#pragma comment(lib, "version.lib")  // for "VerQueryValue"




#include <tchar.h>
#include <stdio.h>
#include <fstream>

#include <dbghelp.h>
#include <QFile>
#include <QStringList>

#define USED_CONTEXT_FLAGS CONTEXT_ALL

namespace U2 {


class StackWalkerInternal
{
public:
    StackWalkerInternal(StackWalker *parent, HANDLE hProcess) {
        m_parent = parent;
        m_hDbhHelp = NULL;
        pSC = NULL;
        m_hProcess = hProcess;
        m_szSymPath = NULL;
        pSFTA = NULL;
        pSGLFA = NULL;
        pSGMB = NULL;
        pSGMI = NULL;
        pSGO = NULL;
        pSGSFA = NULL;
        pSI = NULL;
        pSLM = NULL;
        pSSO = NULL;
        pSW = NULL;
        pUDSN = NULL;
        pSGSP = NULL;
    }
    ~StackWalkerInternal() {
        if (pSC != NULL) {
          pSC(m_hProcess);  // SymCleanup
        }
        if (m_hDbhHelp != NULL) {
          FreeLibrary(m_hDbhHelp);
        }
        m_hDbhHelp = NULL;
        m_parent = NULL;
        if(m_szSymPath != NULL) {
          free(m_szSymPath);
        }
        m_szSymPath = NULL;
    }
    BOOL Init(LPCSTR szSymPath) {
        if (m_parent == NULL) {
            return FALSE;
          }
    // Dynamically load the Entry-Points for dbghelp.dll:
    // First try to load the newest one from
        wchar_t szTemp[4096];
    // But before we do this, we first check if the ".local" file exists
        if (GetModuleFileName(NULL, szTemp, 4096) > 0) {
            wcscat_s(szTemp, TEXT(".local"));
            if (GetFileAttributes(szTemp) == INVALID_FILE_ATTRIBUTES) {
        // ".local" file does not exist, so we can try to load the dbghelp.dll from the "Debugging Tools for Windows"
                if (GetEnvironmentVariable(TEXT("ProgramFiles"), szTemp, 4096) > 0) {
                    wcscat_s(szTemp, TEXT("\\Debugging Tools for Windows\\dbghelp.dll"));
                  // now check if the file exists:
                    if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES){
                        m_hDbhHelp = LoadLibrary(szTemp);
                    }
                }
          // Still not found? Then try to load the 64-Bit version:
                if ( (m_hDbhHelp == NULL) && (GetEnvironmentVariable(TEXT("ProgramFiles"), szTemp, 4096) > 0) ) {
                    wcscat_s(szTemp, TEXT("\\Debugging Tools for Windows 64-Bit\\dbghelp.dll"));
                    if (GetFileAttributes(szTemp) != INVALID_FILE_ATTRIBUTES){
                        m_hDbhHelp = LoadLibrary(szTemp);
                    }
                }
            }
        }
        if (m_hDbhHelp == NULL) { // if not already loaded, try to load a default-one
            m_hDbhHelp = LoadLibrary( TEXT("dbghelp.dll") );
        }
        if (m_hDbhHelp == NULL) {
            return FALSE;
        }
        pSI = (tSI) GetProcAddress(m_hDbhHelp, "SymInitialize" );
        pSC = (tSC) GetProcAddress(m_hDbhHelp, "SymCleanup" );

        pSW = (tSW) GetProcAddress(m_hDbhHelp, "StackWalk64" );
        pSGO = (tSGO) GetProcAddress(m_hDbhHelp, "SymGetOptions" );
        pSSO = (tSSO) GetProcAddress(m_hDbhHelp, "SymSetOptions" );

        pSFTA = (tSFTA) GetProcAddress(m_hDbhHelp, "SymFunctionTableAccess64" );
        pSGLFA = (tSGLFA) GetProcAddress(m_hDbhHelp, "SymGetLineFromAddr64" );
        pSGMB = (tSGMB) GetProcAddress(m_hDbhHelp, "SymGetModuleBase64" );
        pSGMI = (tSGMI) GetProcAddress(m_hDbhHelp, "SymGetModuleInfo64" );
        //pSGMI_V3 = (tSGMI_V3) GetProcAddress(m_hDbhHelp, "SymGetModuleInfo64" );
        pSGSFA = (tSGSFA) GetProcAddress(m_hDbhHelp, "SymGetSymFromAddr64" );
        pUDSN = (tUDSN) GetProcAddress(m_hDbhHelp, "UnDecorateSymbolName" );
        pSLM = (tSLM) GetProcAddress(m_hDbhHelp, "SymLoadModule64" );
        pSGSP =(tSGSP) GetProcAddress(m_hDbhHelp, "SymGetSearchPath" );

        if ( pSC == NULL || pSFTA == NULL || pSGMB == NULL || pSGMI == NULL ||
        pSGO == NULL || pSGSFA == NULL || pSI == NULL || pSSO == NULL ||
        pSW == NULL || pUDSN == NULL || pSLM == NULL ) {
            FreeLibrary(m_hDbhHelp);
            m_hDbhHelp = NULL;
            pSC = NULL;
            return FALSE;
        }

    // SymInitialize
        if (szSymPath != NULL) {
            m_szSymPath = _strdup(szSymPath);
        }
        if (this->pSI(m_hProcess, m_szSymPath, FALSE) == FALSE) {
            this->m_parent->OnDbgHelpErr("SymInitialize", GetLastError(), 0);
        }

        DWORD symOptions = this->pSGO();  // SymGetOptions
        symOptions |= SYMOPT_LOAD_LINES;
        symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;
        // SymSetOptions
        symOptions = this->pSSO(symOptions);

        char buf[StackWalker::STACKWALK_MAX_NAMELEN] = {0};
        if (this->pSGSP != NULL) {
            if (this->pSGSP(m_hProcess, buf, StackWalker::STACKWALK_MAX_NAMELEN) == FALSE) {
                this->m_parent->OnDbgHelpErr("SymGetSearchPath", GetLastError(), 0);
            }
        }
        char szUserName[1024] = {0};
        DWORD dwSize = 1024;
        GetUserNameA(szUserName, &dwSize);
        this->m_parent->OnSymInit(buf, symOptions, szUserName);

        return TRUE;
    }

    StackWalker *m_parent;

    HMODULE m_hDbhHelp;
    HANDLE m_hProcess;
    LPSTR m_szSymPath;

    typedef struct _IMAGEHLP_MODULE64_V2 {
        DWORD    SizeOfStruct;           // set to sizeof(IMAGEHLP_MODULE64)
        DWORD64  BaseOfImage;            // base load address of module
        DWORD    ImageSize;              // virtual size of the loaded module
        DWORD    TimeDateStamp;          // date/time stamp from pe header
        DWORD    CheckSum;               // checksum from the pe header
        DWORD    NumSyms;                // number of symbols in the symbol table
        SYM_TYPE SymType;                // type of symbols loaded
        CHAR     ModuleName[32];         // module name
        CHAR     ImageName[256];         // image name
        CHAR     LoadedImageName[256];   // symbol file name
    } IMAGEHLP_MODULE64_V2;


    // SymCleanup()
    typedef BOOL (__stdcall *tSC)( IN HANDLE hProcess );
    tSC pSC;

    // SymFunctionTableAccess64()
    typedef PVOID (__stdcall *tSFTA)( HANDLE hProcess, DWORD64 AddrBase );
    tSFTA pSFTA;

    // SymGetLineFromAddr64()
    typedef BOOL (__stdcall *tSGLFA)( IN HANDLE hProcess, IN DWORD64 dwAddr,
    OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line );
    tSGLFA pSGLFA;

    // SymGetModuleBase64()
    typedef DWORD64 (__stdcall *tSGMB)( IN HANDLE hProcess, IN DWORD64 dwAddr );
    tSGMB pSGMB;

    // SymGetModuleInfo64()
    typedef BOOL (__stdcall *tSGMI)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT IMAGEHLP_MODULE64_V2 *ModuleInfo );
    tSGMI pSGMI;

    // SymGetOptions()
    typedef DWORD (__stdcall *tSGO)( VOID );
    tSGO pSGO;

    // SymGetSymFromAddr64()
    typedef BOOL (__stdcall *tSGSFA)( IN HANDLE hProcess, IN DWORD64 dwAddr,
    OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_SYMBOL64 Symbol );
    tSGSFA pSGSFA;

    // SymInitialize()
    typedef BOOL (__stdcall *tSI)( IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess );
    tSI pSI;

    // SymLoadModule64()
    typedef DWORD64 (__stdcall *tSLM)( IN HANDLE hProcess, IN HANDLE hFile,
    IN PSTR ImageName, IN PSTR ModuleName, IN DWORD64 BaseOfDll, IN DWORD SizeOfDll );
    tSLM pSLM;

    // SymSetOptions()
    typedef DWORD (__stdcall *tSSO)( IN DWORD SymOptions );
    tSSO pSSO;

  // StackWalk64()
    typedef BOOL (__stdcall *tSW)(
    DWORD MachineType,
    HANDLE hProcess,
    HANDLE hThread,
    LPSTACKFRAME64 StackFrame,
    PVOID ContextRecord,
    PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
    PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
    PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
    PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress );
    tSW pSW;

    // UnDecorateSymbolName()
    typedef DWORD (__stdcall WINAPI *tUDSN)( PCSTR DecoratedName, PSTR UnDecoratedName,
    DWORD UndecoratedLength, DWORD Flags );
    tUDSN pUDSN;

    typedef BOOL (__stdcall WINAPI *tSGSP)(HANDLE hProcess, PSTR SearchPath, DWORD SearchPathLength);
    tSGSP pSGSP;


private:
  // **************************************** ToolHelp32 ************************
    #define MAX_MODULE_NAME32 255
    #define TH32CS_SNAPMODULE   0x00000008
    #pragma pack( push, 8 )
    typedef struct tagMODULEENTRY32 {
        DWORD   dwSize;
        DWORD   th32ModuleID;       // This module
        DWORD   th32ProcessID;      // owning process
        DWORD   GlblcntUsage;       // Global usage count on the module
        DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
        BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
        DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
        HMODULE hModule;            // The hModule of this module in th32ProcessID's context
        char    szModule[MAX_MODULE_NAME32 + 1];
        char    szExePath[MAX_PATH];
    } MODULEENTRY32;
    typedef MODULEENTRY32 *  PMODULEENTRY32;
    typedef MODULEENTRY32 *  LPMODULEENTRY32;
    #pragma pack( pop )

    BOOL GetModuleListTH32(HANDLE hProcess, DWORD pid) {
        // CreateToolhelp32Snapshot()
        typedef HANDLE (__stdcall *tCT32S)(DWORD dwFlags, DWORD th32ProcessID);
        // Module32First()
        typedef BOOL (__stdcall *tM32F)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
        // Module32Next()
        typedef BOOL (__stdcall *tM32N)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

        // try both dlls...
        const wchar_t *dllname[] = { TEXT("kernel32.dll"), TEXT("tlhelp32.dll") };
        HINSTANCE hToolhelp = NULL;
        tCT32S pCT32S = NULL;
        tM32F pM32F = NULL;
        tM32N pM32N = NULL;

        HANDLE hSnap;
        MODULEENTRY32 me;
        me.dwSize = sizeof(me);
        BOOL keepGoing;
        size_t i;

        for (i = 0; i<(sizeof(dllname) / sizeof(dllname[0])); i++ ) {
            hToolhelp = LoadLibrary( dllname[i] );
            if (hToolhelp == NULL) {
                continue;
            }
            pCT32S = (tCT32S) GetProcAddress(hToolhelp, "CreateToolhelp32Snapshot");
            pM32F = (tM32F) GetProcAddress(hToolhelp, "Module32First");
            pM32N = (tM32N) GetProcAddress(hToolhelp, "Module32Next");
            if ( (pCT32S != NULL) && (pM32F != NULL) && (pM32N != NULL) ) {
                break; // found the functions!
            }
            FreeLibrary(hToolhelp);
            hToolhelp = NULL;
        }

        if (hToolhelp == NULL) {
            return FALSE;
        }

        hSnap = pCT32S( TH32CS_SNAPMODULE, pid );
        if (hSnap == (HANDLE) -1) {
            return FALSE;
        }

        keepGoing = !!pM32F( hSnap, &me );
        int cnt = 0;
        while (keepGoing){
            this->LoadModule(hProcess, me.szExePath, me.szModule, (DWORD64) me.modBaseAddr, me.modBaseSize);
            cnt++;
            keepGoing = !!pM32N( hSnap, &me );
        }
        CloseHandle(hSnap);
        FreeLibrary(hToolhelp);
        if (cnt <= 0) {
            return FALSE;
        }
        return TRUE;
  }  // GetModuleListTH32

  // **************************************** PSAPI ************************
    typedef struct _MODULEINFO {
        LPVOID lpBaseOfDll;
        DWORD SizeOfImage;
        LPVOID EntryPoint;
    } MODULEINFO, *LPMODULEINFO;

    BOOL GetModuleListPSAPI(HANDLE hProcess) {
        // EnumProcessModules()
        typedef BOOL (__stdcall *tEPM)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded );
        // GetModuleFileNameEx()
        typedef DWORD (__stdcall *tGMFNE)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
        // GetModuleBaseName()
        typedef DWORD (__stdcall *tGMBN)(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize );
        // GetModuleInformation()
        typedef BOOL (__stdcall *tGMI)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize );

        HINSTANCE hPsapi;
        tEPM pEPM;
        tGMFNE pGMFNE;
        tGMBN pGMBN;
        tGMI pGMI;

        DWORD i;
        //ModuleEntry e;
        DWORD cbNeeded;
        MODULEINFO mi;
        HMODULE *hMods = 0;
        char *tt = NULL;
        char *tt2 = NULL;
        const SIZE_T TTBUFLEN = 8096;
        int cnt = 0;

        hPsapi = LoadLibrary( TEXT("psapi.dll") );
        if (hPsapi == NULL) {
            return FALSE;
        }

        pEPM = (tEPM) GetProcAddress( hPsapi, "EnumProcessModules" );
        pGMFNE = (tGMFNE) GetProcAddress( hPsapi, "GetModuleFileNameExA" );
        pGMBN = (tGMFNE) GetProcAddress( hPsapi, "GetModuleBaseNameA" );
        pGMI = (tGMI) GetProcAddress( hPsapi, "GetModuleInformation" );
        if ( (pEPM == NULL) || (pGMFNE == NULL) || (pGMBN == NULL) || (pGMI == NULL) ) {
            // we couldn't find all functions
            FreeLibrary(hPsapi);
            return FALSE;
        }

        hMods = (HMODULE*) malloc(sizeof(HMODULE) * (TTBUFLEN / sizeof HMODULE));
        tt = (char*) malloc(sizeof(char) * TTBUFLEN);
        tt2 = (char*) malloc(sizeof(char) * TTBUFLEN);
        if (!(((hMods == NULL) || (tt == NULL) || (tt2 == NULL)) || ! pEPM( hProcess, hMods, TTBUFLEN, &cbNeeded ) ||  cbNeeded > TTBUFLEN)) {
            for ( i = 0; i < cbNeeded / sizeof hMods[0]; i++ ){
                // base address, size
                pGMI(hProcess, hMods[i], &mi, sizeof mi );
                // image file name
                tt[0] = 0;
                pGMFNE(hProcess, hMods[i], tt, TTBUFLEN );
                // module name
                tt2[0] = 0;
                pGMBN(hProcess, hMods[i], tt2, TTBUFLEN );

                DWORD dwRes = this->LoadModule(hProcess, tt, tt2, (DWORD64) mi.lpBaseOfDll, mi.SizeOfImage);
                if (dwRes != ERROR_SUCCESS)
                this->m_parent->OnDbgHelpErr("LoadModule", dwRes, 0);
                cnt++;
            }
        }

        if (hPsapi != NULL) FreeLibrary(hPsapi);
        if (tt2 != NULL) free(tt2);
        if (tt != NULL) free(tt);
        if (hMods != NULL) free(hMods);

        return cnt != 0;
    }  // GetModuleListPSAPI

    DWORD LoadModule(HANDLE hProcess, LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size) {
        CHAR *szImg = _strdup(img);
        CHAR *szMod = _strdup(mod);
        DWORD result = ERROR_SUCCESS;
        if ( (szImg == NULL) || (szMod == NULL) )
            result = ERROR_NOT_ENOUGH_MEMORY;
        else
        {
            if (pSLM(hProcess, 0, szImg, szMod, baseAddr, size) == 0)
                result = GetLastError();
        }
        ULONGLONG fileVersion = 0;
        if ( (m_parent != NULL) && (szImg != NULL) )
        {
            // try to retrive the file-version:
            if ( (this->m_parent->m_options & StackWalker::RetrieveFileVersion) != 0)
            {
                VS_FIXEDFILEINFO *fInfo = NULL;
                DWORD dwHandle;
                DWORD dwSize = GetFileVersionInfoSizeA(szImg, &dwHandle);
                if (dwSize > 0)
                {
                    LPVOID vData = malloc(dwSize);
                    if (vData != NULL)
                    {
                        if (GetFileVersionInfoA(szImg, dwHandle, dwSize, vData) != 0)
                        {
                            UINT len;
                            TCHAR szSubBlock[] = TEXT("\\");
                            if (VerQueryValue(vData, szSubBlock, (LPVOID*) &fInfo, &len) == 0)
                                fInfo = NULL;
                            else
                            {
                                fileVersion = ((ULONGLONG)fInfo->dwFileVersionLS) + ((ULONGLONG)fInfo->dwFileVersionMS << 32);
                            }
                        }
                        free(vData);
                    }
                }
            }

            // Retrive some additional-infos about the module
            IMAGEHLP_MODULE64_V2 Module;
            const char *szSymType = "-unknown-";
            if (this->GetModuleInfo(hProcess, baseAddr, &Module) != FALSE)
            {
                switch(Module.SymType)
                {
                case SymNone:
                    szSymType = "-nosymbols-";
                    break;
                case SymCoff:
                    szSymType = "COFF";
                    break;
                case SymCv:
                    szSymType = "CV";
                    break;
                case SymPdb:
                    szSymType = "PDB";
                    break;
                case SymExport:
                    szSymType = "-exported-";
                    break;
                case SymDeferred:
                    szSymType = "-deferred-";
                    break;
                case SymSym:
                    szSymType = "SYM";
                    break;
                case 8: //SymVirtual:
                    szSymType = "Virtual";
                    break;
                case 9: // SymDia:
                    szSymType = "DIA";
                    break;
                }
            }
            this->m_parent->OnLoadModule(img, mod, baseAddr, size, result, szSymType, Module.LoadedImageName, fileVersion);
        }
        if (szImg != NULL) free(szImg);
        if (szMod != NULL) free(szMod);
        return result;
    }
public:
    BOOL LoadModules(HANDLE hProcess, DWORD dwProcessId) {
        // first try toolhelp32
        if (GetModuleListTH32(hProcess, dwProcessId)) {
            return true;
        }
        // then try psapi
        return GetModuleListPSAPI(hProcess);
    }


    bool GetModuleInfo(HANDLE hProcess, DWORD64 baseAddr, IMAGEHLP_MODULE64_V2 *pModuleInfo) {
        if(this->pSGMI == NULL){
            SetLastError(ERROR_DLL_INIT_FAILED);
            return FALSE;
        }
        pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V2);
        void *pData = malloc(4096); // reserve enough memory, so the bug in v6.3.5.1 does not lead to memory-overwrites...
        if (pData == NULL) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return FALSE;
        }
        memcpy(pData, pModuleInfo, sizeof(IMAGEHLP_MODULE64_V2));
        if (this->pSGMI(hProcess, baseAddr, (IMAGEHLP_MODULE64_V2*) pData) != FALSE) {
        // only copy as much memory as is reserved...
            memcpy(pModuleInfo, pData, sizeof(IMAGEHLP_MODULE64_V2));
            pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64_V2);
            free(pData);
            return TRUE;
        }
        free(pData);
        SetLastError(ERROR_DLL_INIT_FAILED);
        return FALSE;
    }
};

// #############################################################
StackWalker::StackWalker(DWORD dwProcessId, HANDLE hProcess) {
    this->m_options = OptionsAll;
    this->m_modulesLoaded = FALSE;
    this->m_hProcess = hProcess;
    this->m_sw = new StackWalkerInternal(this, this->m_hProcess);
    this->m_dwProcessId = dwProcessId;
    this->m_szSymPath = NULL;
}
StackWalker::StackWalker(int options, LPCSTR szSymPath, DWORD dwProcessId, HANDLE hProcess) {
    this->m_options = options;
    this->m_modulesLoaded = FALSE;
    this->m_hProcess = hProcess;
    this->m_sw = new StackWalkerInternal(this, this->m_hProcess);
    this->m_dwProcessId = dwProcessId;
    if (szSymPath != NULL) {
        this->m_szSymPath = _strdup(szSymPath);
        this->m_options |= SymBuildPath;
    } else {
        this->m_szSymPath = NULL;
    }
}

StackWalker::~StackWalker() {
    if (m_szSymPath != NULL) {
        free(m_szSymPath);
    }
    m_szSymPath = NULL;
    if (this->m_sw != NULL) {
        delete this->m_sw;
    }
    this->m_sw = NULL;
}

BOOL StackWalker::LoadModules() {
    if (this->m_sw == NULL) {
        SetLastError(ERROR_DLL_INIT_FAILED);
        return FALSE;
    }
    if (m_modulesLoaded != FALSE) {
        return TRUE;
    }

    // Build the sym-path:
    char *szSymPath = NULL;
    if ( (this->m_options & SymBuildPath) != 0) {
        const size_t nSymPathLen = 4096;
        szSymPath = (char*) malloc(nSymPathLen);
        if (szSymPath == NULL) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return FALSE;
        }
        szSymPath[0] = 0;
        // Now first add the (optional) provided sympath:
        if (this->m_szSymPath != NULL) {
            strcat_s(szSymPath, nSymPathLen, this->m_szSymPath);
            strcat_s(szSymPath, nSymPathLen, ";");
        }

        strcat_s(szSymPath, nSymPathLen, ".;");

        const size_t nTempLen = 1024;
        char szTemp[nTempLen];
        // Now add the current folder:
        if (GetCurrentDirectoryA(nTempLen, szTemp) > 0) {
            szTemp[nTempLen-1] = 0;
            strcat_s(szSymPath, nSymPathLen, szTemp);
            strcat_s(szSymPath, nSymPathLen, ";");
        }

        // Now add the path for the main-module:
        if (GetModuleFileNameA(NULL, szTemp, nTempLen) > 0) {
            szTemp[nTempLen-1] = 0;
            for (char *p = (szTemp+strlen(szTemp)-1); p >= szTemp; --p) {
            // locate the rightmost path separator
                if ( (*p == '\\') || (*p == '/') || (*p == ':') ) {
                    *p = 0;
                    break;
                }
            }  // for (search for path separator...)
            if (strlen(szTemp) > 0){
                strcat_s(szSymPath, nSymPathLen, szTemp);
                strcat_s(szSymPath, nSymPathLen, ";");
            }
        }
        if (GetEnvironmentVariableA("_NT_SYMBOL_PATH", szTemp, nTempLen) > 0) {
            szTemp[nTempLen-1] = 0;
            strcat_s(szSymPath, nSymPathLen, szTemp);
            strcat_s(szSymPath, nSymPathLen, ";");
        }
        if (GetEnvironmentVariableA("_NT_ALTERNATE_SYMBOL_PATH", szTemp, nTempLen) > 0) {
            szTemp[nTempLen-1] = 0;
            strcat_s(szSymPath, nSymPathLen, szTemp);
            strcat_s(szSymPath, nSymPathLen, ";");
        }
        if (GetEnvironmentVariableA("SYSTEMROOT", szTemp, nTempLen) > 0) {
            szTemp[nTempLen-1] = 0;
            strcat_s(szSymPath, nSymPathLen, szTemp);
            strcat_s(szSymPath, nSymPathLen, ";");
            // also add the "system32"-folder:
            strcat_s(szTemp, nTempLen, "\\system32");
            strcat_s(szSymPath, nSymPathLen, szTemp);
            strcat_s(szSymPath, nSymPathLen, ";");
        }

        if ( (this->m_options & SymBuildPath) != 0) {
            if (GetEnvironmentVariableA("SYSTEMDRIVE", szTemp, nTempLen) > 0) {
                szTemp[nTempLen-1] = 0;
                strcat_s(szSymPath, nSymPathLen, "SRV*");
                strcat_s(szSymPath, nSymPathLen, szTemp);
                strcat_s(szSymPath, nSymPathLen, "\\websymbols");
                strcat_s(szSymPath, nSymPathLen, "*http://msdl.microsoft.com/download/symbols;");
            } else {
                strcat_s(szSymPath, nSymPathLen, "SRV*c:\\websymbols*http://msdl.microsoft.com/download/symbols;");
            }
        }
    }

  // First Init the whole stuff...
    BOOL bRet = this->m_sw->Init(szSymPath);
    if (szSymPath != NULL)  {
        free(szSymPath);
    }
    szSymPath = NULL;
    if (bRet == FALSE) {
        this->OnDbgHelpErr("Error while initializing dbghelp.dll", 0, 0);
        SetLastError(ERROR_DLL_INIT_FAILED);
        return FALSE;
    }

    bRet = this->m_sw->LoadModules(this->m_hProcess, this->m_dwProcessId);
    if (bRet != FALSE) {
        m_modulesLoaded = TRUE;
    }
    return bRet;
}


// The following is used to pass the "userData"-Pointer to the user-provided readMemoryFunction
// This has to be done due to a problem with the "hProcess"-parameter in x64...
// Because this class is in no case multi-threading-enabled (because of the limitations
// of dbghelp.dll) it is "safe" to use a static-variable
static StackWalker::PReadProcessMemoryRoutine s_readMemoryFunction = NULL;
static LPVOID s_readMemoryFunction_UserData = NULL;

bool StackWalker::ShowCallstack(HANDLE hThread, const CONTEXT *context, PReadProcessMemoryRoutine readMemoryFunction, LPVOID pUserData) {
    CONTEXT c;
    CallstackEntry csEntry;
    IMAGEHLP_SYMBOL64 *pSym = NULL;
    StackWalkerInternal::IMAGEHLP_MODULE64_V2 Module;
    IMAGEHLP_LINE64 Line;
    int frameNum;

    if (m_modulesLoaded == FALSE)
        this->LoadModules();  // ignore the result...

    if (this->m_sw->m_hDbhHelp == NULL)
    {
        SetLastError(ERROR_DLL_INIT_FAILED);
        return FALSE;
    }

    s_readMemoryFunction = readMemoryFunction;
    s_readMemoryFunction_UserData = pUserData;

    c = *context;

    // init STACKFRAME for first call
    STACKFRAME64 s; // in/out stackframe
    memset(&s, 0, sizeof(s));
    DWORD imageType;
#ifdef _M_IX86
    // normally, call ImageNtHeader() and use machine info from PE header
    imageType = IMAGE_FILE_MACHINE_I386;
    s.AddrPC.Offset = c.Eip;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrFrame.Offset = c.Ebp;
    s.AddrFrame.Mode = AddrModeFlat;
    s.AddrStack.Offset = c.Esp;
    s.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
    imageType = IMAGE_FILE_MACHINE_AMD64;
    s.AddrPC.Offset = c.Rip;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrFrame.Offset = c.Rsp;
    s.AddrFrame.Mode = AddrModeFlat;
    s.AddrStack.Offset = c.Rsp;
    s.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
    imageType = IMAGE_FILE_MACHINE_IA64;
    s.AddrPC.Offset = c.StIIP;
    s.AddrPC.Mode = AddrModeFlat;
    s.AddrFrame.Offset = c.IntSp;
    s.AddrFrame.Mode = AddrModeFlat;
    s.AddrBStore.Offset = c.RsBSP;
    s.AddrBStore.Mode = AddrModeFlat;
    s.AddrStack.Offset = c.IntSp;
    s.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

    pSym = (IMAGEHLP_SYMBOL64 *) malloc(sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
    if (!pSym) goto cleanup;  // not enough memory...
    memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
    pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
    pSym->MaxNameLength = STACKWALK_MAX_NAMELEN;

    memset(&Line, 0, sizeof(Line));
    Line.SizeOfStruct = sizeof(Line);

    memset(&Module, 0, sizeof(Module));
    Module.SizeOfStruct = sizeof(Module);

    for (frameNum = 0; ; ++frameNum )
    {
        // get next stack frame (StackWalk64(), SymFunctionTableAccess64(), SymGetModuleBase64())
        // if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
        // assume that either you are done, or that the stack is so hosed that the next
        // deeper frame could not be found.
        // CONTEXT need not to be suplied if imageTyp is IMAGE_FILE_MACHINE_I386!
        if ( ! this->m_sw->pSW(imageType, this->m_hProcess, hThread, &s, &c, myReadProcMem, this->m_sw->pSFTA, this->m_sw->pSGMB, NULL) )
        {
            this->OnDbgHelpErr("StackWalk64", GetLastError(), s.AddrPC.Offset);
            break;
        }

        csEntry.offset = s.AddrPC.Offset;
        csEntry.name[0] = 0;
        csEntry.undName[0] = 0;
        csEntry.undFullName[0] = 0;
        csEntry.offsetFromSmybol = 0;
        csEntry.offsetFromLine = 0;
        csEntry.lineFileName[0] = 0;
        csEntry.lineNumber = 0;
        csEntry.loadedImageName[0] = 0;
        csEntry.moduleName[0] = 0;
        if (s.AddrPC.Offset == s.AddrReturn.Offset)
        {
            this->OnDbgHelpErr("StackWalk64-Endless-Callstack!", 0, s.AddrPC.Offset);
            break;
        }
        if (s.AddrPC.Offset != 0)
        {
            // we seem to have a valid PC
            // show procedure info (SymGetSymFromAddr64())
            if (this->m_sw->pSGSFA(this->m_hProcess, s.AddrPC.Offset, &(csEntry.offsetFromSmybol), pSym) != FALSE)
            {
                // TODO: Mache dies sicher...!
                strcpy_s(csEntry.name, pSym->Name);
                // UnDecorateSymbolName()
                this->m_sw->pUDSN( pSym->Name, csEntry.undName, STACKWALK_MAX_NAMELEN, UNDNAME_NAME_ONLY );
                this->m_sw->pUDSN( pSym->Name, csEntry.undFullName, STACKWALK_MAX_NAMELEN, UNDNAME_COMPLETE );
            }
            else
            {
                this->OnDbgHelpErr("SymGetSymFromAddr64", GetLastError(), s.AddrPC.Offset);
            }

            // show line number info, NT5.0-method (SymGetLineFromAddr64())
            if (this->m_sw->pSGLFA != NULL )
            { // yes, we have SymGetLineFromAddr64()
                if (this->m_sw->pSGLFA(this->m_hProcess, s.AddrPC.Offset, &(csEntry.offsetFromLine), &Line) != FALSE)
                {
                    csEntry.lineNumber = Line.LineNumber;
                    // TODO: Mache dies sicher...!
                    strcpy_s(csEntry.lineFileName, Line.FileName);
                }
                else
                {
                    this->OnDbgHelpErr("SymGetLineFromAddr64", GetLastError(), s.AddrPC.Offset);
                }
            } // yes, we have SymGetLineFromAddr64()

            // show module info (SymGetModuleInfo64())
            if (this->m_sw->GetModuleInfo(this->m_hProcess, s.AddrPC.Offset, &Module ) != FALSE)
            { // got module info OK
                switch ( Module.SymType )
                {
                case SymNone:
                    csEntry.symTypeString = "-nosymbols-";
                    break;
                case SymCoff:
                    csEntry.symTypeString = "COFF";
                    break;
                case SymCv:
                    csEntry.symTypeString = "CV";
                    break;
                case SymPdb:
                    csEntry.symTypeString = "PDB";
                    break;
                case SymExport:
                    csEntry.symTypeString = "-exported-";
                    break;
                case SymDeferred:
                    csEntry.symTypeString = "-deferred-";
                    break;
                case SymSym:
                    csEntry.symTypeString = "SYM";
                    break;
#if API_VERSION_NUMBER >= 9
                case SymDia:
                    csEntry.symTypeString = "DIA";
                    break;
#endif
                case 8: //SymVirtual:
                    csEntry.symTypeString = "Virtual";
                    break;
                default:
                    //_snprintf( ty, sizeof ty, "symtype=%ld", (long) Module.SymType );
                    csEntry.symTypeString = NULL;
                    break;
                }

                // TODO: Mache dies sicher...!
                strcpy_s(csEntry.moduleName, Module.ModuleName);
                csEntry.baseOfImage = Module.BaseOfImage;
                strcpy_s(csEntry.loadedImageName, Module.LoadedImageName);
            } // got module info OK
            else
            {
                this->OnDbgHelpErr("SymGetModuleInfo64", GetLastError(), s.AddrPC.Offset);
            }
        } // we seem to have a valid PC

        CallstackEntryType et = nextEntry;
        if (frameNum == 0)
            et = firstEntry;
        this->OnCallstackEntry(et, csEntry);

        if (s.AddrReturn.Offset == 0)
        {
            this->OnCallstackEntry(lastEntry, csEntry);
            SetLastError(ERROR_SUCCESS);
            break;
        }
    } // for ( frameNum )

cleanup:
    if (pSym) free( pSym );

    if (context == NULL)
        ResumeThread(hThread);

    return TRUE;
}

BOOL __stdcall StackWalker::myReadProcMem(
    HANDLE      hProcess,
    DWORD64     qwBaseAddress,
    PVOID       lpBuffer,
    DWORD       nSize,
    LPDWORD     lpNumberOfBytesRead
    )
{
    if (s_readMemoryFunction == NULL) {
        SIZE_T st;
        BOOL bRet = ReadProcessMemory(hProcess, (LPVOID) qwBaseAddress, lpBuffer, nSize, &st);
        *lpNumberOfBytesRead = (DWORD) st;
        return bRet;
    } else{
        return s_readMemoryFunction(hProcess, qwBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead, s_readMemoryFunction_UserData);
    }
}

void StackWalker::OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion) {
    libraries[(LPVOID)baseAddr] = QString(img);
}

void StackWalker::OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry) {
    QMap<LPVOID, QString>::iterator it = libraries.begin();
    while(entry.offset > (DWORD)it.key() && it != libraries.end()) {
        it++;
    }
    it--;
    QString library = it.value();
    DWORD libAddr = (DWORD)it.key();
    QString binaryName = library.split("\\").last();
    library.replace("dll", "map");
    library.replace("exe", "map");
    QFile file(library);
    if(!file.exists()) {
        QString str;
        buffer += str.sprintf("%p", entry.offset) + ": (" + binaryName + ") " + entry.name + "\n";
        return;
    }
    file.open(QIODevice::ReadOnly);
    DWORD preferredAddress = 0x10000000;

    QString name;
    DWORD offset = 0;
    while(!file.readLine().contains("Rva+Base") && !file.atEnd());
    file.readLine();
    while(!file.atEnd()) {
        QString line = file.readLine();
        QStringList list = line.split(" ",QString::SkipEmptyParts);
        if(list.size() > 3) {
            DWORD addr = list[2].toInt(NULL, 16);
            DWORD actualAddress = preferredAddress + entry.offset - libAddr;
            if(actualAddress < addr) {
                break;
            }
            name = list[1];
            offset =  actualAddress - addr;
        }
    }
    if(file.atEnd()) {
        name = "Some static function";
    }
    file.close();
    QString str;
    buffer += str.sprintf("%p", entry.offset) + ": (" + binaryName + ") " + name.replace("@", ":") + " + " + str.sprintf("%p", offset) + "\n";
}

void StackWalker::OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr) {
    //CHAR buffer[STACKWALK_MAX_NAMELEN];
    //_snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "ERROR: %s, GetLastError: %d (Address: %p)\n", szFuncName, gle, (LPVOID) addr);
    //OnOutput("--");
}

void StackWalker::OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName) {
    CHAR buffer[STACKWALK_MAX_NAMELEN];
    _snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "SymInit: Symbol-SearchPath: '%s', symOptions: %d, UserName: '%s'\n", szSearchPath, symOptions, szUserName);
    // Also display the OS-version
    OSVERSIONINFOEXA ver;
    ZeroMemory(&ver, sizeof(OSVERSIONINFOEXA));
    ver.dwOSVersionInfoSize = sizeof(ver);
    if (GetVersionExA( (OSVERSIONINFOA*) &ver) != FALSE) {
        _snprintf_s(buffer, STACKWALK_MAX_NAMELEN, "OS-Version: %d.%d.%d (%s) 0x%x-0x%x\n",
            ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber,
            ver.szCSDVersion, ver.wSuiteMask, ver.wProductType);
        OnOutput(buffer);
    }
}

void StackWalker::OnOutput(LPCSTR _buffer) {
    buffer.append(_buffer);
}

}

#endif
