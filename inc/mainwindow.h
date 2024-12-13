#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFuture>
#include "camera.h"
#include "data.h"

class StreamDisplay;
class AreaWidget;
class GraphDisplay;
namespace Ui
{
    class MainWindow;
} // namespace UI


class MainWindow : public QMainWindow
{
    Q_OBJECT
     
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    StreamDisplay* focusedDisplay;
    QMap<StreamDisplay*, QPair<int, int>> displays;

    AreaWidget* areaWidget;

    GraphDisplay* graphDisplay;
    QList<Camera> cameras;

    void init();
    void initStreamDisplay();
    void initAreaWidget();
    void initGraphDisplay();
    void initConnect();
};

#endif // MAINWINDOW_H