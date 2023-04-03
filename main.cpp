#include "mainwindow.h"
#include <QFile>
#include <QApplication>
#include <QDebug>
#ifndef _WIN32
#include <unistd.h>  //fork
#endif

#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

#include <systemutil.h>

void static loadStyleSheet(QString strPath)
{
    //设置编码格式
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("gb2312"));
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("gb2312"));
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("gb2312"));
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QFile file(strPath);
    file.open(QIODevice::ReadOnly);
    QString cstyle = file.readAll();
    qApp->setStyleSheet(cstyle);
}

int main(int argc, char *argv[])
{
    qDebug() << "running cat start success";
    QApplication app(argc, argv);
    // loadStyleSheet(":/styles/MainStyle.qss");
    app.setWindowIcon(QIcon(":/resources/win32.ico"));
    // auto restart
    feiker::setProcessAutoRun(app.applicationFilePath(), true);
    MainWindow w;
    w.show();
    return app.exec();
}
