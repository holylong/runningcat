#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedHeight(43);
    setFixedWidth(220);
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowStaysOnTopHint);
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

