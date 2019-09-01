#ifndef BITMAP_UTILS_16_27_58_1_9_19_h
#define BITMAP_UTILS_16_27_58_1_9_19_h

#include <gdk-pixbuf-2.0/gdk-pixbuf/gdk-pixbuf.h>

#include "result.hpp"
#include "rectangle.hpp"


Result<GdkPixbuf*> createBitmapFromSvg(const char* path, double dpi);

void bitBltBlend(void* frame, int width, int height, void* image, int imageWidth, int imageHeight, utils::AlignPosition align);

#endif // BITMAP-UTILS_16_27_58_1_9_19_h
