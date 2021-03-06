#pragma once

#include "render/scene/blur.h"

#include <gl/pixel.h>
#include <gl/soglb_fwd.h>
#include <glm/fwd.hpp>
#include <gsl/gsl-lite.hpp>

namespace render::scene
{
class MaterialManager;
class Camera;
class ShaderProgram;
class Material;
class Mesh;
} // namespace render::scene

namespace render::pass
{
class GeometryPass;

class HBAOPass
{
public:
  explicit HBAOPass(scene::MaterialManager& materialManager,
                    const glm::ivec2& viewport,
                    const GeometryPass& geometryPass);
  void updateCamera(const gsl::not_null<std::shared_ptr<scene::Camera>>& camera);

  void render(const glm::ivec2& size);

  [[nodiscard]] const auto& getBlurredTexture() const
  {
    return m_blur.getBlurredTexture();
  }

private:
  const std::shared_ptr<scene::Material> m_material;

  std::shared_ptr<scene::Mesh> m_renderMesh;

  std::shared_ptr<gl::Texture2D<gl::ScalarByte>> m_aoBuffer;
  std::shared_ptr<gl::TextureHandle<gl::Texture2D<gl::ScalarByte>>> m_aoBufferHandle;
  std::shared_ptr<gl::Framebuffer> m_fb;

  scene::SeparableBlur<gl::ScalarByte> m_blur;
};
} // namespace render::pass
