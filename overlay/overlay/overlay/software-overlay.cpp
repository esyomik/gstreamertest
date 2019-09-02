#include <librsvg-2.0/librsvg/rsvg.h>
#include <algorithm>

#include "../../config.h"
#include "../../common/bitmap-utils.hpp"
#include "software-overlay.hpp"


SoftwareOverlay::SoftwareOverlay()
    : width_(0)
    , height_(0)
    , svgBitmap(nullptr)
    , align_(utils::AlignPosition::TopLeft) {}


SoftwareOverlay::~SoftwareOverlay() {
    g_object_unref(svgBitmap);
    rsvg_cleanup();
}


int SoftwareOverlay::init(const std::string& path, int width, int height, utils::AlignPosition align) {
    width_ = width;
    height_ = height;
    align_ = align;

    auto result = createBitmapFromSvg(path.c_str(), 90.0); // TODO how to determine DPI?
    if (result.success()) {
        if (gdk_pixbuf_get_n_channels(result.getData()) != BYTE_PER_PIXEL) {
            g_print("Can't use this file. Color depth must be equal 4");
            g_object_unref(result.getData());
            return RetCode::BadFormat;
        }
        svgBitmap = result.getData();
    }

    return result.getCode();
}


int SoftwareOverlay::doProcess(const GstMapInfo* mapInfo) {
    if (svgBitmap == nullptr) {
        return RetCode::FileNotFound;
    }

    bitBltBlend(mapInfo->data, width_, height_, gdk_pixbuf_get_pixels(svgBitmap),
        gdk_pixbuf_get_width(svgBitmap), gdk_pixbuf_get_height(svgBitmap), align_);
    
    return RetCode::Success;
}
