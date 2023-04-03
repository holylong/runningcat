#ifndef _FILE_UTIL_H_
#define _FILE_UTIL_H_

#include <QString>
#include <QFile>

class FileUtil
{
    public:
        static bool IsFileExist(const QString &path){
            return QFile::exists(path);
        }
};

#endif // _FILE_UTIL_H_