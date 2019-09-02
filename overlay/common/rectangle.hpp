#ifndef RECTANGLE_26_02_2016_0_06
#define RECTANGLE_26_02_2016_0_06


namespace utils {

// TODO add method
// AlignPosition valueOf(const std::string_view name);
enum class AlignPosition
{
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    Number
};


template<typename T> class Rectangle
{
public:
    using type = typename T;

public:
    Rectangle() { ; }
    Rectangle(T l, T t, T r, T b) : left_(l), top_(t), right_(r), bottom_(b) { ; }

    template<typename N> Rectangle<N> to() const { return Rectangle<N>((N) left_, (N) top_, (N) right_, (N) bottom_); }

    T left() const { return left_; }
    T top() const { return top_; }
    T right() const { return right_; }
    T bottom() const { return bottom_; }

    Rectangle<T>& alignTo(const Rectangle<T> &r, AlignPosition align);
    Rectangle<T> getAligned(T width, T height, AlignPosition align) const;
    Rectangle<T>& scale(T horizontal, T vertical);
    Rectangle<T>& offset(T dx, T dy);

private:
    T left_;
    T top_;
    T right_;
    T bottom_;

};


template<typename T>
Rectangle<T>& Rectangle<T>::alignTo(const Rectangle<T> &r, AlignPosition align) {
    T w = right_ - left_;
    T h = bottom_ - top_;
    switch (align) {
    case AlignPosition::TopLeft:
        left_ = r.left();
        right_ = left_ + w;
        top_ = r.top();
        bottom_ = top_ + h;
        break;
    case AlignPosition::TopCenter:
        left_ = (r.left() + r.right() - w) / 2;
        right_ = left_ + w;
        top_ = r.top();
        bottom_ = top_ + h;
        break;
    case AlignPosition::TopRight:
        right_ = r.right();
        top_ = r.top();
        left_ = right_ - w;
        bottom_ = top_ + h;
        break;
    case AlignPosition::CenterLeft:
        top_ = (r.top() + r.bottom() - h) / 2;
        bottom_ = top_ + h;
        left_ = r.left();
        right_ = left_ + w;
        break;
    case AlignPosition::Center:
        top_ = (r.top() + r.bottom() - h) / 2;
        bottom_ = top_ + h;
        left_ = (r.left() + r.right() - w) / 2;
        right_ = left_ + w;
        break;
    case AlignPosition::CenterRight:
        top_ = (r.top() + r.bottom() - h) / 2;
        bottom_ = top_ + h;
        right_ = r.right();
        left_ = right_ - w;
        break;
    case AlignPosition::BottomLeft:
        left_ = r.left();
        right_ = left_ + w;
        bottom_ = r.bottom();
        top_ = bottom_ - h;
        break;
    case AlignPosition::BottomCenter:
        left_ = (r.left() + r.right() - w) / 2;
        right_ = left_ + w;
        bottom_ = r.bottom();
        top_ = bottom_ - h;
        break;
    case AlignPosition::BottomRight:
        right_ = r.right();
        left_ = right_ - w;
        bottom_ = r.bottom();
        top_ = bottom_ - h;
        break;
    }
    return *this;
}


template<typename T>
Rectangle<T> Rectangle<T>::getAligned(T width, T height, AlignPosition align)const {
    Rectangle<T> r(0, 0, width, height);
    r.alignTo(*this, align);
    return r;
}


template<typename T>
inline Rectangle<T>& Rectangle<T>::scale(T horizontal, T vertical) {
    left_ *= horizontal;
    top_ *= vertical;
    right_ *= horizontal;
    bottom_ *= vertical;
    return *this;
}


template<typename T>
utils::Rectangle<T>& utils::Rectangle<T>::offset(T dx, T dy) {
    left_ += dx;
    right_ += dx;
    top_ += dy;
    bottom_ += dy;
    return *this;
}

}

#endif // RECTANGLE_26_02_2016_0_06
