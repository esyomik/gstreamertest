#include <gst/gl/gstglfuncs.h>

#include "shader.hpp"


Shader::Shader()
   : shader_(nullptr) {}


Shader::~Shader() {
    destroy();
}


Result<char> Shader::create(GstGLContext* context, const char* vert, const char* frag) {
    shader_ = gst_gl_shader_new(context);
    Result<char> result{' '};
    if (!(result = compile(context, GL_VERTEX_SHADER, vert)).success()
        || !(result = compile(context, GL_FRAGMENT_SHADER, frag)).success()) {
        return result;
    }

    GError *error = nullptr;
    if (!gst_gl_shader_link(shader_, &error)) {
        result = Result<char>{RetCode::BadFormat, error->message};
        g_error_free(error);
        gst_gl_context_clear_shader(context);
        destroy();
    }
    return result;
}


void Shader::destroy() {
    gst_object_unref(shader_);
    shader_ = nullptr;
}


Result<char> Shader::compile(GstGLContext* context, int type, const char* text) {
    if (!text) {
        return {RetCode::FileNotFound, "Can't compile empty shader"};
    }

    GError* error = nullptr;
    GstGLSLStage* shader = gst_glsl_stage_new_with_string(context, type,
        GST_GLSL_VERSION_NONE,
        GstGLSLProfile(GST_GLSL_PROFILE_ES | GST_GLSL_PROFILE_COMPATIBILITY), text);
    if (shader == nullptr) {
        return {RetCode::Error, "Can't create GLSL shader"};
    }
    if (gst_glsl_stage_compile(shader, &error) && gst_gl_shader_attach(shader_, shader)) {
        return {RetCode::Success, ""};
    }

    gst_object_unref(shader);
    Result<char> result{RetCode::BadFormat, error->message};
    g_error_free(error);
    destroy();
    return result;
}
