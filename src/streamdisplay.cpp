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
    playing = false;
    camera = nullptr;
    setBorder();

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    videoItem = new QGraphicsVideoItem();
    player = new QMediaPlayer(this);
    player->setVideoOutput(videoItem);
    scene->addItem(videoItem);

    connect(videoItem, &QGraphicsVideoItem::nativeSizeChanged, this, &StreamDisplay::fitVideo);
    connect(ui->insertAreaButton, &QPushButton::clicked, this, [=]() {
        emit insertArea(this);
    });
    connect(ui->stopStreamButton, &QPushButton::clicked, this, &StreamDisplay::stopStream);
    updateStatus();
}

StreamDisplay::~StreamDisplay()
{
    delete ui;
}

void StreamDisplay::setBorder()
{
    if (focused) {
        this->setStyleSheet("QWidget#graphicsView {border: 2px solid red;}");
    } else {
        this->setStyleSheet("QWidget#graphicsView {border: 1px solid black;}");
    }
}

void StreamDisplay::playStream(QString uri)
{
    player->setSource(QUrl(uri));
    player->play();
    playing = true;
    updateStatus();
}

void StreamDisplay::playStream(Camera* camera)
{
    this->camera = camera;
    playStream(tr("rtsp://%1:8082/test").arg(camera->ip));
}

void StreamDisplay::stopStream()
{
    this->camera = nullptr;
    player->stop();
    playing = false;
    updateStatus();
}

void StreamDisplay::fitVideo()
{
    QTimer::singleShot(0, this, [=]() {
        ui->graphicsView->fitInView(videoItem);
    });
}

void StreamDisplay::hideStatus()
{
    ui->statusWidget->hide();
}

void StreamDisplay::showStatus()
{
    ui->statusWidget->show();
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

void StreamDisplay::updateStatus()
{
    if (playing) {
        ui->cameraNameLabel->setText((camera)?camera->name:"이름 없음");
        ui->stopStreamButton->setEnabled(true);
        ui->insertAreaButton->setEnabled(true);
    } else {
        ui->cameraNameLabel->setText(tr("연결 안됨"));
        ui->stopStreamButton->setEnabled(false);
        ui->insertAreaButton->setEnabled(false);
    }
}