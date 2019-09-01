#include <string.h>

#include "overlay-factory.hpp"
#include "test-overlay.hpp"
#include "software-overlay.hpp"
#include "ogl-overlay.hpp"


static const char* SoftwareOverlayType = "software";
static const char* OpenGlOverlayType = "gl";


std::unique_ptr<OverlayInterface> CreateOverlay(const char* type) {
    if (strcmp(type, SoftwareOverlayType) == 0) {
        return std::make_unique<SoftwareOverlay>();
    }
    if (strcmp(type, OpenGlOverlayType) == 0) {
        return std::make_unique<OglOverlay>();
    }
    return std::make_unique<TestOverlay>();
}