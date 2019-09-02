/*
 * GStreamer
 * Copyright (C) 2008 Filippo Argiolas <filippo.argiolas@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <gst/base/gsttypefindhelper.h>
#include <gst/gl/gstglconfig.h>

#include "../config.h"
#include "../common/bitmap-utils.hpp"
#include "gstgloverlay.h"
#include "gstglutils.h"

#if defined(_MSC_VER) || (defined (__MINGW64_VERSION_MAJOR) && __MINGW64_VERSION_MAJOR >= 6)
#define HAVE_BOOLEAN
#endif

#define GST_CAT_DEFAULT gst_gl_overlay_debug
GST_DEBUG_CATEGORY_STATIC (GST_CAT_DEFAULT);

#define DEBUG_INIT \
  GST_DEBUG_CATEGORY_INIT (gst_gl_overlay_debug, "gloverlay", 0, "gloverlay element");

#define gst_gl_overlay_parent_class parent_class
G_DEFINE_TYPE_WITH_CODE (GstGOverlay, gst_gl_overlay, GST_TYPE_GL_FILTER, DEBUG_INIT);

static gboolean gst_gl_overlay_set_caps(GstGLFilter* filter, GstCaps* incaps, GstCaps* outcaps);
static void gst_gl_overlay_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);
static void gst_gl_overlay_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);
static void gst_gl_overlay_before_transform(GstBaseTransform* trans, GstBuffer* outbuf);
static gboolean gst_gl_overlay_filter_texture(GstGLFilter* filter, GstGLMemory* in_tex, GstGLMemory* out_tex);

enum
{
  PROP_0,
  PROP_LOCATION,
  PROP_ALIGN
};


static const gchar *overlay_v_src =
    "attribute vec4 a_position;\n"
    "attribute vec2 a_texcoord;\n"
    "varying vec2 v_texcoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = a_position;\n"
    "   v_texcoord = a_texcoord;\n"
    "}";

static const gchar *overlay_f_src =
    "uniform sampler2D texture;\n"
    "varying vec2 v_texcoord;\n"
    "void main()\n"
    "{\n"
    "  gl_FragColor = texture2D( texture, v_texcoord );\n"
    "}\n";


static gboolean gst_gl_overlay_gl_start (GstGLBaseFilter * base_filter)
{
  GstGOverlay *overlay = GST_GL_OVERLAY(base_filter);

  if (!GST_GL_BASE_FILTER_CLASS (parent_class)->gl_start (base_filter))
    return FALSE;

  return gst_gl_context_gen_shader (base_filter->context, overlay_v_src,
      overlay_f_src, &overlay->shader);
}

/* free resources that need a gl context */
static void
gst_gl_overlay_gl_stop (GstGLBaseFilter * base_filter)
{
  GstGOverlay *overlay = GST_GL_OVERLAY (base_filter);
  const GstGLFuncs *gl = base_filter->context->gl_vtable;

  if (overlay->shader) {
    gst_object_unref (overlay->shader);
    overlay->shader = NULL;
  }

  if (overlay->image_memory) {
    gst_memory_unref ((GstMemory *) overlay->image_memory);
    overlay->image_memory = NULL;
  }

  if (overlay->vao) {
    gl->DeleteVertexArrays (1, &overlay->vao);
    overlay->vao = 0;
  }

  if (overlay->vbo) {
    gl->DeleteBuffers (1, &overlay->vbo);
    overlay->vbo = 0;
  }

  if (overlay->vbo_indices) {
    gl->DeleteBuffers (1, &overlay->vbo_indices);
    overlay->vbo_indices = 0;
  }

  if (overlay->overlay_vao) {
    gl->DeleteVertexArrays (1, &overlay->overlay_vao);
    overlay->overlay_vao = 0;
  }

  if (overlay->overlay_vbo) {
    gl->DeleteBuffers (1, &overlay->overlay_vbo);
    overlay->overlay_vbo = 0;
  }

  GST_GL_BASE_FILTER_CLASS (parent_class)->gl_stop(base_filter);
}

static void
gst_gl_overlay_class_init (GstGOverlayClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *element_class;

  gobject_class = (GObjectClass *) klass;
  element_class = GST_ELEMENT_CLASS (klass);

  gst_gl_filter_add_rgba_pad_templates (GST_GL_FILTER_CLASS (klass));

  gobject_class->set_property = gst_gl_overlay_set_property;
  gobject_class->get_property = gst_gl_overlay_get_property;

  GST_GL_BASE_FILTER_CLASS (klass)->gl_start = gst_gl_overlay_gl_start;
  GST_GL_BASE_FILTER_CLASS (klass)->gl_stop = gst_gl_overlay_gl_stop;

  GST_GL_FILTER_CLASS (klass)->set_caps = gst_gl_overlay_set_caps;
  GST_GL_FILTER_CLASS (klass)->filter_texture = gst_gl_overlay_filter_texture;

  GST_BASE_TRANSFORM_CLASS (klass)->before_transform = GST_DEBUG_FUNCPTR (gst_gl_overlay_before_transform);

  g_object_class_install_property (gobject_class, PROP_LOCATION,
      g_param_spec_string ("location", "location", "Location of image file to overlay", NULL,
          GParamFlags(GST_PARAM_CONTROLLABLE | GST_PARAM_MUTABLE_PLAYING
              | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));

  g_object_class_install_property(gobject_class, PROP_ALIGN, g_param_spec_int(
      "align", "Align (0-8)", "Align position", 0, 8, 0, G_PARAM_READWRITE));

  gst_element_class_set_metadata (element_class,
      "Gstreamer OpenGL Overlay", "Filter/Effect/Video",
      "Overlay GL video texture with a SVG image",
      "Filippo Argiolas <filippo.argiolas@gmail.com>, "
      "Matthew Waters <matthew@centricular.com>");

  GST_GL_BASE_FILTER_CLASS(klass)->supported_gl_api =
      GstGLAPI(GST_GL_API_OPENGL | GST_GL_API_GLES2 | GST_GL_API_OPENGL3);
}

static void
gst_gl_overlay_set_property(GObject * object, guint prop_id, const GValue * value, GParamSpec * pspec) {
    GstGOverlay* overlay = GST_GL_OVERLAY(object);

    switch (prop_id) {
    case PROP_LOCATION:
        g_free(overlay->location);
        overlay->location_has_changed = TRUE;
        overlay->location = g_value_dup_string(value);
        break;
    case PROP_ALIGN:
        overlay->align = g_value_get_int(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
gst_gl_overlay_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstGOverlay *overlay = GST_GL_OVERLAY (object);

  switch (prop_id) {
    case PROP_LOCATION:
      g_value_set_string (value, overlay->location);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gboolean
gst_gl_overlay_set_caps (GstGLFilter * filter, GstCaps * incaps, GstCaps * outcaps)
{
  GstGOverlay *overlay = GST_GL_OVERLAY (filter);
  GstStructure *s = gst_caps_get_structure (incaps, 0);

  gst_structure_get_int (s, "width", &overlay->window_width);
  gst_structure_get_int (s, "height", &overlay->window_height);

  return TRUE;
}

static void
_unbind_buffer (GstGOverlay * overlay)
{
  GstGLFilter *filter = GST_GL_FILTER (overlay);
  const GstGLFuncs *gl = GST_GL_BASE_FILTER (overlay)->context->gl_vtable;

  gl->BindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
  gl->BindBuffer (GL_ARRAY_BUFFER, 0);

  gl->DisableVertexAttribArray (filter->draw_attr_position_loc);
  gl->DisableVertexAttribArray (filter->draw_attr_texture_loc);
}

static void
_bind_buffer (GstGOverlay * overlay, GLuint vbo)
{
  GstGLFilter *filter = GST_GL_FILTER (overlay);
  const GstGLFuncs *gl = GST_GL_BASE_FILTER (overlay)->context->gl_vtable;

  gl->BindBuffer (GL_ELEMENT_ARRAY_BUFFER, overlay->vbo_indices);
  gl->BindBuffer (GL_ARRAY_BUFFER, vbo);

  gl->EnableVertexAttribArray (filter->draw_attr_position_loc);
  gl->EnableVertexAttribArray (filter->draw_attr_texture_loc);

  gl->VertexAttribPointer (filter->draw_attr_position_loc, 3, GL_FLOAT,
      GL_FALSE, 5 * sizeof (GLfloat), (void *) 0);
  gl->VertexAttribPointer (filter->draw_attr_texture_loc, 2, GL_FLOAT,
      GL_FALSE, 5 * sizeof (GLfloat), (void *) (3 * sizeof (GLfloat)));
}

/* *INDENT-OFF* */
float v_vertices[] = {
/*|      Vertex     | TexCoord  |*/
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
   1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
};

static const GLushort indices[] = { 0, 1, 2, 0, 2, 3, };
/* *INDENT-ON* */

static gboolean
gst_gl_overlay_callback (GstGLFilter * filter, GstGLMemory * in_tex, gpointer stuff)
{
  GstGOverlay *overlay = GST_GL_OVERLAY (filter);
  GstMapInfo map_info;
  guint image_tex;
  gboolean memory_mapped = FALSE;
  const GstGLFuncs *gl = GST_GL_BASE_FILTER (filter)->context->gl_vtable;
  gboolean ret = FALSE;

#if GST_GL_HAVE_OPENGL
  if (gst_gl_context_get_gl_api (GST_GL_BASE_FILTER (filter)->context) &
      GST_GL_API_OPENGL) {

    gl->MatrixMode (GL_PROJECTION);
    gl->LoadIdentity ();
  }
#endif

  gl->ActiveTexture (GL_TEXTURE0);
  gl->BindTexture (GL_TEXTURE_2D, gst_gl_memory_get_texture_id (in_tex));

  gst_gl_shader_use(overlay->shader);
  gst_gl_shader_set_uniform_1i (overlay->shader, "texture", 0);

  filter->draw_attr_position_loc =
      gst_gl_shader_get_attribute_location (overlay->shader, "a_position");
  filter->draw_attr_texture_loc =
      gst_gl_shader_get_attribute_location (overlay->shader, "a_texcoord");

  gst_gl_filter_draw_fullscreen_quad (filter);

  if (!overlay->image_memory)
    goto out;

  if (!gst_memory_map ((GstMemory *) overlay->image_memory, &map_info,
          GstMapFlags(GST_MAP_READ | GST_MAP_GL)) || map_info.data == NULL)
    goto out;

  memory_mapped = TRUE;
  image_tex = *(guint *) map_info.data;

  if (!overlay->overlay_vbo) {
    if (gl->GenVertexArrays) {
      gl->GenVertexArrays (1, &overlay->overlay_vao);
      gl->BindVertexArray (overlay->overlay_vao);
    }

    gl->GenBuffers (1, &overlay->vbo_indices);
    gl->BindBuffer (GL_ELEMENT_ARRAY_BUFFER, overlay->vbo_indices);
    gl->BufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices, GL_STATIC_DRAW);

    gl->GenBuffers (1, &overlay->overlay_vbo);
    gl->BindBuffer (GL_ARRAY_BUFFER, overlay->overlay_vbo);
    gl->BindBuffer (GL_ELEMENT_ARRAY_BUFFER, overlay->vbo_indices);
    overlay->geometry_change = TRUE;
  }

  if (gl->GenVertexArrays) {
    gl->BindVertexArray (overlay->overlay_vao);
  }

  if (overlay->geometry_change) {
    float vertices[] = {
     -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     -1.0f,  1.0f, 0.0f, 0.0,  1.0f,
    };

    auto pos = utils::Rectangle<gint>(0, 0, overlay->window_width, overlay->window_height)
        .getAligned(overlay->image_width, overlay->image_height, (utils::AlignPosition) overlay->align)
        .to<float>()
        .scale(2.0f / overlay->window_width, 2.0f / overlay->window_height)
        .offset(-1.0f, -1.0f);

    vertices[0] = vertices[15] = pos.left();
    vertices[5] = vertices[10] = pos.right();
    vertices[1] = vertices[6] = pos.top();
    vertices[11] = vertices[16] = pos.bottom();

    gl->BufferData (GL_ARRAY_BUFFER, 4 * 5 * sizeof (GLfloat), vertices, GL_STATIC_DRAW);
  }

  _bind_buffer (overlay, overlay->overlay_vbo);

  gl->BindTexture (GL_TEXTURE_2D, image_tex);

  gl->Enable (GL_BLEND);
  if (gl->BlendFuncSeparate)
    gl->BlendFuncSeparate (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  else
    gl->BlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  gl->BlendEquation (GL_FUNC_ADD);

  gl->DrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

  gl->Disable (GL_BLEND);
  ret = TRUE;

out:
  if (gl->GenVertexArrays)
    gl->BindVertexArray (0);
  else
    _unbind_buffer (overlay);

  gst_gl_context_clear_shader (GST_GL_BASE_FILTER (filter)->context);

  if (memory_mapped)
    gst_memory_unmap ((GstMemory *) overlay->image_memory, &map_info);

  overlay->geometry_change = FALSE;

  return ret;
}

static gboolean load_file(GstGOverlay* overlay) {
    if (overlay->location == NULL) {
        return TRUE;
    }

    auto loadResult = createBitmapFromSvg(overlay->location, 72.0); // TODO how to determine DPI?
    if (!loadResult.success()) {
        g_print("Can't load '%s' file; error %s\n", overlay->location, loadResult.description().c_str());
        return FALSE;
    }

    GdkPixbuf* svgBitmap = loadResult.getData();
    int nChannels = gdk_pixbuf_get_n_channels(svgBitmap);
    if (nChannels != BYTE_PER_PIXEL) {
        g_print("Unsupported image format. Expects RGBA.\n");
        g_object_unref(svgBitmap);
        return FALSE;
    }

    overlay->image_width = gdk_pixbuf_get_width(svgBitmap);
    overlay->image_height = gdk_pixbuf_get_height(svgBitmap);

    GstVideoInfo videoInfo;
    gst_video_info_set_format(&videoInfo, GST_VIDEO_FORMAT_RGBA, overlay->image_width, overlay->image_height);
    GstGLBaseMemoryAllocator* memAllocator = GST_GL_BASE_MEMORY_ALLOCATOR(
        gst_gl_memory_allocator_get_default(GST_GL_BASE_FILTER(overlay)->context));
    GstGLVideoAllocationParams* params = gst_gl_video_allocation_params_new(GST_GL_BASE_FILTER(overlay)->context,
        NULL, &videoInfo, 0, NULL, GST_GL_TEXTURE_TARGET_2D, GST_GL_RGBA);
    overlay->image_memory = (GstGLMemory *) gst_gl_base_memory_alloc(memAllocator, (GstGLAllocationParams *) params);
    gst_gl_allocation_params_free((GstGLAllocationParams *) params);
    gst_object_unref(memAllocator);

    GstMapInfo mapInfo;
    if (!gst_memory_map((GstMemory *) overlay->image_memory, &mapInfo, GST_MAP_WRITE)) {
        g_print("Failed to map memory\n");
        g_object_unref(svgBitmap);
        return FALSE;
    }

    memcpy(mapInfo.data, gdk_pixbuf_get_pixels(svgBitmap), BYTE_PER_PIXEL * overlay->image_width * overlay->image_height);
    gst_memory_unmap((GstMemory *) overlay->image_memory, &mapInfo);
    g_object_unref(svgBitmap);
    return TRUE;
}

static gboolean
gst_gl_overlay_filter_texture (GstGLFilter * filter, GstGLMemory * in_tex, GstGLMemory * out_tex)
{
  GstGOverlay *overlay = GST_GL_OVERLAY (filter);

  if(overlay->location_has_changed) {
    if(overlay->image_memory) {
        gst_memory_unref((GstMemory *) overlay->image_memory);
        overlay->image_memory = NULL;
    }

    if(!load_file(overlay)) {
        return FALSE;
    }

    overlay->location_has_changed = FALSE;
  }

  gst_gl_filter_render_to_target(filter, in_tex, out_tex, gst_gl_overlay_callback, overlay);
  return TRUE;
}

static void
gst_gl_overlay_before_transform(GstBaseTransform* trans, GstBuffer* outbuf)
{
  GstClockTime stream_time;

  stream_time = gst_segment_to_stream_time (&trans->segment, GST_FORMAT_TIME,
      GST_BUFFER_TIMESTAMP (outbuf));

  if (GST_CLOCK_TIME_IS_VALID (stream_time))
    gst_object_sync_values (GST_OBJECT (trans), stream_time);
}

static void gst_gl_overlay_init(GstGOverlay * overlay) {
}