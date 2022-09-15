#include "mainwindow.h"
#include <QFile>
#include <QApplication>

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
    QApplication app(argc, argv);
//    loadStyleSheet(":/styles/MainStyle.qss");
    app.setWindowIcon(QIcon(":/resources/win32.ico"));
    MainWindow w;
    w.show();
    return app.exec();
}
