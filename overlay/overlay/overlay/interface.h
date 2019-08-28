#ifndef INTERFACE_2_00_04_28_8_19_h
#define INTERFACE_2_00_04_28_8_19_h

#include <gst/gst.h>
#include <gst/video/video.h>

/**
 * This file contains declarations of overlay functions.
 */

/**
 * Creates an overlay surface.
 * @return a result code
 */
typedef int(*FnOverlayCreate) ();

/**
 * Initializes an overlay surface.
 * @param width the width of the overlay in pixels
 * @param height the height of the overlay in pixels
 * @return a boolean result
 */
typedef int(*FnOverlayInit) (int width, int height);

/**
 * Modifies video frames.
 * @param mapInfo a raw byte buffer contained raw frame data
 * @param width the width of the video frame in pixel
 * @param height the height of the video frame in pixel
 * @return a boolean result
 */
typedef int(*FnOverlayDoProcess) (const GstMapInfo* mapInfo, int width, int height);

/**
 * Disposes resources captured by an overlay.
 */
typedef void(*FnOverlayDispose) ();

#endif // INTERFACE_2_00_04_28_8_19_h
