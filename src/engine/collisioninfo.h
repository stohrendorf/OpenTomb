#pragma once

#include "core/angle.h"
#include "heightinfo.h"
#include "type_safe/flag_set.hpp"

#include <set>

namespace engine::world
{
class World;
}

namespace engine
{
namespace objects
{
class LaraObject;
}

struct CollisionInfo
{
  enum class AxisColl
  {
    None,
    Front,
    Left,
    Right,
    Top,
    TopBottom,
    TopFront
  };

  enum class PolicyFlags
  {
    SlopesAreWalls,
    SlopesArePits,
    LavaIsPit,
    EnableBaddiePush,
    EnableSpaz,
    _flag_set_size [[maybe_unused]]
  };

  using PolicyFlagSet = type_safe::flag_set<PolicyFlags>;
  static constexpr const type_safe::flag_combo<PolicyFlags> SlopeBlockingPolicy
    = PolicyFlags::SlopesAreWalls | PolicyFlags::SlopesArePits;
  static constexpr const type_safe::flag_combo<PolicyFlags> SpazPushPolicy
    = PolicyFlags::EnableBaddiePush | PolicyFlags::EnableSpaz;

  AxisColl collisionType = AxisColl::None;
  mutable core::TRVec shift;
  core::Axis facingAxis = core::Axis::PosZ;
  core::Angle facingAngle = 0_deg;      // external
  core::Length collisionRadius = 0_len; // external
  PolicyFlagSet policies;               // external
  core::TRVec initialPosition;          // external
  //! The deepest floor distance considered passable.
  core::Length badPositiveDistance = 0_len; // external
  //! The highest floor distance considered passable.
  core::Length badNegativeDistance = 0_len; // external
  core::Length badCeilingDistance = 0_len;  // external

  VerticalSpaceInfo mid;
  VerticalSpaceInfo front;
  VerticalSpaceInfo frontLeft;
  VerticalSpaceInfo frontRight;

  int8_t floorSlantX = 0;
  int8_t floorSlantZ = 0;

  bool hasStaticMeshCollision = false;

  void initHeightInfo(const core::TRVec& laraPos, const world::World& world, const core::Length& height);

  static std::set<gsl::not_null<const world::Room*>> collectTouchingRooms(const core::TRVec& position,
                                                                          const core::Length& radius,
                                                                          const core::Length& height,
                                                                          const world::World& world);

  bool checkStaticMeshCollisions(const core::TRVec& pokePosition,
                                 const core::Length& pokeHeight,
                                 const world::World& world);
};

inline gsl::czstring toString(CollisionInfo::AxisColl value)
{
  switch(value)
  {
  case CollisionInfo::AxisColl::None: return "None";
  case CollisionInfo::AxisColl::Front: return "Front";
  case CollisionInfo::AxisColl::Left: return "Left";
  case CollisionInfo::AxisColl::Right: return "Right";
  case CollisionInfo::AxisColl::Top: return "Top";
  case CollisionInfo::AxisColl::TopBottom: return "TopBottom";
  case CollisionInfo::AxisColl::TopFront: return "TopFront";
  default: return "<null>";
  }
}
} // namespace engine
