#include <algorithm>

#include "color.hpp"


Color::Color() {
}


Color::Color(Component red, Component green, Component blue, Component alpha)
    : red_(red)
    , green_(green)
    , blue_(blue)
    , alpha_(alpha) {}


static inline Color::Component mixComponent(Color::Component dest, Color::Component src, float alpha) {
    return Color::Component((float) dest * (1.0f - alpha) + (float) src * alpha);
}


// TODO use SSE
Color Color::mix(Color dest, Color src) {
    float alpha = src.alpha_ / 255.0f;
    return Color(
        mixComponent(dest.red_, src.red_, alpha),
        mixComponent(dest.green_, src.green_, alpha),
        mixComponent(dest.blue_, src.blue_, alpha),
        dest.alpha_
    );
}
