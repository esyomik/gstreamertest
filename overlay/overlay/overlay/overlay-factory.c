#include "overlay-factory.h"
#include "test-overlay.h"


FnOverlayCreate getCreateOverlayFn(const char* overlayType) {
    return createTestOverlay;
}

FnOverlayInit getInitOverlayFn(const char* overlayType) {
    return initTestOverlay;
}

FnOverlayDoProcess getDoProcessFn(const char* overlayType) {
    return doProcessTestOverlay;
}

FnOverlayDispose getOverlayDisposeFn(const char* type) {
    return disposeTestOverlay;
}