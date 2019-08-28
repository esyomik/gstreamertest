#ifndef OVERLAY_FACTORY_2_18_21_28_8_19_h
#define OVERLAY_FACTORY_2_18_21_28_8_19_h

#include "interface.h"

FnOverlayCreate getCreateOverlayFn(const char* overlayType);

FnOverlayInit getInitOverlayFn(const char* overlayType);

FnOverlayDoProcess getDoProcessFn(const char* overlayType);

FnOverlayDispose getOverlayDisposeFn(const char* type);

#endif // OVERLAY-FACTORY_2_18_21_28_8_19_h
