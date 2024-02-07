#ifndef INLINE_
#define INLINE_

template <typename T>
inline static T min(T a, T b) {
  return a < b ? a : b;
}

template <typename T>
inline static T max(T a, T b) {
  return a > b ? a : b;
}

template <typename T>
inline static T clamp(T v, T a, T b) {
  return min(max(v, a), b);
}

#endif  // INLINE_
