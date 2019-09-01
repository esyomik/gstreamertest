#ifndef INTERFACE_2_00_04_28_8_19_h
#define INTERFACE_2_00_04_28_8_19_h

#include <string>
#include <gst/gst.h>
#include <gst/video/video.h>

#include "../util/rectangle.hpp"


/**
 * Overlay interface.
 */
class OverlayInterface
{
public:

    /**
     * Initializes an overlay surface.
     * @param path the name of file with resource; some implementation may use it as path to script
     * @param width the width of the overlay in pixels
     * @param height the height of the overlay in pixels
     * @param align align position of the overlay
     * @return a boolean result
     */
    virtual int init(const std::string& path, int width, int height, utils::AlignPosition align) = 0;

    /**
     * Modifies video frames.
     * @param mapInfo a raw byte buffer contained raw frame data
     * @return a boolean result
     */
    virtual int doProcess(const GstMapInfo* mapInfo) = 0;

};

#endif // INTERFACE_2_00_04_28_8_19_h
