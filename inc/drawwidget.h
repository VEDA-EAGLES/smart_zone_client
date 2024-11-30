#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

#include <QWidget>
#include <QPoint>

namespace Ui
{
    class DrawWidget;
} // namespace UI

class DrawWidget : public QWidget
{
    Q_OBJECT

public:
    DrawWidget(QWidget* parent = nullptr);
    ~DrawWidget();
    QPoint getLeftTop();
    QPoint getRightBottom();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::DrawWidget* ui;

    QPointF relativeStartPoint;
    QPointF relativeEndPoint;
    QPoint startPoint;
    QPoint endPoint;
    bool isDrawing;

    QPoint clampPoint(const QPoint& point);

public slots:
    void clear();
};

#endif // DRAWWIDGET_H