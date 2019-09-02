#include <gst/gl/gstglfuncs.h>

#include "../../common/result.hpp"
#include "../../common/bitmap-utils.hpp"
#include "ogl-overlay.hpp"


OglOverlay::OglOverlay()
    : display_(nullptr)
    , context_(nullptr)
    , texture_(0u)
    , started_(false) {}


OglOverlay::~OglOverlay() {
    if (context_ == nullptr) {
        return;
    }

    const GstGLFuncs *gl = context_->gl_vtable;
    gl->DeleteTextures(1, &texture_);
    gst_gl_context_destroy(context_);
    gst_object_unref(display_);
}


int OglOverlay::init(const std::string& path, int width, int height, utils::AlignPosition align) {
    path_ = path;
    display_ = gst_gl_display_new();
    gst_gl_display_filter_gl_api(display_, GST_GL_API_ANY);
    if (display_ == nullptr) {
        g_print("Can't create OpenGL display\n");
        return RetCode::Error;
    }

    context_ = gst_gl_display_get_gl_context_for_thread(display_, nullptr);
    if (context_ == nullptr && !gst_gl_display_create_context(display_, nullptr, &context_, nullptr)) {
        g_print("Can't create OpenGL context\n");
        return RetCode::Error;
    }

    int major, minor;
    gst_gl_context_get_gl_version(context_, &major, &minor);
    g_print("OpenGL v%d.%d context was created successfully\n", major, minor);
    gst_gl_context_thread_add(context_, filterStop, this);
    gst_gl_display_add_context(display_, context_);

    gst_gl_context_activate(context_, true);
    return RetCode::Success;
}


int OglOverlay::doProcess(const GstMapInfo* mapInfo) {
    if (!started_) {
        load(path_);
        started_ = true;
    }
    return RetCode::Success;
}


static const gchar *gVertShader = R"(
attribute vec4 a_position;
attribute vec2 a_texcoord;
varying vec2 v_texcoord;
void main()
{
    gl_Position = a_position;
    v_texcoord = a_texcoord;
})";

/* fragment source */
static const gchar *gFragShader = R"(
uniform sampler2D texture;
uniform float alpha;
varying vec2 v_texcoord;
void main()
{
    vec4 rgba = texture2D(texture, v_texcoord);
    gl_FragColor = vec4(rgba.rgb, rgba.a * alpha);
})";

int OglOverlay::load(const std::string& path) {
    auto shaderResult = shader_.create(context_, gVertShader, gFragShader);
    if (!shaderResult.success()) {
        g_print(shaderResult.description().c_str());
        return shaderResult.getCode();
    }

    auto result = createBitmapFromSvg(path.c_str(), 90.0); // TODO how to determine DPI?
    if (!result.success()) {
        return result.getCode();
    }
    auto* bitmap = result.getData();
    const GstGLFuncs* gl = context_->gl_vtable;
    gl->GenTextures(1, &texture_);
    gl->BindTexture(GL_TEXTURE_2D, texture_);
    gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gdk_pixbuf_get_width(bitmap),
        gdk_pixbuf_get_height(bitmap), 0, GL_RGBA, GL_UNSIGNED_BYTE, gdk_pixbuf_get_pixels(bitmap));
    //gl->GenerateMipmap(GL_TEXTURE_2D);
    g_object_unref(bitmap);

    started_ = true;
    return RetCode::Success;
}


void OglOverlay::filterStop(GstGLContext * context, gpointer data) {
    OglOverlay *overlay = reinterpret_cast<OglOverlay*>(data);
    overlay->started_ = false;
}