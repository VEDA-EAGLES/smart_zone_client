#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "streamdisplay.h"
#include "areawidget.h"

#include "httpclient.h"

#include "data.h"

#include <QGridLayout>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
    for (auto& display: displays.keys()) {
        delete display;
    }
    delete areaWidget;
}

void MainWindow::init()
{
    ui->setupUi(this);
    focusedDisplay = NULL;

    initStreamDisplay();
    initAreaWidget();
    initConnect();
}

void MainWindow::initStreamDisplay()
{
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            StreamDisplay* streamDisplay = new StreamDisplay(ui->displayWidget);
            displays[streamDisplay] = qMakePair(i, j);
            ui->gridLayout->addWidget(streamDisplay, i, j);
            connect(streamDisplay, &StreamDisplay::focusIn, this, [this](StreamDisplay* display) {
                focusedDisplay = display;
            });
            connect(streamDisplay, &StreamDisplay::insertArea, this, [=](StreamDisplay* display) {
                focusedDisplay = display;
                if (!focusedDisplay) {
                    qDebug() << "No focused display";
                    return;
                }
                focusedDisplay->hideStatus();
                areaWidget->showDisplay(focusedDisplay);
                ui->stackedWidget->setCurrentIndex(1);
                ui->headWidget->hide();
            });
            
            // streamDisplay->playStream(tr("rtsp://210.99.70.120:1935/live/cctv00%1.stream").arg(i * 2 + j + 1));
            streamDisplay->playStream(tr("C:/Users/sm136/Downloads/test2.mp4"));
            // streamDisplay->playStream(tr("rtsp://192.168.0.107:8082/test"));
        }
    }
    ui->gridLayout->setColumnStretch(0, 1);
    ui->gridLayout->setColumnStretch(1, 1);
    ui->gridLayout->setRowStretch(0, 1);
    ui->gridLayout->setRowStretch(1, 1);
}

void MainWindow::initAreaWidget()
{
    areaWidget = new AreaWidget(ui->areaInsertPage);
    ui->areaInsertPage->layout()->addWidget(areaWidget);

    connect(areaWidget, &AreaWidget::quit, this, [=]() {
        QGridLayout* layout = qobject_cast<QGridLayout*>(ui->displayWidget->layout());
        if (layout) {
            layout->addWidget(focusedDisplay, displays[focusedDisplay].first, displays[focusedDisplay].second);
            focusedDisplay->show();
            focusedDisplay->showStatus();
            ui->stackedWidget->setCurrentIndex(0);
            ui->headWidget->show();
        }
    });
    connect(areaWidget, &AreaWidget::insertArea, this, [=](Area area) {
        // httpclient로 area insert 요청
    });
}

void MainWindow::initConnect()
{
    connect(ui->deviceButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget_2->setCurrentIndex(0);
        ui->headWidget->show();
    });
    connect(ui->graphButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget_2->setCurrentIndex(1);
    });
}