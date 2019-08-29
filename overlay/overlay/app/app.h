#ifndef APP_16_04_44_25_8_19_h
#define APP_16_04_44_25_8_19_h

#include <memory>
#include <gst/gst.h>

#include "../overlay/interface.hpp"


G_BEGIN_DECLS

#define GST_OVERLAY_FILTER (gst_overlay_get_type())
#define GST_MY_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_OVERLAY_FILTER, OverlayFilter))
#define GST_MY_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_OVERLAY_FILTER, OverlayFilterClass))
#define GST_IS_MY_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_OVERLAY_FILTER))
#define GST_IS_MY_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_OVERLAY_FILTER))


typedef struct _OverlayFilter
{
    GstElement element;
    GstPad* sinkpad;
    GstPad* srcpad;

    std::string fileName;
    int align;

    std::unique_ptr<OverlayInterface> overlay;
} OverlayFilter;

typedef struct _OverlayFilterClass
{
    GstElementClass parentClass;
} OverlayFilterClass;

/**
 * Standard function returning type information.
 * @return type info of the plug-in
 */
GType gst_overlay_get_type(void);

G_END_DECLS

#endif // APP_16_04_44_25_8_19_h
