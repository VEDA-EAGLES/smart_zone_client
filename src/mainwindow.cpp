#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "streamdisplay.h"
#include "areawidget.h"
#include "graphdisplay.h"

#include "httpclient.h"

#include <QGridLayout>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    init();

    ui->listWidget->addItem("jongheon");
    Camera camera;
    camera.name = "jongheon";
    camera.ip = "192.168.0.107";
    camera.id = 1;
    cameras.append(camera);

    ui->listWidget->addItem("seongmin");
    camera.name = "seongmin";
    camera.ip = "192.168.0.117";
    camera.id = 2;
    cameras.append(camera);

    HTTPCLIENT->getAllCamera();
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
    initGraphDisplay();
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
}

void MainWindow::initGraphDisplay()
{
    graphDisplay = new GraphDisplay(ui->graphPage);
    ui->graphPage->layout()->addWidget(graphDisplay);
}

void MainWindow::initConnect()
{
    connect(ui->deviceButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget_2->setCurrentIndex(0);
        ui->headWidget->show();
        ui->deviceButton->setChecked(true);
        ui->graphButton->setChecked(false);
    });
    connect(ui->graphButton, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget_2->setCurrentIndex(1);
        ui->graphButton->setChecked(true);
        ui->deviceButton->setChecked(false);
    });
    connect(ui->listWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem* item) {
        for (auto& camera : cameras) {
            if (camera.name == item->text()) {
                graphDisplay->setCamera(camera);
                break;
            }
        }
    });
    connect(ui->startStreamButton, &QPushButton::clicked, this, [=]() {
        if (!focusedDisplay) {
            qDebug() << "No focused display";
            return;
        }
        QListWidgetItem* item = ui->listWidget->currentItem();
        for (auto& camera : cameras) {
            if (camera.name == item->text()) {
                focusedDisplay->playStream(&camera);
                break;
            }
        }
    });
    connect(HTTPCLIENT, &HttpClient::allCameraFetched, this, [=](QList<Camera> cameras) {
        this->cameras = cameras;
        ui->listWidget->clear();
        qDebug() << cameras.size();
        for (const auto& camera : cameras) {
            qDebug() << camera.name << camera.ip;
            ui->listWidget->addItem(camera.name);
        }
    });
}