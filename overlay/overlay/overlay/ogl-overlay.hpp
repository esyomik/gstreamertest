#ifndef OGL_OVERLAY_17_06_54_1_9_19_h
#define OGL_OVERLAY_17_06_54_1_9_19_h

#include <gst/gl/gl.h>

#include "interface.hpp"


class OglOverlay : public OverlayInterface
{
public:
    OglOverlay();
    virtual ~OglOverlay();

    virtual int init(const std::string& path, int width, int height, utils::AlignPosition align) override;
    virtual int doProcess(const GstMapInfo* mapInfo) override;

private:
    GstGLDisplay* display_;
    GstGLContext* context_;
    unsigned int texture_;

};

#endif // OGL-OVERLAY_17_06_54_1_9_19_h
