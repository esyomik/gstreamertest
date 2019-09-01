#ifndef TEST_OVERLAY_2_25_14_28_8_19_h
#define TEST_OVERLAY_2_25_14_28_8_19_h

#include "interface.hpp"


class TestOverlay : public OverlayInterface
{
public:
    TestOverlay();
    virtual ~TestOverlay() { ; }

    virtual int init(const std::string& path, int width, int height, utils::AlignPosition align) override;
    virtual int doProcess(const GstMapInfo* mapInfo) override;

private:
    int width_;

};

#endif // TEST-OVERLAY_2_25_14_28_8_19_h
