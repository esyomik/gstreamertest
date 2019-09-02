#include <algorithm>
#include <librsvg-2.0/librsvg/rsvg.h>

#include "../config.h"
#include "color.hpp"
#include "rectangle.hpp"
#include "bitmap-utils.hpp"


Result<GdkPixbuf*> createBitmapFromSvg(const char* path, double dpi) {
    GFile* file = g_file_new_for_path(path);
    GError* error = nullptr;
    RsvgHandle* svgHandle = rsvg_handle_new_from_gfile_sync(file, RSVG_HANDLE_FLAG_KEEP_IMAGE_DATA, NULL, &error);
    if (svgHandle == nullptr) {
        g_print("Can't load '%s' file; error %s\n", path, error->message);
        rsvg_handle_close(svgHandle, nullptr);
        return {nullptr, RetCode::FileNotFound};
    }

    g_print("File '%s' was successfully loaded\n", path);
    rsvg_handle_set_dpi(svgHandle, dpi); // TODO how to determine DPI?
    GdkPixbuf* svgBitmap = rsvg_handle_get_pixbuf(svgHandle);
    rsvg_handle_close(svgHandle, nullptr);
    return {svgBitmap};
}


void bitBltBlend(void* frame, int width, int height, void* image, int imageWidth, int imageHeight, utils::AlignPosition align) {
    int nRow = std::min(imageHeight, height);
    int nCol = std::min(imageWidth, width);
    auto bmpPosition = utils::Rectangle<int>(0, 0, width, height).getAligned(nCol, nRow, align);

    for (int i = 0; i < nRow; ++i) {
        auto src = reinterpret_cast<Color*>(reinterpret_cast<char*>(image) + i * imageWidth * BYTE_PER_PIXEL);
        auto dest = reinterpret_cast<Color*>(reinterpret_cast<char*>(frame)
            + ((i + bmpPosition.top()) * width + bmpPosition.left())* BYTE_PER_PIXEL);
        for (int j = 0; j < nCol; ++j) {
            dest[j] = Color::mix(dest[j], src[j]);
        }
    }
}