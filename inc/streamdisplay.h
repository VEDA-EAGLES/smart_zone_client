#ifndef STREAMDISPLAY_H
#define STREAMDISPLAY_H

#include <QWidget>

#include "gststreamthread.h"

namespace Ui {
    class StreamDisplay;
} // namespace UI

class StreamDisplay : public QWidget
{
    Q_OBJECT

public:
    StreamDisplay(QWidget *parent = nullptr);
    ~StreamDisplay();
    void setBorder();
    void playStream(QString uri);
    void stopStream();

signals:
    void focusIn(StreamDisplay* widget);

protected:
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
private:
    Ui::StreamDisplay *ui;

    bool focused;
    GstStreamThread* streamThread;
};

#endif // STREAMDISPLAY_H