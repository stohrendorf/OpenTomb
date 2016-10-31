#include "Base.h"
#include "ScreenOverlay.h"
#include "Game.h"
#include "MaterialParameter.h"
#include "Image.h"

#include <glm/gtc/matrix_transform.hpp>

#include <boost/log/trivial.hpp>


namespace gameplay
{
    namespace
    {
        std::shared_ptr<ShaderProgram> screenOverlayProgram = nullptr;
    }


    ScreenOverlay::ScreenOverlay()
    {
        if( screenOverlayProgram == nullptr )
        {
            screenOverlayProgram = ShaderProgram::createFromFile("shaders/screenoverlay.vert", "shaders/screenoverlay.frag", {});
            if( screenOverlayProgram == nullptr )
            {
                BOOST_THROW_EXCEPTION(std::runtime_error("Failed to create effect for screen overlay."));
            }
        }

        resize();
    }


    void ScreenOverlay::resize()
    {
        // Update the projection matrix for our batch to match the current viewport
        const Rectangle& vp = Game::getInstance()->getViewport();

        if( vp.isEmpty() )
        BOOST_THROW_EXCEPTION(std::runtime_error("Cannot create screen overlay because the viewport is empty"));

        _image = Image::create(vp.width, vp.height);
        _image->fill({0,0,0,0});

        _texture = Texture::create(_image, false);

        _batch = SpriteBatch::create(_texture, screenOverlayProgram);
        if( _batch == nullptr )
        {
            BOOST_THROW_EXCEPTION(std::runtime_error("Failed to create batch for screen overlay."));
        }

        if( !vp.isEmpty() )
        {
            glm::mat4 projectionMatrix = glm::ortho(vp.x, vp.width, vp.height, vp.y, 0.0f, 1.0f);
            _batch->setProjectionMatrix(projectionMatrix);
        }

        _batch->getSampler()->setWrapMode(Texture::CLAMP, Texture::CLAMP);

        _batch->start();
        _batch->draw(0, 0, vp.width, vp.height, 0, 0, 1, 1, glm::vec4{1,1,1,1});
    }


    ScreenOverlay::~ScreenOverlay() = default;
}