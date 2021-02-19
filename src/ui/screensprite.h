#pragma once

#include "loader/file/datatypes.h"

#include <gl/soglb_fwd.h>

namespace ui
{
class ScreenSprite
{
public:
  explicit ScreenSprite(const loader::file::Sprite& sprite)
      : m_sprite{sprite}
  {
  }

  void
    render(const glm::vec2& xy, const glm::vec2& screenSize, const std::shared_ptr<render::scene::Material>& material);

private:
  const loader::file::Sprite m_sprite;

  gsl::not_null<std::shared_ptr<render::scene::Mesh>> createQuad(const glm::vec2& xy, const gl::Program& program);
};
} // namespace ui