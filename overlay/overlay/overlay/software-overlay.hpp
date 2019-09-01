#ifndef SOFTWARE_OVERLAY_0_49_53_29_8_19_h
#define SOFTWARE_OVERLAY_0_49_53_29_8_19_h

#include <gdk-pixbuf-2.0/gdk-pixbuf/gdk-pixbuf.h>
#include "interface.hpp"


class SoftwareOverlay : public OverlayInterface
{
public:
    SoftwareOverlay();
    virtual ~SoftwareOverlay();

    virtual int init(const std::string& path, int width, int height, utils::AlignPosition align) override;
    virtual int doProcess(const GstMapInfo* mapInfo) override;

private:
    int width_;
    int height_;
    utils::AlignPosition align_;

    GdkPixbuf* svgBitmap;

};

#endif // SOFTWARE-OVERLAY_0_49_53_29_8_19_h
