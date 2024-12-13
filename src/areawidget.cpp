#include "ui_areawidget.h"
#include "areawidget.h"
#include "drawwidget.h"
#include "streamdisplay.h"
#include "httpclient.h"

#include <QStackedLayout>
#include <QColorDialog>
#include <QMessageBox>
AreaWidget::AreaWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AreaWidget)
{
    init();
}

AreaWidget::~AreaWidget()
{
    delete ui;
}

void AreaWidget::showDisplay(StreamDisplay* display)
{
    drawWidget = new DrawWidget();
    stackedLayout->addWidget(display);
    stackedLayout->addWidget(drawWidget);
    drawWidget->raise();
    focusedDisplay = display;
}

void AreaWidget::init() {
    ui->setupUi(this);
    drawWidget = new DrawWidget();
    stackedLayout = new QStackedLayout(ui->drawAreaWidget);
    stackedLayout->setStackingMode(QStackedLayout::StackAll);
    ui->drawAreaWidget->setLayout(stackedLayout);
    focusedDisplay = nullptr;
    initConnect();
    clearWidget();
}

void AreaWidget::initConnect() {
    connect(ui->saveButton, &QPushButton::clicked, [=]() {
        QPoint leftTop = drawWidget->getLeftTop();
        QPoint rightBottom = drawWidget->getRightBottom();
        QString areaName = ui->areaNameEdit->text();

        if (areaName.isEmpty()) {
            ui->areaNameEdit->setFocus();
            return;
        }

        if (leftTop.x() == rightBottom.x() || leftTop.y() == rightBottom.y()) {
            return;
        }
        int cameraWidget = 640;
        int cameraHeight = 480;
        int drawWidgetWidth = drawWidget->width();
        int drawWidgetHeight = drawWidget->height();
        int x = leftTop.x() * cameraWidget / drawWidgetWidth;
        int y = leftTop.y() * cameraHeight / drawWidgetHeight;
        int width = rightBottom.x() * cameraWidget / drawWidgetWidth - x;
        int height = rightBottom.y() * cameraHeight / drawWidgetHeight - y;
        
        Area area;
        area.name = areaName;
        area.x = x;
        area.y = y;
        area.width = width;
        area.height = height;
        area.color = color.name();

        qDebug() << area.name << area.x << area.y << area.width << area.height << area.color;

        HTTPCLIENT->insertArea(*(focusedDisplay->getCamera()), area);
        ui->saveButton->setEnabled(false);
        ui->backButton->setEnabled(false);
    });
    connect(ui->backButton, &QPushButton::clicked, [=]() {
        clearWidget();
        emit quit();
    });
    connect(ui->colorButton, &QPushButton::clicked, [=]() {
        color = QColorDialog::getColor(Qt::red, this);
        if (!color.isValid()) {
            qDebug() << "Invalid color";
            return;
        }
        drawWidget->setPenColor(color);
        ui->colorPreview->setStyleSheet(QString("border: 1px solid black; background-color: %1;").arg(color.name()));
    });
    connect(HTTPCLIENT, &HttpClient::areaInserted, this, [=]() {
        clearWidget();
        emit quit();
        
        ui->saveButton->setEnabled(true);
        ui->backButton->setEnabled(true);
    });
    connect(HTTPCLIENT, &HttpClient::areaInsertFailedByDuplicateName, this, [=]() {
        // alert error
        QMessageBox msg;
        msg.setText("이미 존재하는 영역 이름입니다.");
        msg.exec();
        ui->areaNameEdit->setFocus();
        
        ui->saveButton->setEnabled(true);
        ui->backButton->setEnabled(true);
    });
    connect(HTTPCLIENT, &HttpClient::areaInsertFailed, this, [=]() {
        // alert error
        QMessageBox msg;
        msg.setText("영역 추가에 실패했습니다.");
        msg.exec();
        
        ui->saveButton->setEnabled(true);
        ui->backButton->setEnabled(true);
    });
}

void AreaWidget::clearWidget()
{
    drawWidget->deleteLater();
    ui->areaNameEdit->clear();
    ui->colorPreview->setStyleSheet(QString("border: 1px solid black; background-color: red;"));
    color = Qt::red;
    focusedDisplay = nullptr;
}