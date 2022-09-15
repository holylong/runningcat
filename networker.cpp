#include "networker.h"

#ifdef _WIN32
#include <Windows.h>
#include <IPTypes.h>
#include <iphlpapi.h>
#else

#endif

#include <functional>
#include <QString>
#include <QDebug>

Networker::Networker(QThread *parent) : QThread(parent)
{
#ifdef _WIN32
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
#else

#endif //_WIN32
    GetAdapterInfo(_conns);
}

void Networker::run()
{
    while(1){
#ifdef _WIN32
        Sleep(2000);
#else
        sleep(2);
#endif 
        QtGetIfTable();
        GetAllTraffic();
    }
}

void Networker::QtGetIfTable()
{
    int rtn{};
//    rtn = GetIfTable(_ifTable, &_dwSize, FALSE);
//    qDebug() <<"["<< __FUNCTION__<< ":" << __LINE__ << "]" << "rtn:" << rtn << " dwsize:" << _dwSize;
//    int connection_index =  1;
//    if (connection_index >= 0 && connection_index < static_cast<int>(_conns.size()))
//    {
//        int index = _conns[connection_index].index;
//        if (_ifTable != nullptr && index >= 0 && index < _ifTable->dwNumEntries)
//            qDebug() << "["<< __FUNCTION__<< ":" << __LINE__ << "]" <<"index:" << index << " in:" << _ifTable->table[index].dwInOctets;;
//    }
#ifdef _WIN32
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
#else

#endif //_WIN32
//    qDebug() << "["<< __FUNCTION__<< ":" << __LINE__ << "]" << " rtn:" << rtn << " dwsize:" << _dwSize;
}

void Networker::GetAllTraffic()
{
#ifdef _WIN32
    _pre_in_bytes = _in_bytes;
    _pre_out_bytes = _out_bytes;
    _out_bytes = 0;
    _in_bytes = 0;
    qDebug() << "[" << __FUNCTION__ << ":" << __LINE__ << "]";
    for (size_t i{}; i < _conns.size(); i++)
    {
        auto table = GetConnectIfTable(i);
        _in_bytes += table.dwInOctets;
        _out_bytes += table.dwOutOctets;
        qDebug() <<  "[" << "network:" << (const char*)table.bDescr << "]" <<  "_in_bytes:" << table.dwInOctets << " _out_bytes:" <<table.dwOutOctets;
    }

    _in_speed = _in_bytes - _pre_in_bytes;
    _out_speed = _out_bytes - _pre_out_bytes;
//    qDebug() <<"["<< __FUNCTION__<< ":" << __LINE__ << "]" <<"conns:" <<
//               _conns.size() << " in:" << (_in_speed/1000.0f) << " out:" << (_out_speed/1000.0f)  << " counter:" << _count++;
#else

#endif //_WIN32
    QString in="下载:", out="上传:";

    if(_in_speed > 1024*1024*1024){
        in += QString::number(_in_speed / (1024*1024*1024.0f)/ 2, 'f', 2) + " GB/s";
    }else if(_in_speed > 1024*1024){
        in += QString::number(_in_speed / (1024*1024.0f)/ 2, 'f', 2) + " MB/s";
    }else if(_in_speed > 1024){
        in += QString::number(_in_speed / 1024.0f/ 2, 'f', 2) + " KB/s";
    }else{
        in += QString::number(_in_speed /2) + " B/s";
    }

    if(_out_speed > 1024*1024*1024){
        out += QString::number(_out_speed / (1024*1024*1024.0f)/ 2, 'f', 2) + " GB/s";
    }else if(_out_speed > 1024*1024){
        out += QString::number(_out_speed / (1024*1024.0f)/ 2, 'f', 2) + " MB/s";
    }else if(_out_speed > 1024){
        out += QString::number((_out_speed / 1024.0f) / 2, 'f', 2) + " KB/s";
    }else{
        out += QString::number(_out_speed / 2) + "B/s";
    }

    emit ReportNetworker(in, out);
}

#ifdef _WIN32
MIB_IFROW Networker::GetConnectIfTable(int connection_index)
{
    if (connection_index >= 0 && connection_index < static_cast<int>(_conns.size()))
    {
        int index = _conns[connection_index].index;
        if (_ifTable != nullptr && index >= 0 && index < _ifTable->dwNumEntries){
//            qDebug() << "get a table:" << connection_index << " index:" <<index;
            return _ifTable->table[index];
        }
    }
    return MIB_IFROW();
}
#else

#endif //_WIN32

void Networker::GetAdapterInfo(std::vector<NetworkConn>& adapters)
{
#ifdef _WIN32
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
//            connection.index = pIpAdapterInfo->ComboIndex;

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

    //check index
    for(size_t i = 0; i < adapters.size(); i++)
    {
        int index = -1;
        if(!adapters[i].description.isEmpty())
        {
            for(size_t j = 0; j < _ifTable->dwNumEntries; j++)
            {
                qDebug() << "des:" << adapters[i].description << " des2:" << QString((const char*)_ifTable->table[j].bDescr) << " entries:" << _ifTable->dwNumEntries;
                if(adapters[i].description == QString((const char*)_ifTable->table[j].bDescr))
                {
                    qDebug() << "==> fisrt";
                    index = j;
                    break;
                }
                if(index == -1)
                {
                    int tinx = -1;
                    QString descr = (const char*)_ifTable->table[j].bDescr;
                    if(descr.length() > adapters[i].description.length()){
                        tinx = descr.indexOf(adapters[i].description);
                    }else{
                        tinx = adapters[i].description.indexOf(descr);
                    }
                    if(tinx != -1){
                        qDebug() << "==> second tinx:" << tinx;
                        index = j;
                        break;
                    }
                }
            }

            adapters[i].index = index;
            adapters[i].in_bytes = _ifTable->table[index].dwInOctets;
            adapters[i].out_bytes = _ifTable->table[index].dwOutOctets;
            adapters[i].description_2 = (const char*)_ifTable->table[index].bDescr;
        }
    }
#else

#endif //_WIN32
}
