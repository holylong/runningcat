#include "cpumemo.h"

#ifdef _WIN32
#include <Windows.h>
#endif

CpuMemoProfier::CpuMemoProfier(QObject *parent)
    : QThread(parent)
{

}

CpuMemoProfier::~CpuMemoProfier()
{

}

void CpuMemoProfier::Start()
{

}

void CpuMemoProfier::Stop()
{

}

UINT64 CpuMemoProfier::CalFileTime(const FILETIME& t1, const FILETIME& t2)
{
    UINT64 t1diff = (UINT64)t1.dwHighDateTime << 32 | (UINT64)t1.dwLowDateTime;
    UINT64 t2diff = (UINT64)t2.dwHighDateTime << 32 | (UINT64)t2.dwLowDateTime;

    return (t2diff - t1diff);
}


void CpuMemoProfier::run()
{
    while(!isInterruptionRequested()){
        QThread::msleep(2000);

//        emit ReportCpuUsage(GetCpuUsage()*2);
        emit ReportCpuUsage(GetCpuUsage3());
    }
}

double CpuMemoProfier::GetCpuUsage2() {
    FILETIME idleTime, kernelTime, userTime;

    // 获取系统空闲时间、内核时间和用户时间
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime))
    {
        printf("Failed to get system times.\n");
        return 1;
    }

    // 计算CPU使用率
    UINT64 idleDiff = CalFileTime(lastIdleTime, idleTime);
    UINT64 kernelDiff = CalFileTime(lastKernelTime, kernelTime);
    UINT64 userDiff = CalFileTime(lastUserTime, userTime);


    double cpuUsage = (kernelDiff + userDiff - idleDiff)*100/(kernelDiff + userDiff);
    UINT64 cpuidle = 100.0 * idleDiff/(kernelDiff + userDiff);

    // 打印CPU使用率
    // printf("CPU usage: %.2f%%\n", cpuUsage);

    // 更新上一次的时间
    lastIdleTime = idleTime;
    lastKernelTime = kernelTime;
    lastUserTime = userTime;

    return cpuUsage;
}

__int64 CpuMemoProfier::MsFiletime2Int64(const FILETIME &ftime)
{
  LARGE_INTEGER li;
  li.LowPart = ftime.dwLowDateTime;
  li.HighPart = ftime.dwHighDateTime;
  return li.QuadPart;
}

__int64 CpuMemoProfier::CompareFileTime2(const FILETIME &preTime, const FILETIME &nowTime)
{
  return MsFiletime2Int64(nowTime) - MsFiletime2Int64(preTime);
}

double CpuMemoProfier::GetCpuUsage3()
{
  FILETIME preIdleTime;
  FILETIME preKernelTime;
  FILETIME preUserTime;
  GetSystemTimes (&preIdleTime, &preKernelTime, &preUserTime);

  Sleep (1000);

  FILETIME idleTime;
  FILETIME kernelTime;
  FILETIME userTime;
  GetSystemTimes (&idleTime, &kernelTime, &userTime);

  auto idle = CompareFileTime2 (preIdleTime, idleTime);
  auto kernel = CompareFileTime2 (preKernelTime, kernelTime);
  auto user = CompareFileTime2 (preUserTime, userTime);

  if (kernel + user == 0)
    return 0;

  return 100.0* (kernel + user - idle) / (kernel + user);
}

double CpuMemoProfier::GetCpuUsage()
{
    FILETIME idleTime, kernelTime, userTime;
    double currentPercent = 0.0f;

    if(GetSystemTimes(&idleTime, &kernelTime, &userTime))
    {
        UINT64 currentIdleTime = (UINT64)idleTime.dwHighDateTime << 32 | (UINT64)idleTime.dwLowDateTime;
        UINT64 currentUsedTime = ((UINT64)kernelTime.dwHighDateTime << 32 | (UINT64)kernelTime.dwLowDateTime)
                                    + ((UINT64)userTime.dwHighDateTime << 32 | (UINT64)userTime.dwLowDateTime);

        UINT64 idleTimeChange = currentIdleTime - previousIdleTime;
        UINT64 usedTimeChange = currentUsedTime - previousUsedTime;

        currentPercent = (usedTimeChange - idleTimeChange) * 100.0 / usedTimeChange;

        previousIdleTime = currentIdleTime;
        previousUsedTime = currentUsedTime;
    }

    previousPercent = (previousPercent + currentPercent) / 2;
    dwMilliseconds = static_cast<DWORD>(2000 / (previousPercent + 20));

    return currentPercent;
}
