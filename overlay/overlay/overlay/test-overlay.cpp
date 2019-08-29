#include <memory.h>

#include "../config.h"
#include "test-overlay.hpp"


TestOverlay::TestOverlay()
    : width_(0) {;}


int TestOverlay::init(const std::string& path, int width, int height) {
    width_ = width;
    return 0;
}


int TestOverlay::doProcess(const GstMapInfo* mapInfo) {
    memset(mapInfo->data + mapInfo->size / 2, 0, BYTE_PER_PIXEL * width_);
    return 0;
}
