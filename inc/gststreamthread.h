#ifndef GSTSTREAMTHREAD_H
#define GSTSTREAMTHREAD_H

#include <QWidget>

namespace Ui {
    class GstStreamThread;
} // namespace UI

class GstStreamThread : public QWidget
{
    Q_OBJECT

public:
    GstStreamThread(QWidget *parent = nullptr);
    ~GstStreamThread();
    void gst_test();

private:
    Ui::GstStreamThread *ui;
};

#endif // GSTSTREAMTHREAD_H