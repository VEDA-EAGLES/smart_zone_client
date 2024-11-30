#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class StreamDisplay;
class AreaWidget;
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
};

#endif // MAINWINDOW_H