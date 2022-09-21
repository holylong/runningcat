#ifndef _SYSTEM_UTIL_H_
#define _SYSTEM_UTIL_H_

#include <QString>
#include <QSettings>
#include <QFileInfo>
#include <QDir>

namespace feiker{
  #define AUTO_RESTART "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
  void setProcessAutoRun(const QString& path, bool bautostart)
  {
    QSettings settings(AUTO_RESTART, QSettings::NativeFormat);

    QFileInfo fi(path);
    QString name = fi.baseName();

    QString older = settings.value(name).toString();
    QString newer = QDir::toNativeSeparators(path);

    if(bautostart){
      if(older != newer)settings.setValue(name, path);
    }else settings.remove(name);
  }
}

#endif //_SYSTEM_UTIL_H_
