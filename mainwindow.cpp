#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMouseEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QPainter>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QDebug>

#include "networker.h"
#include "frameplayer.h"
#include "framethread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    InitLayout();
    InitTrayIcon();

    _netWorker = new Networker();

    QObject::connect(_netWorker, SIGNAL(ReportNetworker(const QString&, const QString&)), this, SLOT(UpdateNetworker(const QString&, const QString&)));
    QObject::connect(_catWidget, SIGNAL(ReportCpuUsage(const QString&)), this, SLOT(UpdateCpuUsage(const QString&)));

    _netWorker->start();
}

void MainWindow::UpdateCpuUsage(const QString& msg)
{
    // qDebug() << "UpdateCpuUsage:" << msg;
    _labelCpu->setText(msg);
}

void MainWindow::InitLayout()
{
    setFixedHeight(50);
    setFixedWidth(271);
    //不显示状态栏图标
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint | Qt::SplashScreen);
    setAttribute(Qt::WA_TranslucentBackground);

    _labelUpload = new QLabel;
    _labelUpload->setText("上传:100M/s");
    _labelUpload->setStyleSheet("color:black; font:10pt;");
    _labelUpload->setAlignment(Qt::AlignLeft);
    QFont ft;
    ft.setPointSize(11);
    ft.setBold(true);
    ft.setWeight(75);
    _labelUpload->setFont(ft);

    _labelDownload = new QLabel;
    _labelDownload->setText("下载:10M/s");
    _labelDownload->setStyleSheet("color:black; font:10pt;");
    _labelDownload->setAlignment(Qt::AlignLeft);
    _labelDownload->setFont(ft);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->addWidget(_labelUpload, 0, 0);
    gridLayout->addWidget(_labelDownload, 0, 1);

    _labelCpu = new QLabel;
    _labelCpu->setText("CPU:10 %");
    _labelCpu->setStyleSheet("color:black; font:10pt;");
    _labelCpu->setAlignment(Qt::AlignLeft);
    _labelCpu->setFont(ft);

    _labelMemory = new QLabel;
    _labelMemory->setText("内存:20 %");
    _labelMemory->setStyleSheet("color:black; font:10pt;");
    _labelMemory->setAlignment(Qt::AlignLeft);
    _labelMemory->setFont(ft);

    gridLayout->addWidget(_labelCpu, 1, 0);
    gridLayout->addWidget(_labelMemory, 1, 1);

    _catWidget = new FramePlayerWidget(this);
    _catWidget->SetFrameCount(5);
    _catWidget->SetFrameSize(50, 45);
    _catWidget->LoadFrame(":/resources/frames");

    QHBoxLayout* mainLayout = new QHBoxLayout();
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(_catWidget);
    mainLayout->addLayout(vLayout);
    mainLayout->addLayout(gridLayout);

//    ui->centralwidget->setStyleSheet("background-color: gray; ");
    ui->centralwidget->setFixedHeight(50);
    ui->centralwidget->setFixedWidth(271);
    ui->centralwidget->setLayout(mainLayout);

    // ui->centralwidget->setStyleSheet("background-image:url(:/styles/background.bmp); border-radius:5px;");
    // ui->centralwidget->setStyleSheet(QString::fromUtf8("QMainWindow#MainWindow{border-image:url(:/styles/background.bmp); border-radius:5px;}"));
    // setStyleSheet(QString::fromUtf8("QMainWindow {border-image:url(:/styles/background.bmp); border-radius:5px;}"));
    // setStyleSheet("background-image:url(:/styles/background.bmp); border-radius: 20px;");
    // setStyleSheet("background-color: gray; ");
    // ui->centralwidget->setStyleSheet("border-image:url(:/styles/background.bmp);");
    setStyleSheet("QWidget#centralwidget{border-image:url(:/styles/background.bmp); border-radius:5px;}");
}

void MainWindow::InitTrayIcon()
{
    _trayIcon = new QSystemTrayIcon(this);
    _trayIcon->setIcon(QIcon(":/resources/win32.ico"));
    _trayIcon->setToolTip("RunningCat");
    _trayMenu = new QMenu(this);
    _trayMenu->addAction(tr("显示RunningCat窗口"),this,[=](){
          this->show();
          this->activateWindow();
     });
    _trayMenu->addAction(tr("退出RunningCat"),this,[=](){
        qDebug() << "quit";
        QWidget::close();
        //   qApp->quit();
     });
    _trayIcon->setContextMenu(_trayMenu);
    _trayIcon->show();
}

void MainWindow::UpdateNetworker(const QString& in, const QString& out)
{
    _labelUpload->setText(out);
    _labelDownload->setText(in);
}

/**
 * @brief MainWindow::SetBackground
 * set background image
 */
void MainWindow::SetBackground()
{
#if 0
    QPixmap pix;
    pix.load(":/styles/background.bmp");
    setAutoFillBackground(true);
    QPalette pal(palette());
    pal.setBrush(QPalette::Window, QBrush(pix.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
    setPalette(pal);
#elif 0
    QPixmap pix;
    pix.load(":/styles/background.bmp");
    QPalette pal;
    pal.setBrush(QPalette::Window, QBrush(pix));
    setPalette(pal);
#elif 1
    QPixmap pix;
    pix.load(":/styles/background.bmp");
    QPalette pal;
    pal.setBrush(backgroundRole(), QBrush(pix));
    setPalette(pal);
#endif
}

void MainWindow::paintEvent(QPaintEvent *event)
{
#if 0
    QPixmap pix;
    pix.load(":/styles/background.bmp");
    QPainter painter(this);
    painter.drawPixmap(pix.rect(), pix);
#endif
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    _pressPos = event->globalPos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->globalX() - _pressPos.x();
    int dy = event->globalY() - _pressPos.y();

    _pressPos = event->globalPos();
    move(x()+dx, y()+dy);
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow()";
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWindow::CloseEvent";
    _catWidget->Stop();
    qDebug() << "MainWindow::CloseEvent 1";
    qApp->quit();
}
