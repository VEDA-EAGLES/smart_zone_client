#include "ui_streamdisplay.h"
#include "streamdisplay.h"

#include <QMediaPlayer>
#include <QUrl>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QMediaPlayer>

#include <QTimer>


StreamDisplay::StreamDisplay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StreamDisplay)
{
    ui->setupUi(this);

    focused = false;
    setBorder();

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    videoItem = new QGraphicsVideoItem();
    player = new QMediaPlayer(this);
    player->setVideoOutput(videoItem);
    scene->addItem(videoItem);

    connect(videoItem, &QGraphicsVideoItem::nativeSizeChanged, this, &StreamDisplay::fitVideo);
}

StreamDisplay::~StreamDisplay()
{
    delete ui;
}

void StreamDisplay::setBorder()
{
    if (focused) {
        this->setStyleSheet("QWidget {border: 2px solid red;}");
    } else {
        this->setStyleSheet("QWidget {border: 1px solid black;}");
    }
}

void StreamDisplay::playStream(QString uri)
{
    player->setSource(QUrl(uri));
    player->play();
}

void StreamDisplay::stopStream()
{
    player->stop();
}

void StreamDisplay::fitVideo()
{
    QTimer::singleShot(0, this, [=]() {
        ui->graphicsView->fitInView(videoItem);
    });
}

void StreamDisplay::focusInEvent(QFocusEvent* event)
{
    focused = true;
    setBorder();
    emit focusIn(this);
}

void StreamDisplay::focusOutEvent(QFocusEvent* event)
{
    focused = false;
    setBorder();
}

void StreamDisplay::mousePressEvent(QMouseEvent* event)
{
    setFocus();
}

void StreamDisplay::resizeEvent(QResizeEvent* event)
{
    fitVideo();
}

void StreamDisplay::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::ParentChange) {
        fitVideo();
    }
}