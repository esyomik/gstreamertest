#include <string.h>

#include "overlay-factory.hpp"
#include "test-overlay.hpp"
#include "software-overlay.hpp"


static const char* TypeSoftware = "software";


std::unique_ptr<OverlayInterface> CreateOverlay(const char* type) {
    if (strcmp(type, TypeSoftware) == 0) {
        return std::make_unique<SoftwareOverlay>();
    }
    return std::make_unique<TestOverlay>();
}