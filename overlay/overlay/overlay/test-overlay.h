#ifndef TEST_OVERLAY_2_25_14_28_8_19_h
#define TEST_OVERLAY_2_25_14_28_8_19_h

#include <gst/gst.h>
#include <gst/video/video.h>


int createTestOverlay();

int initTestOverlay(int width, int height);

int doProcessTestOverlay(const GstMapInfo* mapInfo, int width, int height);

void disposeTestOverlay();

#endif // TEST-OVERLAY_2_25_14_28_8_19_h
