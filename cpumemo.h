#ifndef _CPU_MEMO_H_
#define _CPU_MEMO_H_

#include <QThread>

#ifdef _WIN32
#include <Windows.h>
#else
typedef unsigned long long UINT64;
typedef unsigned long DWORD;
typedef unsigned long long ULONGLONG;
typedef unsigned long long ULARGE_INTEGER;
#endif

class CpuMemoProfier : public QThread
{
    Q_OBJECT
public:
    CpuMemoProfier(QObject *parent = nullptr);
    ~CpuMemoProfier();

protected:
    void run() override;

    void Start();
    void Stop();
private:
    __int64 MsFiletime2Int64(const FILETIME &ftime);
    __int64 CompareFileTime2(const FILETIME &preTime, const FILETIME &nowTime);
    UINT64 CalFileTime(const FILETIME& t1, const FILETIME& t2);

public:
    double GetCpuUsage3();
    double GetCpuUsage2();
    double GetCpuUsage();

signals:
    void ReportCpuUsage(double);

private:
    double previousPercent;
    UINT64 previousIdleTime;
    UINT64 previousUsedTime;
    DWORD dwMilliseconds;

    FILETIME lastIdleTime{0};
    FILETIME lastKernelTime{0};
    FILETIME lastUserTime{0};
};

#endif // _CPU_MEMO_H_
