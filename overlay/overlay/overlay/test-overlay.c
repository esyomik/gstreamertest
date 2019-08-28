#include "test-overlay.h"


int createTestOverlay() {
    return 0;
}

int initTestOverlay(int width, int height) {
    return 0;
}

int doProcessTestOverlay(const GstMapInfo* mapInfo, int width, int height) {
    memset(mapInfo->data + mapInfo->size / 3, 0, 3 * width);
    return 0;
}

void disposeTestOverlay() {}