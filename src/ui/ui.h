#pragma once

#include <array>
#include <gl/pixel.h>
#include <gl/soglb_fwd.h>
#include <gsl/gsl-lite.hpp>
#include <memory>
#include <utility>
#include <vector>

namespace render::scene
{
class Mesh;
class Material;
} // namespace render::scene

namespace engine::world
{
struct Sprite;
}

namespace ui
{
struct BoxGouraud;

class Ui final
{
public:
  struct UiVertex
  {
    glm::vec2 pos;
    glm::vec2 uv;
    glm::int32_t texIndex;
    glm::vec4 topLeft{0};
    glm::vec4 topRight{0};
    glm::vec4 bottomLeft{0};
    glm::vec4 bottomRight{0};

    static std::shared_ptr<gl::VertexBuffer<Ui::UiVertex>> createVertexBuffer();
    static std::shared_ptr<gl::ElementArrayBuffer<uint16_t>> createIndexBuffer();
  };

  explicit Ui(std::shared_ptr<render::scene::Material> material, const std::array<gl::SRGBA8, 256>& palette);

  void drawOutlineBox(const glm::ivec2& xy, const glm::ivec2& size, uint8_t alpha = 255);
  void drawBox(const glm::ivec2& xy, const glm::ivec2& size, const BoxGouraud& gouraud);
  void drawBox(const glm::ivec2& xy, const glm::ivec2& size, const gl::SRGBA8& color);
  void drawBox(const glm::ivec2& xy, const glm::ivec2& size, const size_t color)
  {
    drawBox(xy, size, m_palette.at(color));
  }

  void drawHLine(const glm::ivec2& xy, int length, const gl::SRGBA8& color);
  void drawVLine(const glm::ivec2& xy, int length, const gl::SRGBA8& color);
  void draw(const engine::world::Sprite& sprite, const glm::ivec2& xy);

  void render(const glm::vec2& screenSize);

private:
  const std::shared_ptr<render::scene::Material> m_material;
  const std::array<gl::SRGBA8, 256> m_palette;
  std::vector<UiVertex> m_vertices{};
};
} // namespace ui
