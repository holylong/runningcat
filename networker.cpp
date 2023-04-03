#include "networker.h"

#ifdef _WIN32
#include <Windows.h>
#include <IPTypes.h>
#include <iphlpapi.h>
#else
#include <fstream>
#include <QFile>
#include <QTextStream>
#endif
#include <iostream>
#include <functional>
#include <QString>
#include <QDebug>
#include <stringutil.h>

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
    //read /proc/net/dev
    _conns.clear();
    QFile file(_netPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString line = in.readLine();
    while(!line.isNull()){
        process_line(line);
        line = in.readLine();
    }
    file.close();
#endif //_WIN32
}

void Networker::process_line(const QString& line)
{
    QStringList items = line.split(" ");
    items.removeAll("");
#if 1
    if(items.at(0) == "eno1:" 
    || items.at(0) == "eth1:" 
    || feiker::StringUtil::StringFuzzySearch(items.at(0), "eno")
    || feiker::StringUtil::StringFuzzySearch(items.at(0), "eth")){
        //recv index 1
        NetworkConn conn;
        conn.description = items.at(0);
        conn.in_bytes = items.at(1).toULong();
        //send index 9
        conn.out_bytes = items.at(9).toULong();

        _conns.push_back(conn);
    }
#else
    NetworkConn conn;
    conn.description = items.at(0);
    conn.in_bytes = items.at(1).toULong();
    //send index 9
    conn.out_bytes = items.at(9).toULong();

    _conns.push_back(conn);
#endif
}

void Networker::GetAllTraffic()
{
    _pre_in_bytes = _in_bytes;
    _pre_out_bytes = _out_bytes;
    _out_bytes = 0;
    _in_bytes = 0;
#ifdef _WIN32
#ifdef MS_DEBUG
    qDebug() << "[" << __FUNCTION__ << ":" << __LINE__ << "]";
#endif //MS_DEBUG
    for (size_t i{}; i < _conns.size(); i++)
    {
        auto table = GetConnectIfTable(i);
        _in_bytes += table.dwInOctets;
        _out_bytes += table.dwOutOctets;
        #ifdef MS_DEBUG
        qDebug() <<  "[" << "network:" << (const char*)table.bDescr << "]" <<  "_in_bytes:" << table.dwInOctets << " _out_bytes:" <<table.dwOutOctets;
        #endif //MS_DEBUG
    }
    _in_speed = _in_bytes - _pre_in_bytes;
    _out_speed = _out_bytes - _pre_out_bytes;
#else
    for (size_t i{}; i < _conns.size(); i++)
    {
        _in_bytes += _conns[i].in_bytes;
        _out_bytes += _conns[i].out_bytes;
    }
    // _in_speed = (_in_bytes - _pre_in_bytes)/2;
    // _out_speed = (_out_bytes - _pre_out_bytes)/2;
    _in_speed = _in_bytes - _pre_in_bytes;
    _out_speed = _out_bytes - _pre_out_bytes;
#endif //_WIN32

//    qDebug() <<"["<< __FUNCTION__<< ":" << __LINE__ << "]" <<"conns:" <<
//               _conns.size() << " in:" << (_in_speed/1000.0f) << " out:" << (_out_speed/1000.0f)  << " counter:" << _count++;
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
        out += QString::number(_out_speed / 2) + " B/s";
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
