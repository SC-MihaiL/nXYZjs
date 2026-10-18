
/*
 * Copyright (C) NGINX, Inc.
 *
 * Small Windows compatibility layer used by the njs core.  This is
 * intentionally limited to APIs needed by the njs engine; the fs module
 * still requires a separate Win32 implementation.
 */


#ifndef _NJS_WINDOWS_H_INCLUDED_
#define _NJS_WINDOWS_H_INCLUDED_


#include <windows.h>
#include <tlhelp32.h>
#include <process.h>
#include <signal.h>
#include <errno.h>
#include <stddef.h>
#include <time.h>


static inline long
njs_win_getppid(void)
{
    HANDLE           snapshot;
    PROCESSENTRY32   entry;
    DWORD            pid, ppid;

    pid = GetCurrentProcessId();
    ppid = 0;
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    entry.dwSize = sizeof(entry);

    if (Process32First(snapshot, &entry)) {
        do {
            if (entry.th32ProcessID == pid) {
                ppid = entry.th32ParentProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);

    return (long) ppid;
}


/*
 * Windows has no POSIX kill().  Preserve the useful same-process case and
 * report an explicit error for attempts to signal another process.
 */
static inline int
njs_win_kill(unsigned long pid, int sig)
{
    if (pid != (unsigned long) GetCurrentProcessId()) {
        errno = ENOSYS;
        return -1;
    }

    if (sig == 0) {
        return 0;
    }

    return raise(sig);
}


static inline struct tm *
njs_win_localtime_r(const time_t *timep, struct tm *result)
{
    return localtime_s(result, timep) == 0 ? result : NULL;
}


static inline long
njs_win_timezone(const struct tm *tm)
{
    long  timezone, dstbias;

    _get_timezone(&timezone);
    _get_dstbias(&dstbias);

    /*
     * The Microsoft CRT stores the standard timezone as seconds west of
     * UTC and the daylight-saving adjustment as a value to add to it.
     * njs_timezone() follows the POSIX tm_gmtoff convention: seconds east
     * of UTC.
     */
    return -timezone - ((tm->tm_isdst > 0) ? dstbias : 0);
}


#define getppid       njs_win_getppid
#define kill          njs_win_kill
#define localtime_r   njs_win_localtime_r


#endif /* _NJS_WINDOWS_H_INCLUDED_ */