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
    ui->setupUi(this);

    color = Qt::red;

    drawWidget = new DrawWidget();
    stackedLayout = new QStackedLayout(ui->drawAreaWidget);
    stackedLayout->setStackingMode(QStackedLayout::StackAll);
    stackedLayout->addWidget(drawWidget);
    ui->drawAreaWidget->setLayout(stackedLayout);	

    connect(ui->deleteButton, &QPushButton::clicked, drawWidget, &DrawWidget::clear);
    connect(ui->saveButton, &QPushButton::clicked, [=]() {
        QPoint leftTop = drawWidget->getLeftTop();
        QPoint rightBottom = drawWidget->getRightBottom();
        QString areaName = ui->areaNameEdit->text();

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
    });
    connect(ui->backButton, &QPushButton::clicked, [=]() {
        drawWidget->clear();
        ui->areaNameEdit->clear();
        emit quit();
    });
    connect(ui->colorButton, &QPushButton::clicked, [=]() {
        color = QColorDialog::getColor(Qt::red, this);
        if (color.isValid()) {
            drawWidget->setPenColor(color);
        }
    });
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