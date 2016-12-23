#pragma once

#include "loader/animationids.h"
#include "loader/larastateid.h"
#include "collisioninfo.h"
#include "engine/lara/abstractstatehandler.h"
#include "engine/items/itemnode.h"
#include <chrono>


namespace engine
{
    struct CollisionInfo;


    enum class UnderwaterState
    {
        OnLand,
        Diving,
        Swimming
    };


    class LaraNode final : public items::ItemNode
    {
        using LaraStateId = loader::LaraStateId;

    private:
        // Lara's vars
        core::InterpolatedValue<float> m_health{1000.0f};
        //! @brief Additional rotation in AU per TR Engine Frame
        core::InterpolatedValue<core::Angle> m_yRotationSpeed{0_deg};
        int m_fallSpeedOverride = 0;
        core::Angle m_movementAngle{0};
        core::InterpolatedValue<float> m_air{1800.0f};
        core::Angle m_currentSlideAngle{0};

        int m_handStatus = 0;
        std::chrono::microseconds m_uvAnimTime{0};

        UnderwaterState m_underwaterState = UnderwaterState::OnLand;
        std::unique_ptr<lara::AbstractStateHandler> m_currentStateHandler = nullptr;

        bool m_handlingFrame = false;

    public:
        LaraNode(const gsl::not_null<level::Level*>& level,
                 const std::string& name,
                 const gsl::not_null<const loader::Room*>& room,
                 const core::Angle& angle,
                 const core::ExactTRCoordinates& position,
                 const loader::ActivationState& activationState,
                 int16_t darkness,
                 const loader::AnimatedModel& animatedModel)
                : ItemNode( level, name, room, angle, position, activationState, false, 0x3c, darkness, animatedModel )
        {
            setAnimIdGlobal( loader::AnimationId::STAY_IDLE );
            setTargetState( loader::LaraStateId::Stop );
            setMovementAngle( getRotation().Y );
        }


        ~LaraNode();

        void updateImpl(const std::chrono::microseconds& deltaTime, const boost::optional<FrameChangeType>& /*frameChangeType*/) override;


        bool isInWater() const
        {
            return m_underwaterState == UnderwaterState::Swimming || m_underwaterState == UnderwaterState::Diving;
        }


        bool isDiving() const
        {
            return m_underwaterState == UnderwaterState::Diving;
        }


        float getAir() const
        {
            return m_air.getCurrentValue();
        }


    private:
        void handleLaraStateOnLand(const std::chrono::microseconds& deltaTime, const boost::optional<FrameChangeType>& frameChangeType);

        void handleLaraStateDiving(const std::chrono::microseconds& deltaTime, const boost::optional<FrameChangeType>& frameChangeType);

        void handleLaraStateSwimming(const std::chrono::microseconds& deltaTime, const boost::optional<FrameChangeType>& frameChangeType);

        void testInteractions();

        //! @brief If "none", we are not allowed to dive until the "Dive" action key is released
        //! @remarks This happens e.g. just after dive-to-swim transition, when players still
        //!          keep the "Dive Forward" action key pressed; in this case, you usually won't go
        //!          diving immediately again.
        boost::optional<std::chrono::microseconds> m_swimToDiveKeypressDuration = boost::none;
        uint16_t m_secretsFoundBitmask = 0;

    public:
        const core::InterpolatedValue<float>& getHealth() const noexcept
        {
            return m_health;
        }


        void setHealth(const core::InterpolatedValue<float>& h) noexcept
        {
            m_health = h;
        }


        void setAir(const core::InterpolatedValue<float>& a) noexcept
        {
            m_air = a;
        }


        void setMovementAngle(core::Angle angle) noexcept
        {
            m_movementAngle = angle;
        }


        core::Angle getMovementAngle() const override
        {
            return m_movementAngle;
        }


        int getHandStatus() const noexcept
        {
            return m_handStatus;
        }


        void setHandStatus(int status) noexcept
        {
            m_handStatus = status;
        }


        void placeOnFloor(const CollisionInfo& collisionInfo);


        void setYRotationSpeed(core::Angle spd)
        {
            m_yRotationSpeed = spd;
        }


        core::Angle getYRotationSpeed() const
        {
            return static_cast<core::Angle>(m_yRotationSpeed);
        }


        void
        subYRotationSpeed(const std::chrono::microseconds& deltaTime, core::Angle val, core::Angle limit = -32768_au)
        {
            m_yRotationSpeed.sub( val, deltaTime ).limitMin( limit );
        }


        void
        addYRotationSpeed(const std::chrono::microseconds& deltaTime, core::Angle val, core::Angle limit = 32767_au)
        {
            m_yRotationSpeed.add( val, deltaTime ).limitMax( limit );
        }


        void setFallSpeedOverride(int v)
        {
            m_fallSpeedOverride = v;
        }


        core::Angle getCurrentSlideAngle() const noexcept
        {
            return m_currentSlideAngle;
        }


        void setCurrentSlideAngle(core::Angle a) noexcept
        {
            m_currentSlideAngle = a;
        }


        LaraStateId getTargetState() const;

        void setTargetState(loader::LaraStateId st);

        loader::LaraStateId getCurrentState() const;

        loader::LaraStateId getCurrentAnimState() const;

        void setAnimIdGlobal(loader::AnimationId anim, const boost::optional<uint16_t>& firstFrame = boost::none);

        void updateFloorHeight(int dy);

        void handleCommandSequence(const uint16_t* floorData, bool skipFirstTriggers);

        boost::optional<int> getWaterSurfaceHeight() const;


        void addSwimToDiveKeypressDuration(const std::chrono::microseconds& ms) noexcept
        {
            if( !m_swimToDiveKeypressDuration )
                return;

            *m_swimToDiveKeypressDuration += ms;
        }


        void setSwimToDiveKeypressDuration(const std::chrono::microseconds& ms) noexcept
        {
            m_swimToDiveKeypressDuration = ms;
        }


        const boost::optional<std::chrono::microseconds>& getSwimToDiveKeypressDuration() const noexcept
        {
            return m_swimToDiveKeypressDuration;
        }


        void setUnderwaterState(UnderwaterState u) noexcept
        {
            m_underwaterState = u;
        }


        void setCameraRotation(core::Angle x, core::Angle y);

        void setCameraRotationX(core::Angle x);

        void setCameraRotationY(core::Angle y);

        void setCameraDistance(int d);

        void setCameraUnknown1(int k);


        void onFrameChanged(FrameChangeType frameChangeType) override;

#ifndef NDEBUG
        CollisionInfo lastUsedCollisionInfo;
#endif
    };
}
