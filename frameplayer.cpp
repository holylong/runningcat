#include "frameplayer.h"
#include "fileutil.h"
#include "framethread.h"
#include <QDebug>
#include <QHBoxLayout>

FramePlayerWidget::FramePlayerWidget(QWidget *parent):QWidget(parent)
{
    _label = new QLabel(this);
    _label->setScaledContents(true);
    // setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    // setAttribute(Qt::WA_TranslucentBackground);
    
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(_label);
//    setStyleSheet("background-color:rgb(122,122,122);");
    setLayout(layout);

    _cpuProfier = new CpuMemoProfier();
    _catThread = new FrameThread(this);
    QObject::connect(_catThread, SIGNAL(FrameChanged(int)),  this, SLOT(FrameChangedSlot(int)));
    QObject::connect(_cpuProfier, &CpuMemoProfier::ReportCpuUsage, [=](double cpu){
        emit ReportCpuUsage("CPU:" + QString::number(cpu, 'f', 2) + "%");
        if(SpeedState_Low > cpu){
            _catThread->SetFrameRate(5);
        }else if(SpeedState_Low < cpu
                && SpeedState_Middle > cpu){
            _catThread->SetFrameRate(10);
        }else if(SpeedState_Middle < cpu
                && SpeedState_MiddleHigh > cpu){
            _catThread->SetFrameRate(15);
        }else if(SpeedState_MiddleHigh < cpu
                && SpeedState_High > cpu){
            _catThread->SetFrameRate(20);
        }else if(SpeedState_High < cpu){
            _catThread->SetFrameRate(25);
        }
    });

    _catThread->SetFrameRate(5);
    _catThread->start();
    _cpuProfier->start();
}

FramePlayerWidget::~FramePlayerWidget()
{

}

void FramePlayerWidget::LoadFrame(const QString &path)
{
    for(int i = 0; i < _frameCount; i++){
        QString framePath = path + QString("/cat-running-%1.svg").arg(i);
        if(FileUtil::IsFileExist(framePath)){
            _frames.push_back(QPixmap(framePath).scaled(_frameWidth, _frameHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }

        qDebug() << framePath;
    }
}


void FramePlayerWidget::SetFrameRate(int fps)
{
    _frameRate = fps;
}

void FramePlayerWidget::SetFrameCount(int count)
{
    _frameCount = count;
}

void FramePlayerWidget::SetFrameSize(int width, int height)
{
    _frameHeight = height;
    _frameWidth = width;
    
    setFixedSize(width, height);
    _label->setFixedSize(width, height);
}


void FramePlayerWidget::Play()
{

}

void FramePlayerWidget::FrameChangedSlot(int index)
{
    if(index%_step == 0){
        qDebug() << "FrameChangedSlot:" << " count:" << index << " inx:" << index%5;
    }
    
    _label->setPixmap(_frames[index%5]);
}

void FramePlayerWidget::Pause()
{

}

void FramePlayerWidget::Stop()
{
    _catThread->Stop();
    if(_catThread->isRunning())_catThread->requestInterruption();
    while(_catThread->isRunning())QThread::sleep(1);
}
