#include "networker.h"

#ifdef _WIN32
#include <Windows.h>
#include <IPTypes.h>
#include <iphlpapi.h>
#else

#endif

#include <functional>

#include <QDebug>

Networker::Networker(QThread *parent) : QThread(parent)
{
    _ifTable = NULL;
    _dwSize = sizeof(MIB_IFTABLE);
    _ifTable = (MIB_IFTABLE*)malloc(_dwSize);
    int rtn;
    rtn = GetIfTable(_ifTable, &_dwSize, FALSE);
    if (rtn == ERROR_INSUFFICIENT_BUFFER)
    {
        free(_ifTable);
        _ifTable = (MIB_IFTABLE*)malloc(_dwSize);
    }
    GetIfTable(_ifTable, &_dwSize, FALSE);
    GetAdapterInfo(_conns);
}

void Networker::run()
{
    while(1){
        Sleep(2000);
        QtGetIfTable();
        GetAllTraffic();
    }
}

void Networker::QtGetIfTable()
{
    int rtn{};
    rtn = GetIfTable(_ifTable, &_dwSize, FALSE);
    qDebug() << "rtn:" << rtn << " dwsize:" << _dwSize;
    int connection_index =  1;
    if (connection_index >= 0 && connection_index < static_cast<int>(_conns.size()))
    {
        int index = _conns[connection_index].index;
        if (_ifTable != nullptr && index >= 0 && index < _ifTable->dwNumEntries)
            qDebug() << "index:" << index << " in:" << _ifTable->table[index].dwInOctets;;
    }

    auto getLfTable = [&]()
    {
       __try
       {
           rtn = GetIfTable(_ifTable, &_dwSize, FALSE);
       }
       __except (EXCEPTION_EXECUTE_HANDLER)
       {
           free(_ifTable);
           _dwSize = sizeof(MIB_IFTABLE);
           _ifTable = (MIB_IFTABLE*)malloc(_dwSize);
           rtn = GetIfTable(_ifTable, &_dwSize, FALSE);
           if (rtn == ERROR_INSUFFICIENT_BUFFER)
           {
               free(_ifTable);
               _ifTable = (MIB_IFTABLE*)malloc(_dwSize);
           }
           GetIfTable(_ifTable, &_dwSize, FALSE);
       }
    };

    getLfTable();
}

void Networker::GetAllTraffic()
{
    _out_bytes = 0;
    _in_bytes = 0;
    for (size_t i{}; i < _conns.size(); i++)
    {
        auto table = GetConnectIfTable(i);
        _in_bytes += table.dwInOctets;
        _out_bytes += table.dwOutOctets;
    }
    qDebug() << "conns:" << _conns.size() << " in:" << _in_bytes << " out:" << _out_bytes;
}

MIB_IFROW Networker::GetConnectIfTable(int connection_index)
{
    if (connection_index >= 0 && connection_index < static_cast<int>(_conns.size()))
    {
        int index = _conns[connection_index].index;
        if (_ifTable != nullptr && index >= 0 && index < _ifTable->dwNumEntries)
            return _ifTable->table[index];
    }
    return MIB_IFROW();
}

void Networker::GetAdapterInfo(std::vector<NetworkConn>& adapters)
{
    adapters.clear();
    PIP_ADAPTER_INFO pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[sizeof(IP_ADAPTER_INFO)];
    unsigned long stSize = sizeof(IP_ADAPTER_INFO);
    int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);

    if (ERROR_BUFFER_OVERFLOW == nRel)
    {
        delete[] (BYTE*)pIpAdapterInfo;
        pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
        nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
    }

    PIP_ADAPTER_INFO pIpAdapterInfoHead = pIpAdapterInfo;
    if (ERROR_SUCCESS == nRel)
    {
        while (pIpAdapterInfo)
        {
            NetworkConn connection;
            connection.description = pIpAdapterInfo->Description;
            connection.ip_address = pIpAdapterInfo->IpAddressList.IpAddress.String;
            connection.subnet_mask = pIpAdapterInfo->IpAddressList.IpMask.String;
            connection.default_gateway = pIpAdapterInfo->GatewayList.IpAddress.String;

            adapters.push_back(connection);
            pIpAdapterInfo = pIpAdapterInfo->Next;
        }
    }
    if (pIpAdapterInfoHead)
    {
        delete[] (BYTE*)pIpAdapterInfoHead;
    }
    if (adapters.empty())
    {
        NetworkConn connection{};
        connection.description = "no connections";
        adapters.push_back(connection);
    }
}
