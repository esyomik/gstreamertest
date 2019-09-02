#ifndef COLOR_23_17_23_30_8_19_h
#define COLOR_23_17_23_30_8_19_h


class Color
{
public:
    typedef unsigned char Component;

public:
    Color();
    Color(Component red, Component green, Component blue, Component alpha);

    inline Component red() const { return red_; }
    inline Component green() const { return green_; }
    inline Component blue() const { return blue_; }
    inline Component alpha() const { return alpha_; }

    static Color mix(Color dest, Color src);

private:
    Component red_;
    Component green_;
    Component blue_;
    Component alpha_;

};

#endif // COLOR_23_17_23_30_8_19_h
