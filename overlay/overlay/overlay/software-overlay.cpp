#include "../config.h"
#include "software-overlay.hpp"


SoftwareOverlay::SoftwareOverlay()
    : width_(0)
    , height_(0)
    , svgHandle(nullptr)
    , svgBitmap(nullptr) {}


SoftwareOverlay::~SoftwareOverlay() {
    rsvg_handle_close(svgHandle, NULL);
    rsvg_cleanup();
}


int SoftwareOverlay::init(const std::string& path, int width, int height) {
    width_ = width;
    height_ = height;

    GFile* file = g_file_new_for_path(path.c_str());
    GError* error = NULL;
    svgHandle = rsvg_handle_new_from_gfile_sync(file, RSVG_HANDLE_FLAG_KEEP_IMAGE_DATA, NULL, &error);
    if (svgHandle == NULL) {
        g_print("Can't load '%s' file; error %s\n", path, error->message);
        return RetCode::FileNotFound;
    }

    g_print("SVG file '%s' was successfully loaded\n", path);
    rsvg_handle_set_dpi(svgHandle, 90.0); // TODO how to determine DPI?
    svgBitmap = rsvg_handle_get_pixbuf(svgHandle); // TODO it is better render to Cairo context instead
    return svgBitmap ? RetCode::Success : RetCode::Error;
}


int SoftwareOverlay::doProcess(const GstMapInfo* mapInfo) {
    if (svgBitmap == NULL) {
        return 2;
    }

    /*
    guint8* dest = mapInfo->data;
    guchar* pixels = gdk_pixbuf_get_pixels(svgBitmap);
    int bmpHeight = gdk_pixbuf_get_height(svgBitmap);
    int bmpWidth = gdk_pixbuf_get_width(svgBitmap);
    for (int i = 0; i < bmpHeight; ++i) {
        memcpy(dest, pixels, bmpWidth * 4);
        dest += width * BYTE_PER_PIXEL;
        pixels += bmpWidth * 4;
    }
    */
    return 0;
}
