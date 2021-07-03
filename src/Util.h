#ifndef CLOTHSIM_UTIL_H
#define CLOTHSIM_UTIL_H

#include <Magnum/Magnum.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector.h>

#include "Eigen/Dense"

#include <chrono>
#include <string>
#include <vector>

#include <cassert>

namespace clothsim {
using namespace Magnum;

template <typename V> struct AABB {
  V min;
  V max;
};

template <class V> AABB<V> computeAABB(const std::vector<V> &elements) {
  V min{std::numeric_limits<typename V::Type>::max()};
  V max{std::numeric_limits<typename V::Type>::min()};

  for (auto element : elements) {
    min = Magnum::Math::min(min, element);
    max = Magnum::Math::max(max, element);
  }

  return AABB<V>{min, max};
}

std::vector<Magnum::Vector2i> bresenham(const Magnum::Vector2i a,
                                        const Magnum::Vector2i b);

#define TO_STRING_DETAIL(x) #x
#define TO_STRING(x) TO_STRING_DETAIL(x)

#define TIME_FUN(function, ...)                                                \
  clothsim::createTimer(#function ": line " TO_STRING(__LINE__) ":")           \
      .time(function, __VA_ARGS__);

// Perfectly over-engineered timing function.
// Can be used for timing functions with any return type or even
// whole blocks of code.
template <std::size_t N> class Timer {
public:
  template <std::size_t... seq>
  Timer(const char name[N], const std::index_sequence<seq...> &)
      : m_name{name[seq]...}, m_pre{std::chrono::high_resolution_clock::now()} {
  }

  // All this hassle only for avoiding a heap allocation from std::string
  explicit constexpr Timer(const char name[N])
      : Timer{name, std::make_index_sequence<N>()} {}

  // Should work for functions and member functions with any arguments and
  // return types.
  template <typename F, typename... Args>
  inline decltype(auto) time(F &&f, Args &&...args) {
    return std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
  }

  ~Timer() {
    using namespace std::chrono;
    const auto post = high_resolution_clock::now();
    const auto elapsed = duration_cast<microseconds>(post - m_pre).count();
    Magnum::Debug{} << m_name << elapsed << "ms";
  }

private:
  const char m_name[N];
  const std::chrono::high_resolution_clock::time_point m_pre;
};

template <std::size_t N> constexpr Timer<N> createTimer(const char (&name)[N]) {
  return Timer<N>{name};
}

} // namespace clothsim

#endif // CLOTHSIM_UTIL_H
