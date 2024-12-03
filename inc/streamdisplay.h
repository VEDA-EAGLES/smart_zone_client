#ifndef STREAMDISPLAY_H
#define STREAMDISPLAY_H

#include <QWidget>

#include "gststreamthread.h"

class QMediaPlayer;
class QGraphicsScene;
class QGraphicsVideoItem;
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
    void fitVideo();

signals:
    void focusIn(StreamDisplay* widget);
    void videoFrameChanged();

protected:
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
private:
    Ui::StreamDisplay *ui;

    bool focused;
    QMediaPlayer* player;
    QGraphicsScene* scene;
    QGraphicsVideoItem* videoItem;
};

#endif // STREAMDISPLAY_H