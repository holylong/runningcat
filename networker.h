#ifndef NETWORKER_H
#define NETWORKER_H

#ifdef _WIN32
#include <Windows.h>
#include <IPTypes.h>
#include <iphlpapi.h>
#else

#endif

#include <QThread>
#include <QString>
#include <vector>

struct NetworkConn{
    int index{};			//MIB_IFTABLE
    QString description;		//from GetAdapterInfo
    QString description_2;	//from GetIfTable
    unsigned int in_bytes;
    unsigned int out_bytes;
    QString ip_address{ "-.-.-.-" };
    QString subnet_mask{ "-.-.-.-" };
    QString default_gateway{ "-.-.-.-" };
};

class Networker : public QThread
{
    Q_OBJECT
public:
    explicit Networker(QThread *parent = nullptr);

    void GetAdapterInfo(std::vector<NetworkConn>& adapters);

#ifdef _WIN32
    MIB_IFROW GetConnectIfTable(int connection_index);
#else

#endif //_WIN32
    void QtGetIfTable();

protected:
    void run();

    void GetAllTraffic();



signals:
    void ReportNetworker(const QString& in, const QString& out);



signals:

private:
    std::vector<NetworkConn> _conns;
#ifdef _WIN32
    MIB_IFTABLE*             _ifTable;
    DWORD                    _dwSize{};
#else

#endif //_WIN32
    uint64_t                 _in_bytes;
    uint64_t                 _out_bytes;
    uint64_t                 _pre_in_bytes;
    uint64_t                 _pre_out_bytes;
    uint64_t                 _in_speed;
    uint64_t                 _out_speed;
    unsigned short           _count{0};
};

#endif // NETWORKER_H
