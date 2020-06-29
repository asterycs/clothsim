#include "Util.h"

#include <array>
#include <sstream>

namespace clothsim
{
    static std::vector<Vector2i> bresenhamL(const Vector2i a, const Vector2i b)
    {
        std::vector<Vector2i> output;

        Vector2i delta = b - a;
        Int ys = Magnum::Math::sign(delta.y());
        delta.y() = abs(delta.y());

        Int err = 2 * delta.y() - delta.x();
        Int y = a.y();

        for (Int x = a.x(); x <= b.x(); ++x)
        {
            output.push_back({x, y});

            if (err > 0)
            {
                y += ys;
                err -= 2 * delta.x();
            }
            err += 2 * delta.y();
        }

        return output;
    }

    static std::vector<Vector2i> bresenhamH(const Vector2i a, const Vector2i b)
    {
        std::vector<Vector2i> output;

        Vector2i delta = b - a;
        Int xs = Magnum::Math::sign(delta.x());
        delta.x() = abs(delta.x());

        Int err = 2 * delta.x() - delta.y();
        Int x = a.x();

        for (Int y = a.y(); y <= b.y(); ++y)
        {
            output.push_back({x, y});

            if (err > 0)
            {
                x += xs;
                err -= 2 * delta.y();
            }
            err += 2 * delta.x();
        }

        return output;
    }

    std::vector<Vector2i> bresenham(const Vector2i a, const Vector2i b)
    {
        if (abs(b.y() - a.y()) < abs(b.x() - a.x()))
        {
            if (a.x() > b.x())
                return bresenhamL(b, a);
            else
                return bresenhamL(a, b);
        }
        else
        {
            if (a.y() > b.y())
                return bresenhamH(b, a);
            else
                return bresenhamH(a, b);
        }
    }

} // namespace clothsim