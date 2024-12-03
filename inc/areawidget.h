#ifndef AREAWIDGET_H
#define AREAWIDGET_H

#include <QWidget>
#include "area.h"

class DrawWidget;
class QStackedLayout;
class StreamDisplay;
class QGraphicsView;
class QGraphicsScene;
class QGraphicsProxyWidget;

namespace Ui
{
    class AreaWidget;
} // namespace UI

class AreaWidget : public QWidget
{
    Q_OBJECT
public:
    AreaWidget(QWidget* parent = nullptr);
    ~AreaWidget();
    void showDisplay(StreamDisplay* display);

signals:
    void quit();
    void insertArea(Area area);

private:
    Ui::AreaWidget* ui;
    DrawWidget* drawWidget;
    QStackedLayout* stackedLayout;

    QGraphicsView* graphicsView;
    QGraphicsScene* graphicsScene;
    QGraphicsProxyWidget* drawItem;
};

#endif // AREAWIDGET_H