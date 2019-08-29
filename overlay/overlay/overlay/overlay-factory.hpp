#ifndef OVERLAY_FACTORY_2_18_21_28_8_19_h
#define OVERLAY_FACTORY_2_18_21_28_8_19_h

#include <memory>
#include "interface.hpp"


std::unique_ptr<OverlayInterface> CreateOverlay(const char* type);


#endif // OVERLAY-FACTORY_2_18_21_28_8_19_h
