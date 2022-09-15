#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMouseEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QSystemTrayIcon>
#include <QMenu>
#include "networker.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    InitLayout();
    InitTrayIcon();

    _netWorker = new Networker();

    QObject::connect(_netWorker, SIGNAL(ReportNetworker(const QString&, const QString&)), this, SLOT(UpdateNetworker(const QString&, const QString&)));

    _netWorker->start();
}

void MainWindow::InitLayout()
{
   setFixedHeight(41);
    setFixedWidth(230);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: gray; border-radius:20px;");
    //不显示状态栏图标
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint | Qt::SplashScreen);
//    setStyleSheet("background-image:url(:/styles/background.bmp);border-radius: 5px;");
    setStyleSheet("border-radius: 5px;");

    _labelUpload = new QLabel;
    _labelUpload->setText("上传:100M/s");
    _labelUpload->setStyleSheet("color:black; font:10pt;");
    _labelUpload->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    QFont ft;
    ft.setPointSize(11);
    _labelUpload->setFont(ft);

    _labelDownload = new QLabel;
    _labelDownload->setText("下载:10M/s");
    _labelDownload->setStyleSheet("color:black; font:10pt;");
    _labelDownload->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    _labelDownload->setFont(ft);

    QHBoxLayout* trafficLayout = new QHBoxLayout();
    trafficLayout->addWidget(_labelUpload);
    trafficLayout->addWidget(_labelDownload);
    trafficLayout->setMargin(0);

    _labelCpu = new QLabel;
    _labelCpu->setText("CPU:10 %");
    _labelCpu->setStyleSheet("color:black; font:10pt;");
    _labelCpu->setIndent(11);
    _labelCpu->setFont(ft);

    _labelMemory = new QLabel;
    _labelMemory->setText("内存:20 %");
    _labelMemory->setStyleSheet("color:black; font:10pt;");
    _labelMemory->setIndent(15);
    _labelMemory->setFont(ft);

    QHBoxLayout* usedLayout = new QHBoxLayout();
    usedLayout->addWidget(_labelCpu);
    usedLayout->addWidget(_labelMemory);
    usedLayout->setMargin(2);


    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(trafficLayout);
    mainLayout->addLayout(usedLayout);
    mainLayout->setMargin(2);
    ui->centralwidget->setLayout(mainLayout);

    //    ui->centralwidget->setStyleSheet("border-image:url(:/styles/background.bmp);");
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
          qApp->quit();
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
    QPixmap pix;
    pix.load(":/styles/background.bmp");
    QPainter painter(this);
    painter.drawPixmap(pix.rect(), pix);
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
    delete ui;
}

