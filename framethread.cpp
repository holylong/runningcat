#include "framethread.h"

FrameThread::FrameThread(QObject *parent) : QThread(parent)
{

}

void FrameThread::run()
{
    while(_stop == false && !isInterruptionRequested()){
        emit FrameChanged(_index++);
        msleep(1000 / _fps);
    }
}