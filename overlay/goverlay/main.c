#include "gstgloverlay.h"

#define VERSION "0.1.0"
#define LICENCE "LGPL"
#define PACKAGE_NAME "TESTG source release"
#define PACKAGE_ORIGIN "http://google.com"


static gboolean plugin_init(GstPlugin* plugin) {
    return gst_element_register(plugin, "goverlay", GST_RANK_NONE, GST_TYPE_GL_OVERLAY);
}


#ifndef PACKAGE
#define PACKAGE "TESTG"
#endif

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR, GST_VERSION_MINOR,
    goverlay, "OpenGL overlay plug-in",
    plugin_init,
    VERSION, LICENCE, PACKAGE_NAME, PACKAGE_ORIGIN)