#include "ui_streamdisplay.h"
#include "streamdisplay.h"

StreamDisplay::StreamDisplay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StreamDisplay)
{
    ui->setupUi(this);
    streamThread = NULL;
    ui->label->setPixmap(QPixmap(":/images/placeholder.png"));
    focused = false;
    setBorder();
}

StreamDisplay::~StreamDisplay()
{
    delete ui;
}

void StreamDisplay::setBorder()
{
    if (focused) {
        this->setStyleSheet("border: 2px solid red;");
    } else {
        this->setStyleSheet("border: 2px solid black;");
    }
}

void StreamDisplay::playStream(QString uri)
{
    if (streamThread != NULL) {
        stopStream();
    }
    streamThread = new GstStreamThread(ui->label->winId(), uri);
    streamThread->start();
}

void StreamDisplay::stopStream()
{
    streamThread->terminate();
    streamThread->wait();
    delete streamThread;
    streamThread = NULL;
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