#ifndef _FRAME_PLAYER_H_
#define _FRAME_PLAYER_H_

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QVector>
#include <QLabel>

#include "cpumemo.h"

class FrameThread;

class FramePlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FramePlayerWidget(QWidget *parent = 0);
    ~FramePlayerWidget();

    void LoadFrame(const QString &path);

    void SetFrameRate(int fps);
    void SetFrameCount(int count);
    void SetFrameSize(int width, int height);


    void Play();
    void Pause();
    void Stop();

signals:
    void ReportCpuUsage(const QString&);

protected slots:
        void FrameChangedSlot(int index);

    private:
    int _frameRate;
    int _frameCount;
    int _frameWidth;
    int _frameHeight;

    int     _step{60};

    QVector<QPixmap> _frames;
    QLabel           *_label;

    CpuMemoProfier       *_cpuProfier;
    FrameThread      *_catThread;

    double SpeedState_Low = 10.0f;
    double SpeedState_Middle = 30.0f;
    double SpeedState_MiddleHigh = 60.0f;
    double SpeedState_High = 90.0f;
};

#endif // _FRAME_PLAYER_H_