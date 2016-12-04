#pragma once

#include "abstractstatehandler.h"
#include "engine/collisioninfo.h"
#include "engine/inputstate.h"
#include "level/level.h"

namespace engine
{
    namespace lara
    {
        class StateHandler_32 final : public AbstractStateHandler
        {
        public:
            explicit StateHandler_32(LaraNode& lara)
                    : AbstractStateHandler(lara, LaraStateId::SlideBackward)
            {
            }

            boost::optional<LaraStateId> handleInputImpl(CollisionInfo& /*collisionInfo*/) override
            {
                if( getLevel().m_inputHandler->getInputState().jump )
                    setTargetState(LaraStateId::JumpBack);

                return {};
            }

            void animateImpl(CollisionInfo& /*collisionInfo*/, const std::chrono::microseconds& /*deltaTimeMs*/) override
            {
            }

            boost::optional<LaraStateId> postprocessFrame(CollisionInfo& collisionInfo) override
            {
                setMovementAngle(getRotation().Y + 180_deg);
                return commonSlideHandling(collisionInfo);
            }
        };
    }
}