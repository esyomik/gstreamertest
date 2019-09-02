#ifndef OGL_OVERLAY_17_06_54_1_9_19_h
#define OGL_OVERLAY_17_06_54_1_9_19_h

#include <gst/gl/gl.h>
#include <gst/gl/gstglcontext.h>

#include "interface.hpp"
#include "../gl/shader.hpp"


class OglOverlay : public OverlayInterface
{
public:
    OglOverlay();
    virtual ~OglOverlay();

    virtual int init(const std::string& path, int width, int height, utils::AlignPosition align) override;
    virtual int doProcess(const GstMapInfo* mapInfo) override;

private:
    int load(const std::string& path);
    static void filterStop(GstGLContext * context, gpointer data);

private:
    std::string path_;
    GstGLDisplay* display_;
    GstGLContext* context_;
    unsigned int texture_;

    bool started_;
    Shader shader_;

};

#endif // OGL-OVERLAY_17_06_54_1_9_19_h
