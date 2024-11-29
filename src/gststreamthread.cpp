#include "gststreamthread.h"

#include <iostream>

#include <QDebug>
#include <QtConcurrent>

#include <glib.h>
#include <gst/video/videooverlay.h>

static void pad_added_handler(GstElement* src, GstPad* new_pad, CustomData* data);

GstStreamThread::GstStreamThread()
: w_id(NULL), uri(""), terminated(false) { }

GstStreamThread::GstStreamThread(WId w_id, QString uri)
: w_id(w_id), uri(uri), terminated(false) { }

GstStreamThread::~GstStreamThread() {}

void GstStreamThread::terminate() {
    terminated = true;
}

void GstStreamThread::run() {
    GstBus* bus;
    GstMessage* msg;

    data.pipeline = gst_pipeline_new("rtsp-player");

    data.source = gst_element_factory_make("rtspsrc", "source");
    data.depay = gst_element_factory_make("rtph264depay", "depay");
    data.parse = gst_element_factory_make("h264parse", "parse");
    data.decode = gst_element_factory_make("openh264dec", "decode");
    data.convert = gst_element_factory_make("videoconvert", "convert");
    data.sink = gst_element_factory_make("glimagesink", "sink");

    if (!data.pipeline || !data.source || !data.depay || !data.parse || !data.decode || !data.convert || !data.sink) {
        g_printerr("Not all elements could be created.\n");
        return;
    }
    g_print("All elements created.\n");

    gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.depay, data.parse, data.decode, data.convert, data.sink, NULL);

    g_signal_connect(data.source, "pad-added", G_CALLBACK(pad_added_handler), &data);

    std::string rtsp_uri = uri.toStdString();
    g_object_set(G_OBJECT(data.source), "location", rtsp_uri.c_str(), NULL);

    if (!gst_element_link_many(data.depay, data.parse, data.decode, data.convert, data.sink, NULL)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(data.pipeline);
        return;
    }

    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(data.sink), w_id);

    gst_element_set_state(data.pipeline, GST_STATE_PLAYING);

    bus = gst_element_get_bus(data.pipeline);
    do {
        msg = gst_bus_timed_pop_filtered(bus, 100*GST_MSECOND, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

        if (msg != NULL) {
            GError* err;
            gchar* debug_info;

            switch (GST_MESSAGE_TYPE(msg)) {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error(msg, &err, &debug_info);
                    g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
                    g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
                    g_clear_error(&err);
                    g_free(debug_info);
                    terminated = true;
                    break;
                case GST_MESSAGE_EOS:
                    g_print("End-Of-Stream reached.\n");
                    terminated = true;
                    break;
                default:
                    g_printerr("Unexpected message received.\n");
                    break;
            }
            gst_message_unref(msg);
        }
    } while (!terminated);

    gst_element_set_state(data.pipeline, GST_STATE_NULL);
    gst_object_unref(data.pipeline);
    gst_object_unref(bus);
}

static void pad_added_handler(GstElement* src, GstPad* new_pad, CustomData* data) {
    GstPad* sink_pad = gst_element_get_static_pad(data->depay, "sink");
    GstPadLinkReturn ret;
    GstCaps* new_pad_caps = NULL;
    GstStructure* new_pad_struct = NULL;
    const gchar* new_pad_type = NULL;

    g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME(new_pad), GST_ELEMENT_NAME(src));

    if (gst_pad_is_linked(sink_pad)) {
        g_print("We are already linked. Ignoring.\n");
        gst_object_unref(sink_pad);
        return;
    }

    new_pad_caps = gst_pad_get_current_caps(new_pad);
    new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    new_pad_type = gst_structure_get_name(new_pad_struct);
    if (!g_str_has_prefix(new_pad_type, "application/x-rtp")) {
        g_print("It has type '%s' which is not RTP. Ignoring.\n", new_pad_type);
        if (new_pad_caps != NULL) gst_caps_unref(new_pad_caps);
        gst_object_unref(sink_pad);
        return;
    }

    ret = gst_pad_link(new_pad, sink_pad);
    if (GST_PAD_LINK_FAILED(ret)) {
        g_print("Type is '%s' but link failed.\n", new_pad_type);
    } else {
        g_print("Link succeeded (type '%s').\n", new_pad_type);
    }
}