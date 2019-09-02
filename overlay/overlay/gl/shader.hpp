#ifndef SHADER_14_55_34_2_9_19_h
#define SHADER_14_55_34_2_9_19_h

#include <gst/gl/gl.h>

#include "../../common/result.hpp"


class Shader
{
public:
    Shader();
    ~Shader();

    Result<char> create(GstGLContext* context, const char* vert, const char* frag);
    void destroy();

private:
    Result<char> compile(GstGLContext* context, int type, const char* text);

private:
    GstGLShader* shader_;

};

#endif // SHADER_14_55_34_2_9_19_h
