

#ifndef PROCESS_MONITOR_H_
#define PROCESS_MONITOR_H_

/* When to use, rewrite */

#include <Windows.h>
#include <stdint.h>

namespace base {
namespace process {
namespace monitor {

uint64_t filetime2utc(const FILETIME& ftime)
{
    LARGE_INTEGER li;
    li.LowPart = ftime.dwLowDateTime;
    li.HighPart = ftime.dwHighDateTime;
    return li.QuadPart;
}
DWORD getProcessNumber()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
}

uint64_t getProcessCPUTime(HANDLE hProcess)
{
    static size_t s_processor_count = -1;
    if (s_processor_count == -1) {
        s_processor_count = getProcessNumber();
    }
    FILETIME proc_creation_time;
    FILETIME proc_exit_time;
    FILETIME proc_kernel_time;
    FILETIME proc_user_time;
    if (!GetProcessTimes(hProcess, &proc_creation_time, &proc_exit_time, &proc_kernel_time, &proc_user_time)) {
        return -1;
    }
    return (uint64_t)(filetime2utc(proc_kernel_time) + filetime2utc(proc_user_time)) / s_processor_count;
}
uint64_t getSystemCPUTime()
{
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    return filetime2utc(now);
}

int getCPUUseage(HANDLE hProcess)
{
    static uint64_t s_last_system_time = 0;
    static uint64_t s_last_proc_time = 0;

    if (0 == s_last_system_time || 0 == s_last_proc_time) {
        s_last_proc_time = getProcessCPUTime(hProcess);
        s_last_system_time = getSystemCPUTime();
        return -1;
    }

    int cpu = -1;
    uint64_t proc_time = getProcessCPUTime(hProcess);
    if (-1 == proc_time)
        return -1;
    uint64_t system_time = getSystemCPUTime();

    uint64_t system_time_delta = system_time - s_last_system_time;
    uint64_t proc_time_delta = proc_time - s_last_proc_time;

    if (proc_time_delta == 0 || 0 == system_time_delta)
        return -1;

    cpu = (int)((proc_time_delta * 100 + system_time_delta / 2) / system_time_delta);  // 四舍五入

    s_last_proc_time = proc_time;
    s_last_system_time = system_time;
    return cpu;
}

};  // namespace monitor

};  // namespace process

};  // namespace base

#endif  // PROCESS_MONITOR_H_