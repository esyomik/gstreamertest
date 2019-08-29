#include <gst/video/video-format.h>
#include <memory.h>

#include "../config.h"
#include "../overlay/overlay-factory.hpp"
#include "app.h"


#define overlay_filter_parent_class parent_class
G_DEFINE_TYPE(OverlayFilter, gst_overlay, GST_TYPE_ELEMENT);

static GstStaticPadTemplate sinkTemplate = GST_STATIC_PAD_TEMPLATE(
    "sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS(GST_VIDEO_CAPS_MAKE(INTERNAL_VIDEO_FORMAT))
);

static GstStaticPadTemplate srcTemplate = GST_STATIC_PAD_TEMPLATE("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS(GST_VIDEO_CAPS_MAKE(INTERNAL_VIDEO_FORMAT))
);

enum
{
    PROP_TYPE = 1,
    PROP_FILE,
    PROP_ALIGN,
};

static void gst_overlay_set_property(GObject* object, guint propId, const GValue* value, GParamSpec* pspec) {
    OverlayFilter *filter = (OverlayFilter*) object;
    switch (propId) {
    case PROP_TYPE:
        filter->overlay = CreateOverlay(g_value_get_string(value));
        break;
    case PROP_FILE:
        filter->fileName = g_value_get_string(value);
        break;
    case PROP_ALIGN:
        filter->align = g_value_get_int(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, propId, pspec);
        break;
    }
}

static void gst_overlay_get_property(GObject* object, guint propId, GValue* value, GParamSpec* pspec) {
}

static void gst_overlay_dispose(GObject * object) {
    OverlayFilter* filter = (OverlayFilter*) object;
    filter->overlay.reset();
    filter->fileName.~basic_string();
}

static void gst_overlay_class_init(OverlayFilterClass* klass) {
    GObjectClass *objectClass = G_OBJECT_CLASS(klass);
    objectClass->dispose = gst_overlay_dispose;
    objectClass->set_property = gst_overlay_set_property;
    objectClass->get_property = gst_overlay_get_property;

    g_object_class_install_property(objectClass, PROP_TYPE, g_param_spec_string(
        "type", "Type", "Overlay type", "test", GParamFlags(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));
    g_object_class_install_property(objectClass, PROP_FILE, g_param_spec_string(
        "file", "File", "File name to render", "", GParamFlags(G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));
    g_object_class_install_property(objectClass, PROP_ALIGN, g_param_spec_int(
        "align", "Align (0-8)", "Align position", 0, 8, 0, G_PARAM_READWRITE));

    GstElementClass* elementClass = GST_ELEMENT_CLASS(klass);

    gst_element_class_set_static_metadata(elementClass,
        "Overlay plugin",
        "Filter/Effect/Video",
        "Overlays an image over video",
        "your name <your.name@your.isp>");

    gst_element_class_add_pad_template(elementClass, gst_static_pad_template_get(&sinkTemplate));
    gst_element_class_add_pad_template(elementClass, gst_static_pad_template_get(&srcTemplate));
}

static GstBuffer* gst_painter_process_data(OverlayFilter* filter, GstBuffer* buf) {
    GstMapInfo srcmapinfo;
    gst_buffer_map(buf, &srcmapinfo, GST_MAP_READ);

    GstBuffer* outbuf = gst_buffer_new();
    GstMemory* memory = gst_allocator_alloc(NULL, srcmapinfo.size, NULL);
    GstMapInfo dstmapinfo;
    if (gst_memory_map(memory, &dstmapinfo, GST_MAP_READWRITE)) {
        memcpy(dstmapinfo.data, srcmapinfo.data, srcmapinfo.size);
        filter->overlay->doProcess(&dstmapinfo);
        gst_buffer_insert_memory(outbuf, -1, memory);
        gst_memory_unmap(memory, &dstmapinfo);
    }

    gst_buffer_unmap(buf, &srcmapinfo);
    return outbuf;
}

static GstFlowReturn overlay_filter_chain(GstPad* pad, GstObject* parent, GstBuffer* buf) {
    OverlayFilter* filter = GST_MY_FILTER(parent);
    GstBuffer *outbuf = gst_painter_process_data(filter, buf);
    gst_buffer_unref(buf);
    if (!outbuf) {
        GST_ELEMENT_ERROR(GST_ELEMENT(filter), STREAM, FAILED, (NULL), (NULL));
        return GST_FLOW_ERROR;
    }
    return gst_pad_push(filter->srcpad, outbuf);
}

static gboolean overlay_filter_sink_event(GstPad* pad, GstObject* parent, GstEvent* event) {
    OverlayFilter *filter = GST_MY_FILTER(parent);
    switch (GST_EVENT_TYPE(event)) {
    case GST_EVENT_CAPS:
        {
            GstCaps* caps;
            gst_event_parse_caps(event, &caps);
            GstStructure* structure = gst_caps_get_structure(caps, 0);
            int width, height;
            gst_structure_get_int(structure, "width", &width);
            gst_structure_get_int(structure, "height", &height);
            g_print("Initializing overlay. width=%d, height=%d\n", width, height);
            filter->overlay->init(filter->fileName, width, height);
        }
        break;
    case GST_EVENT_EOS:
        break;
    }
    return gst_pad_event_default(pad, parent, event);
}

static void gst_overlay_init(OverlayFilter* filter) {
    filter->sinkpad = gst_pad_new_from_static_template(&sinkTemplate, "sink");
    gst_pad_set_event_function(filter->sinkpad, overlay_filter_sink_event);
    gst_pad_set_chain_function(filter->sinkpad, overlay_filter_chain);
    GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

    filter->srcpad = gst_pad_new_from_static_template(&srcTemplate, "src");
    GST_PAD_SET_PROXY_CAPS(filter->srcpad);
    gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);
}
