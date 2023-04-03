#ifndef _FRAME_THREAD_H_
#define _FRAME_THREAD_H_

#include <QThread>

class FrameThread : public QThread
{
    Q_OBJECT
    public:
        explicit FrameThread(QObject *parent = nullptr);

    void SetFrameRate(int fps){_fps = fps;}

        void Stop(){_stop = true;}

    protected:
        void run() override;

    signals:
        void FrameChanged(int index);

    private:
        int _fps{5};
        bool _stop{false};
        unsigned short _index{0};
};

#endif // _FRAME_THREAD_H_
