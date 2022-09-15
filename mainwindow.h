#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>

class QLabel;
class Networker;
class QSystemTrayIcon;
class QMenu;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void SetBackground();
    void InitLayout();
    void InitTrayIcon();

private:
    QPoint _pressPos;

private slots:
    void UpdateNetworker(const QString& in, const QString& out);

private:
    Ui::MainWindow *ui;

    QLabel*   _labelUpload;
    QLabel*   _labelDownload;
    QLabel*   _labelCpu;
    QLabel*   _labelMemory;

    QSystemTrayIcon *_trayIcon{nullptr};
    QMenu           *_trayMenu{nullptr};

    Networker* _netWorker;
};
#endif // MAINWINDOW_H
