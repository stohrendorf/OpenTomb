#pragma once

#include "aiagent.h"
#include "engine/ai/ai.h"

namespace engine::objects
{
class Lion final : public AIAgent
{
public:
  Lion(const gsl::not_null<world::World*>& world, const core::RoomBoundPosition& position)
      : AIAgent{world, position}
  {
  }

  Lion(const gsl::not_null<world::World*>& world,
       const gsl::not_null<const world::Room*>& room,
       const loader::file::Item& item,
       const gsl::not_null<const world::SkeletalModelType*>& animatedModel)
      : AIAgent{world, room, item, animatedModel}
  {
  }

  void update() override;
};
} // namespace engine::objects
