#ifndef STREAMDISPLAY_H
#define STREAMDISPLAY_H

#include <QWidget>

#include "gststreamthread.h"
#include "camera.h"

class QMediaPlayer;
class QGraphicsView;
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
    void playStream(Camera* camera);
    void stopStream();
    void fitVideo();
    void hideStatus();
    void showStatus();
    Camera* getCamera();

signals:
    void focusIn(StreamDisplay* display);
    void videoFrameChanged();
    void insertArea(StreamDisplay* display);

protected:
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
private:
    Ui::StreamDisplay *ui;

    bool focused;
    bool playing;
    QMediaPlayer* player;
    QGraphicsScene* scene;
    QGraphicsVideoItem* videoItem;
    Camera* camera;

    void updateStatus();
    void initializePlayer();
};

#endif // STREAMDISPLAY_H