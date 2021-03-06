#pragma once

#include "modelobject.h"

namespace engine::objects
{
class WaterfallMist final : public NullRenderModelObject
{
public:
  WaterfallMist(const gsl::not_null<world::World*>& world, const core::RoomBoundPosition& position)
      : NullRenderModelObject{world, position}
  {
  }

  WaterfallMist(const gsl::not_null<world::World*>& world,
                const gsl::not_null<const world::Room*>& room,
                const loader::file::Item& item,
                const gsl::not_null<const world::SkeletalModelType*>& animatedModel)
      : NullRenderModelObject{world, room, item, true, animatedModel}
  {
  }

  void update() override;
};
} // namespace engine::objects
