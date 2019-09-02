#include "app.h"
#include "../../config.h"


static gboolean plugin_init(GstPlugin* plugin) {
    return gst_element_register(plugin, "overlay", GST_RANK_NONE, GST_OVERLAY_FILTER);
}

#ifndef PACKAGE
#define PACKAGE "TEST"
#endif

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR, GST_VERSION_MINOR,
    overlay, "OpenGL overlay plug-in",
    plugin_init,
    VERSION, LICENCE, PACKAGE_NAME, PACKAGE_ORIGIN)