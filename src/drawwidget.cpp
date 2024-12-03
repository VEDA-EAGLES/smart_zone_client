#include "ui_drawwidget.h"
#include "drawwidget.h"

#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include <QRect>

DrawWidget::DrawWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DrawWidget)
{
    ui->setupUi(this);   
    pen.setColor(Qt::red);
    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    isDrawing = false;
}

DrawWidget::~DrawWidget()
{
    delete ui;
}

QPoint DrawWidget::getLeftTop()
{
    int x = qMin(startPoint.x(), endPoint.x());
    int y = qMin(startPoint.y(), endPoint.y());
    return QPoint(x, y);
}

QPoint DrawWidget::getRightBottom()
{
    int x = qMax(startPoint.x(), endPoint.x());
    int y = qMax(startPoint.y(), endPoint.y());
    return QPoint(x, y);
}

void DrawWidget::setPenColor(const QColor& color)
{
    pen.setColor(color);
}

void DrawWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(pen);
    painter.drawRect(QRect(startPoint, endPoint));
}

void DrawWidget::mousePressEvent(QMouseEvent* event)
{
    startPoint = clampPoint(event->pos());
    endPoint = clampPoint(event->pos());
    relativeStartPoint.setX((double)startPoint.x() / (double)width());
    relativeStartPoint.setY((double)startPoint.y() / (double)height());
    relativeEndPoint.setX((double)endPoint.x() / (double)width());
    relativeEndPoint.setY((double)endPoint.y() / (double)height());
    isDrawing = true;
}

void DrawWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (isDrawing) {
        endPoint = clampPoint(event->pos());
        relativeEndPoint.setX((double)endPoint.x() / (double)width());
        relativeEndPoint.setY((double)endPoint.y() / (double)height());
        update();
    }
}

void DrawWidget::mouseReleaseEvent(QMouseEvent* event)
{
    endPoint = clampPoint(event->pos());
    relativeEndPoint.setX((double)endPoint.x() / (double)width());
    relativeEndPoint.setY((double)endPoint.y() / (double)height());
    isDrawing = false;
    update();
}

void DrawWidget::resizeEvent(QResizeEvent* event)
{
    startPoint.setX(relativeStartPoint.x() * width());
    startPoint.setY(relativeStartPoint.y() * height());
    endPoint.setX(relativeEndPoint.x() * width());
    endPoint.setY(relativeEndPoint.y() * height());
    update();
}

QPoint DrawWidget::clampPoint(const QPoint& point)
{
    int x = point.x();
    int y = point.y();
    x = qMax(0, x);
    x = qMin(width() - 1, x);
    y = qMax(0, y);
    y = qMin(height() - 1, y);
    return QPoint(x, y);
}

// slots
void DrawWidget::clear()
{
    startPoint = QPoint(-1, -1);
    endPoint = QPoint(-1, -1);
    isDrawing = false;
    pen.setColor(Qt::red);
    update();
}