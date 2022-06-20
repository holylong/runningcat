#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMouseEvent>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedHeight(41);
    setFixedWidth(230);
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint);
//    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("border-radius: 5px;");
//    setAttribute(Qt::WA_TranslucentBackground);

    QLabel *labelUpload = new QLabel;
    labelUpload->setText("上传:100M/s");
    labelUpload->setStyleSheet("color:black;");
    labelUpload->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    QFont ft;
    ft.setPointSize(11);
    labelUpload->setFont(ft);

    QLabel *labelDownload = new QLabel;
    labelDownload->setText("下载:10M/s");
    labelDownload->setStyleSheet("color:black;");
    labelDownload->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    labelDownload->setFont(ft);

    QHBoxLayout* trafficLayout = new QHBoxLayout();
    trafficLayout->addWidget(labelUpload);
    trafficLayout->addWidget(labelDownload);
    trafficLayout->setMargin(0);

    QLabel *labelCpu = new QLabel;
    labelCpu->setText("CPU:10 %");
    labelCpu->setStyleSheet("color:black;");
    labelCpu->setIndent(11);
    labelCpu->setFont(ft);

    QLabel *labelMemory = new QLabel;
    labelMemory->setText("内存:20 %");
    labelMemory->setStyleSheet("color:black;");
    labelMemory->setIndent(11);
    labelMemory->setFont(ft);

    QHBoxLayout* usedLayout = new QHBoxLayout();
    usedLayout->addWidget(labelCpu);
    usedLayout->addWidget(labelMemory);
    usedLayout->setMargin(2);


    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(trafficLayout);
    mainLayout->addLayout(usedLayout);
    mainLayout->setMargin(2);
    ui->centralwidget->setLayout(mainLayout);
    ui->centralwidget->setStyleSheet("image:url(./styles/background.bmp);");
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

