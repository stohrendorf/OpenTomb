#pragma once

#include "vec.h"

#include <utility>

namespace core
{
struct BoundingBox final
{
  explicit BoundingBox() = default;

  explicit BoundingBox(TRVec min, TRVec max)
      : min{std::move(min)}
      , max{std::move(max)}
  {
  }

  [[nodiscard]] bool isValid() const noexcept
  {
    return min.X <= max.X && min.Y <= max.Y && min.Z <= max.Z;
  }

  void makeValid() noexcept
  {
    if(min.X > max.X)
      std::swap(min.X, max.X);
    if(min.Y > max.Y)
      std::swap(min.Y, max.Y);
    if(min.Z > max.Z)
      std::swap(min.Z, max.Z);
  }

  [[nodiscard]] bool contains(const TRVec& v) const noexcept
  {
    return v.X >= min.X && v.X <= max.X && v.Y >= min.Y && v.Y <= max.Y && v.Z >= min.Z && v.Z <= max.Z;
  }

  TRVec min{0_len, 0_len, 0_len};

  TRVec max{0_len, 0_len, 0_len};

  [[nodiscard]] bool intersects(const core::BoundingBox& b) const noexcept
  {
    return min.X < b.max.X && max.X > b.min.X && min.Y < b.max.Y && max.Y > b.min.Y && min.Z < b.max.Z
           && max.Z > b.min.Z;
  }
};
} // namespace core
