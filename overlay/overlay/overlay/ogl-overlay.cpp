#include <gst/gl/gstglcontext.h>
#include <gst/gl/gstglfuncs.h>

#include "../util/result.hpp"
#include "../util/bitmap-utils.hpp"
#include "ogl-overlay.hpp"


OglOverlay::OglOverlay()
    : display_(nullptr)
    , context_(nullptr)
    , texture_(0u) {}


OglOverlay::~OglOverlay() {
    if (context_ == nullptr) {
        return;
    }

    const GstGLFuncs *gl = context_->gl_vtable;
    gl->DeleteTextures(1, &texture_);
    gst_gl_context_destroy(context_);
}


int OglOverlay::init(const std::string& path, int width, int height, utils::AlignPosition align) {
    display_ = gst_gl_display_new();
    if (!gst_gl_display_create_context(display_, nullptr, &context_, nullptr)) {
        g_print("Can't create OpenGL context\n");
        return RetCode::Error;
    }

    int major, minor;
    gst_gl_context_get_gl_version(context_, &major, &minor);
    g_print("OpenGL v%d.%d context was created successfully\n", major, minor);

    auto result = createBitmapFromSvg(path.c_str(), 90.0); // TODO how to determine DPI?
    if (!result.success()) {
        return result.getCode();
    }
    gst_gl_context_activate(context_, true);

    const GstGLFuncs* gl = context_->gl_vtable;
    auto* bitmap = result.getData();
    gl->GenTextures(1, &texture_);
    gl->BindTexture(GL_TEXTURE_2D, texture_);
    gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gdk_pixbuf_get_width(bitmap),
        gdk_pixbuf_get_height(bitmap), 0, GL_RGBA, GL_UNSIGNED_BYTE, gdk_pixbuf_get_pixels(bitmap));
    //gl->GenerateMipmap(GL_TEXTURE_2D);

    int W = -1;
    gl->GetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &W);
    g_print("WIDTH: %d\n", W);

    g_object_unref(bitmap);
    return RetCode::Success;
}


int OglOverlay::doProcess(const GstMapInfo* mapInfo) {
    if (texture_ == 0u) {

    }
    return RetCode::Success;
}
