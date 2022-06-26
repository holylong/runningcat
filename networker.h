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
    MIB_IFROW GetConnectIfTable(int connection_index);

    void QtGetIfTable();

protected:
    void run();

    void GetAllTraffic();



signals:

private:
    std::vector<NetworkConn> _conns;
    MIB_IFTABLE* _ifTable;
    DWORD        _dwSize{};
    uint64_t          _in_bytes;
    uint64_t          _out_bytes;
};

#endif // NETWORKER_H
