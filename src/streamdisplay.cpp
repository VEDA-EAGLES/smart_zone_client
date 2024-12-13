#include "ui_streamdisplay.h"
#include "streamdisplay.h"
#include "httpclient.h"

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
    connect(player, &QMediaPlayer::errorOccurred, this, [=](QMediaPlayer::Error error) {
        qDebug() << "Error occurred: " << error;
    });
    connect(ui->deleteAllAreaButton, &QPushButton::clicked, this, [=]() {
        HTTPCLIENT->deleteAreaAll(*camera);
    });
    initializePlayer();
    updateStatus();
}

StreamDisplay::~StreamDisplay()
{
    delete ui;
}

void StreamDisplay::setBorder()
{
    if (focused) {
        this->setStyleSheet("QWidget#graphicsView {border: 2px solid rgb(255, 221, 174);}");
    } else {
        this->setStyleSheet("QWidget#graphicsView {border: 2px solid rgb(238, 238, 238);}");
    }
}

void StreamDisplay::playStream(QString uri)
{
    if (player) {
        player->stop();
        player->setSource(QUrl());
    }
    player->setSource(QUrl(uri));
    player->play();
}

void StreamDisplay::playStream(Camera* camera)
{
    // 기존 스트림 완전 중지
    stopStream();

    // 새로운 카메라 설정
    this->camera = camera;
    QString rtspUrl = tr("rtsp://%1:8082/test").arg(camera->ip);

    // 재연결 전략 추가
    QTimer::singleShot(0, this, [this, rtspUrl]() {
        // 미디어 플레이어 재설정
        player->stop();
        player->setSource(QUrl(rtspUrl));
        player->play();
    });
}

void StreamDisplay::stopStream()
{
    if (player) {
        player->stop();
        player->setSource(QUrl());
        
        this->camera = nullptr;
        playing = false;
        updateStatus();
    }
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

Camera* StreamDisplay::getCamera()
{
    return camera;
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
        ui->deleteAllAreaButton->setEnabled(true);
    } else {
        ui->cameraNameLabel->setText(tr("연결 안됨"));
        ui->stopStreamButton->setEnabled(false);
        ui->insertAreaButton->setEnabled(false);
        ui->deleteAllAreaButton->setEnabled(false);
    }
}

void StreamDisplay::initializePlayer()
{
    if (player) {
        // 오류 발생 시 자동 재시도 로직
        connect(player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error) {
            qDebug() << "Stream Error: " << error;
            
            // 오류 발생 시 5초 후 재연결 시도
            QTimer::singleShot(5000, this, [this]() {
                if (camera) {
                    playStream(camera);
                }
            });
        });

        // 네트워크 상태 모니터링 추가
        connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
            switch (status) {
                case QMediaPlayer::EndOfMedia:
                    qDebug() << "Stream ended, attempting to reconnect";
                    if (camera) {
                        playStream(camera);
                    }
                    break;
                case QMediaPlayer::InvalidMedia:
                    qDebug() << "Invalid media, check stream URL";
                    break;
                default:
                    break;
            }
        });
        connect(player, &QMediaPlayer::playingChanged, this, [=](bool playing) {
            this->playing = playing;
            updateStatus();
        });
    }
}