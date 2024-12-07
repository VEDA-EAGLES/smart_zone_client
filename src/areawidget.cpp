#include "ui_areawidget.h"
#include "areawidget.h"
#include "drawwidget.h"
#include "streamdisplay.h"

#include <QStackedLayout>
#include <QColorDialog>

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
    stackedLayout->insertWidget(0, display);
    drawWidget->raise();
}

void AreaWidget::init() {
    ui->setupUi(this);
    drawWidget = new DrawWidget();
    stackedLayout = new QStackedLayout(ui->drawAreaWidget);
    stackedLayout->setStackingMode(QStackedLayout::StackAll);
    stackedLayout->addWidget(drawWidget);
    ui->drawAreaWidget->setLayout(stackedLayout);
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

        emit insertArea(area);
        clearWidget();
        emit quit();
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
}

void AreaWidget::clearWidget()
{
    drawWidget->clear();
    ui->areaNameEdit->clear();
    ui->colorPreview->setStyleSheet(QString("border: 1px solid black; background-color: red;"));
    color = Qt::red;
}