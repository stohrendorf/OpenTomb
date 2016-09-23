#pragma once

#include "Drawable.h"
#include "Rectangle.h"
#include "Vector4.h"
#include "SpriteBatch.h"
#include "Effect.h"


namespace gameplay
{
    /**
     * Defines a sprite for rendering a 2D region.
     *
     * A sprite has alignment for controlling the local offset bounds.
     * A sprite has an anchor for controlling the origin for rotation and physics.
     *
     * You can add transformation and hierarchial by attaching sprites to Nodes within a Scene.
     *
     * The active camera in a scene effects the rendering location in the viewport for each sprite.
     *
     * Sprites can be animated using the animation system.
     * Sprites can have physics applied to them via their node binding.
     */
    class Sprite : public Drawable
    {
        friend class Node;

    public:
        Sprite();
        ~Sprite();

        /**
         * Defines the offset for position.
         */
        enum Offset
        {
            OFFSET_LEFT = 0x01,
            OFFSET_HCENTER = 0x02,
            OFFSET_RIGHT = 0x04,
            OFFSET_TOP = 0x10,
            OFFSET_VCENTER = 0x20,
            OFFSET_BOTTOM = 0x40,
            OFFSET_ANCHOR = 0x80,
            OFFSET_TOP_LEFT = OFFSET_TOP | OFFSET_LEFT,
            OFFSET_VCENTER_LEFT = OFFSET_VCENTER | OFFSET_LEFT,
            OFFSET_BOTTOM_LEFT = OFFSET_BOTTOM | OFFSET_LEFT,
            OFFSET_TOP_HCENTER = OFFSET_TOP | OFFSET_HCENTER,
            OFFSET_VCENTER_HCENTER = OFFSET_VCENTER | OFFSET_HCENTER,
            OFFSET_BOTTOM_HCENTER = OFFSET_BOTTOM | OFFSET_HCENTER,
            OFFSET_TOP_RIGHT = OFFSET_TOP | OFFSET_RIGHT,
            OFFSET_VCENTER_RIGHT = OFFSET_VCENTER | OFFSET_RIGHT,
            OFFSET_BOTTOM_RIGHT = OFFSET_BOTTOM | OFFSET_RIGHT
        };


        /**
         * Defines the flip flags used for rendering the sprite.
         */
        enum FlipFlags
        {
            FLIP_NONE,
            FLIP_VERTICAL,
            FLIP_HORIZONTAL
        };


        /**
         * Defines the blend modes.
         */
        enum BlendMode
        {
            BLEND_NONE,
            BLEND_ALPHA,
            BLEND_ADDITIVE,
            BLEND_MULTIPLIED
        };


        /**
         * Gets the width of the sprite.
         *
         * @return The width of the sprite.
         */
        float getWidth() const;

        /**
         * Gets the height of the sprite.
         *
         * @return The height of the sprite.
         */
        float getHeight() const;

        /**
         * Sets the offset used for how much to locally adjust the bounds of the sprite.
         *
         * Note: The default offset is Offset::OFFSET_BOTTOM_LEFT
         *
         * @param offset The offset used for how much to locally adjust the bounds of the sprite.
         */
        void setOffset(Offset offset);

        /**
         * Gets the offset used for how much to locally adjust the bounds of the sprite.
         *
         * Note: The default offset is Offset::OFFSET_BOTTOM_LEFT
         *
         * @return The offset used for how much to locally adjust the bounds of the sprite.
         */
        Offset getOffset() const;

        /**
         * Sets the anchor which is a origin ratio of the sprite width and height from [0.0,1.0].
         *
         * @param anchor The anchor which is a origin ratio of the sprite width and height from [0.0,1.0].
         */
        void setAnchor(const Vector2& anchor);

        /**
         * Gets the anchor which is a origin ratio of the sprite width and height from [0.0,1.0].
         *
         * @return The anchor which is a origin ratio of the sprite width and height from [0.0,1.0].
         */
        const Vector2& getAnchor() const;

        /**
         * Sets the flip flags used for rendering the sprite.
         *
         * @param flipFlags The flip flags used for rendering the sprite.
         * @see Sprite::FlipFlags
         */
        void setFlip(int flipFlags);

        /**
         * Gets the flip flags used for rendering the sprite.
         *
         * @return The flip flags used for rendering the sprite.
         * @see Sprite::FlipFlags
         */
        int getFlip() const;

        /**
         * Sets the source region from the source image.
         *
         * @param frameIndex The frame index to specify the source region for.
         * @param source The source clip region from the source image.
         */
        void setFrameSource(unsigned int frameIndex, const Rectangle& source);

        /**
         * Gets the source region from the source image.
         *
         * @param frameIndex The frame index to get the source region from.
         * @return The source clip region from the source image.
         */
        const Rectangle& getFrameSource(unsigned int frameIndex) const;

        /**
         * Computes the source frames for sprites with frameCount > 1.
         *
         * This walks the image starting from the first source region and traverses
         * the image moving across the image by the frame stride and then wrapping at the
         * edges of the image.
         *
         * @param frameStride The number of frames to move across before moving down or wrapping to the top.
         * @param framePadding The number of pixels used as padding between frame.
         */
        void computeFrames(unsigned int frameStride = 0, unsigned int framePadding = 1);

        /**
         * Gets the number of frames this sprite can render.
         *
         * @return The total number of frames this sprite can render.
         */
        unsigned int getFrameCount() const;

        /**
         * Gets the number of frames to travel across before
         * wrapping to the next row or column.
         *
         * @return The total number of frames this sprite can render.
         */
        unsigned int getFrameStride() const;

        /**
         * Gets the source padding in pixels around the source region.
         *
         * @return The source flip flags from the source image.
         * @see Sprite::FlipFlags
         */
        unsigned int getFramePadding() const;

        /**
         * Sets the current frame index to be rendered.
         *
         * @param index The current frame index to be rendered.
         */
        void setFrameIndex(unsigned int index);

        /**
         * Gets the current frame index to be rendered.
         *
         * @return The current frame index to be rendered.
         */
        unsigned int getFrameIndex() const;

        /**
         * Sets the opacity for the sprite.
         *
         * The range is from full transparent to opaque [0.0,1.0].
         *
         * @param opacity The opacity for the sprite.
         */
        void setOpacity(float opacity);

        /**
         * Gets the opacity for the sprite.
         *
         * The range is from full transparent to opaque [0.0,1.0].
         *
         * @return The opacity for the sprite.
         */
        float getOpacity() const;

        /**
         * Sets the color (RGBA) for the sprite.
         *
         * @param color The color(RGBA) for the sprite.
         */
        void setColor(const Vector4& color);

        /**
         * Gets the color (RGBA) for the sprite.
         *
         * @return The color(RGBA) for the sprite.
         */
        const Vector4& getColor() const;

        /**
         * Sets the blend mode for the sprite.
         *
         * @param mode The blend mode for the sprite.
         * @see Sprite::BlendMode
         */
        void setBlendMode(BlendMode mode);

        /**
         * Gets the blend mode for the sprite.
         *
         * @return The blend mode for the sprite.
         * @see Sprite::BlendMode
         */
        BlendMode getBlendMode() const;

        /**
         * Gets the texture sampler used when sampling the texture.
         * This can be modified for controlling sampler setting such as
         * filtering modes.
         *
         * @return The texture sampler used when sampling the texture.
         */
        const std::shared_ptr<Texture::Sampler>& getSampler() const;

        /**
         * Gets the StateBlock for the SpriteBatch.
         *
         * The returned state block controls the renderer state used when drawing items
         * with this sprite batch. Modification can be made to the returned state block
         * to change how primitives are rendered.
         *
         * @return The StateBlock for this SpriteBatch.
         */
        std::shared_ptr<RenderState::StateBlock> getStateBlock() const;

        /**
         * Gets the material used by sprite batch.
         *
         * @return The material used by the sprite batch.
         */
        const std::shared_ptr<Material>& getMaterial() const;

        /**
         * @see Drawable::draw
         */
        size_t draw(bool wireframe = false) override;

        static std::shared_ptr<Sprite> create(const std::shared_ptr<Texture>& texture, float width, float height, const Rectangle& source, unsigned frameCount = 1, const std::shared_ptr<ShaderProgram>& shaderProgram = nullptr);
    protected:

        Sprite& operator=(const Sprite& sprite) = delete;

    private:

        float _width;
        float _height;
        Offset _offset;
        Vector2 _anchor;
        int _flipFlags;
        Rectangle* _frames;
        unsigned int _frameCount;
        unsigned int _frameStride;
        unsigned int _framePadding;
        unsigned int _frameIndex;
        SpriteBatch* _batch;
        float _opacity;
        Vector4 _color;
        BlendMode _blendMode;
    };
}