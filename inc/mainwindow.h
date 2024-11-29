#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class StreamDisplay;
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

    StreamDisplay* focuesdDisplay;
};

#endif // MAINWINDOW_H