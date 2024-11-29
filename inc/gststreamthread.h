#ifndef GSTSTREAMTHREAD_H
#define GSTSTREAMTHREAD_H

#include <QWidget>
#include <QThread>

#include <gst/gst.h>

typedef struct _CustomData {
    GstElement* pipeline;
    GstElement* source;
    GstElement* depay;
    GstElement* parse;
    GstElement* decode;
    GstElement* convert;
    GstElement* sink;
} CustomData;

class GstStreamThread : public QThread
{
    Q_OBJECT

public:
    GstStreamThread();
    GstStreamThread(WId w_id, QString uri);
    ~GstStreamThread();

    void terminate();

protected:
    void run() override;

private:
    WId w_id;
    QString uri;
    bool terminated;

    CustomData data;
};

#endif // GSTSTREAMTHREAD_H