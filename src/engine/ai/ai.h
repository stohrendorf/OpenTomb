#pragma once

#include "engine/world/box.h"
#include "pathfinder.h"

#include <boost/range/adaptor/map.hpp>

namespace engine
{
namespace objects
{
class AIAgent;
}

namespace objects
{
struct ObjectState;
}

namespace ai
{
enum class Mood
{
  Bored,
  Attack,
  Escape,
  Stalk
};

inline std::ostream& operator<<(std::ostream& str, const Mood mood)
{
  switch(mood)
  {
  case Mood::Bored: return str << "Bored";
  case Mood::Attack: return str << "Attack";
  case Mood::Escape: return str << "Escape";
  case Mood::Stalk: return str << "Stalk";
  default: BOOST_THROW_EXCEPTION(std::runtime_error("Invalid mood"));
  }
}

struct AiInfo
{
  world::ZoneId zone_number = 0;
  world::ZoneId enemy_zone = 0;
  bool enemy_unreachable = false;
  core::Area distance{0};
  bool ahead = false;
  bool bite = false;
  core::Angle angle = 0_deg;
  core::Angle enemy_facing = 0_deg;

  AiInfo(world::World& world, objects::ObjectState& objectState);

  [[nodiscard]] bool canReachEnemyZone() const noexcept
  {
    return !enemy_unreachable && zone_number == enemy_zone;
  }
};

struct CreatureInfo
{
  core::Angle head_rotation = 0_deg;
  core::Angle neck_rotation = 0_deg;
  core::Angle maximum_turn = 1_deg;
  Mood mood = Mood::Bored;
  PathFinder pathFinder;
  core::TRVec target;

  CreatureInfo(const world::World& world, core::TypeId type, const gsl::not_null<const world::Box*>& initialBox);

  // serialization constructor
  explicit CreatureInfo(const world::World& world);

  void rotateHead(const core::Angle& angle)
  {
    const auto delta = std::clamp(angle - head_rotation, -5_deg, +5_deg);
    head_rotation = std::clamp(delta + head_rotation, -90_deg, +90_deg);
  }

  void serialize(const serialization::Serializer<world::World>& ser);
};

std::shared_ptr<CreatureInfo> create(const serialization::TypeId<std::shared_ptr<CreatureInfo>>&,
                                     const serialization::Serializer<world::World>& ser);

void serialize(std::shared_ptr<CreatureInfo>& data, const serialization::Serializer<world::World>& ser);

void updateMood(const world::World& world, const objects::ObjectState& objectState, const AiInfo& aiInfo, bool violent);
} // namespace ai
} // namespace engine
