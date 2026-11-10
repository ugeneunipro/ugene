/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2026 UniPro <ugene@unipro.ru>
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

#include <windows.h>

#include <dbghelp.h>

#include <cstdio>
#include <cstdlib>
#include <string>

static int fail(const char* operation) {
    fprintf(stderr, "%s failed: Win32 error=%lu\n", operation, GetLastError());
    return 1;
}

int wmain(int argc, wchar_t** argv) {
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
    if (argc != 3) {
        fprintf(stderr, "Usage: CoreTestsDump PID dump-path\n");
        return 1;
    }
    wchar_t* end = nullptr;
    unsigned long pid = wcstoul(argv[1], &end, 10);
    if (pid == 0 || *end != L'\0') {
        fprintf(stderr, "Invalid process ID\n");
        return 1;
    }
    printf("Opening process %lu\n", pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (process == nullptr) {
        return fail("OpenProcess");
    }
    std::wstring partialPath = std::wstring(argv[2]) + L".partial";
    HANDLE file = CreateFileW(partialPath.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        int result = fail("CreateFile");
        CloseHandle(process);
        return result;
    }
    printf("Writing minidump\n");
    // Stack memory and thread/module metadata are sufficient for hang analysis; avoid a full heap dump.
    auto flags = static_cast<MINIDUMP_TYPE>(MiniDumpWithThreadInfo | MiniDumpWithFullMemoryInfo | MiniDumpWithUnloadedModules);
    BOOL written = MiniDumpWriteDump(process, pid, file, flags, nullptr, nullptr, nullptr);
    int result = written ? 0 : fail("MiniDumpWriteDump");
    if (written && !FlushFileBuffers(file)) {
        result = fail("FlushFileBuffers");
    }
    CloseHandle(file);
    CloseHandle(process);
    if (result == 0 && !MoveFileExW(partialPath.c_str(), argv[2], MOVEFILE_REPLACE_EXISTING)) {
        result = fail("MoveFileEx");
    }
    if (result == 0) {
        printf("Minidump saved\n");
    }
    return result;
}
