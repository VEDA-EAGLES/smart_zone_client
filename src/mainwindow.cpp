#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "streamdisplay.h"
#include "areawidget.h"

#include "httpclient.h"

#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    focusedDisplay = NULL;
    
    // StreamDisplay
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            StreamDisplay* streamDisplay = new StreamDisplay(ui->displayWidget);
            displays[streamDisplay] = qMakePair(i, j);
            ui->gridLayout->addWidget(streamDisplay, i, j);
            connect(streamDisplay, &StreamDisplay::focusIn, this, [this](StreamDisplay* widget) {
                focusedDisplay = widget;
            });

            streamDisplay->playStream(tr("rtsp://210.99.70.120:1935/live/cctv00%1.stream").arg(i * 2 + j + 1));
            // streamDisplay->playStream(tr("C:/Users/sm136/Downloads/test.mp4"));
        }
    }
    ui->gridLayout->setColumnStretch(0, 1);
    ui->gridLayout->setColumnStretch(1, 1);
    ui->gridLayout->setRowStretch(0, 1);
    ui->gridLayout->setRowStretch(1, 1);
    

    // AreaWidget
    areaWidget = new AreaWidget(ui->areaInsertPage);
    ui->areaInsertPage->layout()->addWidget(areaWidget);

    connect(areaWidget, &AreaWidget::quit, this, [=]() {
        QGridLayout* layout = qobject_cast<QGridLayout*>(ui->displayWidget->layout());
        if (layout) {
            layout->addWidget(focusedDisplay, displays[focusedDisplay].first, displays[focusedDisplay].second);
            focusedDisplay->show();
        }
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->insertAreaButton, &QPushButton::clicked, this, [=]() {
        if (focusedDisplay) {
            areaWidget->showDisplay(focusedDisplay);
            ui->stackedWidget->setCurrentIndex(2);
        }
    });
    connect(areaWidget, &AreaWidget::insertArea, this, [=](Area area) {
        // httpclient로 area insert 요청
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    for (auto& display: displays.keys()) {
        delete display;
    }
    delete areaWidget;
}
