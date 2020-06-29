#ifndef CLOTHSIM_UTIL_H
#define CLOTHSIM_UTIL_H

#include <Magnum/Magnum.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector.h>

#include "Eigen/Dense"

#include <string>
#include <vector>
#include <chrono>

#include <cassert>

namespace clothsim
{
    using namespace Magnum;

    template <typename V>
    struct AABB
    {
        V min;
        V max;
    };

    template <class V>
    AABB<V> computeAABB(const std::vector<V> &elements)
    {
        V min{std::numeric_limits<typename V::Type>::max()};
        V max{std::numeric_limits<typename V::Type>::min()};

        for (auto element : elements)
        {
            min = Magnum::Math::min(min, element);
            max = Magnum::Math::max(max, element);
        }

        return AABB<V>{min, max};
    }

    std::vector<Magnum::Vector2i> bresenham(const Magnum::Vector2i a, const Magnum::Vector2i b);

#define TIME_FUN(function, parent) \
    clothsim::Timer(std::string(#function) + ": line " + std::to_string(__LINE__) + ":").time(function, parent);

    class Timer
    {
    public:
        explicit Timer(std::string name) : m_pre{std::chrono::high_resolution_clock::now()}, m_name{std::move(name)}
        {
        }

        // Should work for functions and member functions with any arguments and return types
        template <typename F, typename... Args>
        decltype(auto) time(F &&f, Args &&... args)
        {
            return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        }

        ~Timer()
        {
            using namespace std::chrono;
            const high_resolution_clock::time_point post = high_resolution_clock::now();
            const auto elapsed = duration_cast<microseconds>(post - m_pre).count();
            Magnum::Debug{} << m_name.c_str() << elapsed << "ms";
        }

    private:
        const std::chrono::high_resolution_clock::time_point m_pre;
        const std::string m_name;
    };
} // namespace clothsim

#endif //CLOTHSIM_UTIL_H
