#include "larastatehandler.h"

#include "defaultanimdispatcher.h"
#include "trcamerascenenodeanimator.h"
#include "heightinfo.h"
#include "larastate.h"
#include "abstractstatehandler.h"

void LaraStateHandler::setTargetState(LaraStateId st)
{
    m_dispatcher->setTargetState(static_cast<uint16_t>(st));
}

loader::LaraStateId LaraStateHandler::getTargetState() const
{
    return static_cast<LaraStateId>(m_dispatcher->getTargetState());
}

void LaraStateHandler::playAnimation(loader::AnimationId anim, const boost::optional<irr::u32>& firstFrame)
{
    m_dispatcher->playLocalAnimation(static_cast<uint16_t>(anim), firstFrame);
}

void LaraStateHandler::applyRotation()
{
    //! @todo This is horribly inefficient code, but it properly converts ZXY angles to XYZ angles.
    irr::core::quaternion q;
    q.makeIdentity();
    q *= irr::core::quaternion().fromAngleAxis(util::auToRad(getRotation().Y), {0,1,0});
    q *= irr::core::quaternion().fromAngleAxis(util::auToRad(getRotation().X), {1,0,0});
    q *= irr::core::quaternion().fromAngleAxis(util::auToRad(getRotation().Z), {0,0,-1});

    irr::core::vector3df euler;
    q.toEuler(euler);
    m_lara->setRotation(euler * 180 / irr::core::PI);
}

void LaraStateHandler::handleLaraStateOnLand(bool newFrame)
{
    LaraState laraState;
    laraState.position = getExactPosition();
    laraState.collisionRadius = 100; //!< @todo MAGICK 100
    laraState.frobbelFlags = LaraState::FrobbelFlag10 | LaraState::FrobbelFlag08;

    std::unique_ptr<AbstractStateHandler> nextHandler = nullptr;
    if( newFrame )
    {
        //BOOST_LOG_TRIVIAL(debug) << "Input state: " << loader::toString(m_currentStateHandler->getId());
        nextHandler = m_currentStateHandler->handleInput(laraState);
    }

    m_currentStateHandler->animate(laraState, getCurrentDeltaTime());

    if( nextHandler != nullptr )
    {
        m_currentStateHandler = std::move(nextHandler);
        BOOST_LOG_TRIVIAL(debug) << "New input state override: " << loader::toString(m_currentStateHandler->getId());
    }

    // "slowly" revert rotations to zero
    if( getRotation().Z < 0 )
    {
        m_rotation.Z += makeSpeedValue(182).getScaledExact(getCurrentDeltaTime());
        if( getRotation().Z >= 0 )
            m_rotation.Z = 0;
    }
    else if( getRotation().Z > 0 )
    {
        m_rotation.Z -= makeSpeedValue(182).getScaledExact(getCurrentDeltaTime());
        if( getRotation().Z <= 0 )
            m_rotation.Z = 0;
    }

    if( getYRotationSpeed() < 0 )
    {
        m_yRotationSpeed.addExact(364, getCurrentDeltaTime()).limitMax(0);
    }
    else if( getYRotationSpeed() > 0 )
    {
        m_yRotationSpeed.subExact(364, getCurrentDeltaTime()).limitMin(0);
    }
    else
    {
        setYRotationSpeed(0);
    }

    m_rotation.Y += m_yRotationSpeed.getScaledExact(getCurrentDeltaTime());

    applyRotation();

    if( !newFrame )
        return;

    //BOOST_LOG_TRIVIAL(debug) << "Post-processing state: " << loader::toString(m_currentStateHandler->getId());

    auto animCommandOverride = processAnimCommands();
    if( animCommandOverride )
    {
        m_currentStateHandler = std::move(animCommandOverride);
        BOOST_LOG_TRIVIAL(debug) << "New anim command state override: " << loader::toString(m_currentStateHandler->getId());
    }

    // @todo test interactions

    nextHandler = m_currentStateHandler->postprocessFrame(laraState);
    if( nextHandler != nullptr )
    {
        m_currentStateHandler = std::move(nextHandler);
        BOOST_LOG_TRIVIAL(debug) << "New post-processing state override: " << loader::toString(m_currentStateHandler->getId());
    }

    updateFloorHeight(-381);
    handleTriggers(laraState.current.floor.lastTriggerOrKill, false);
}

void LaraStateHandler::handleLaraStateDiving(bool newFrame)
{
    LaraState laraState;
    laraState.position = getExactPosition();
    laraState.collisionRadius = 300; //!< @todo MAGICK 300
    laraState.frobbelFlags &= ~(LaraState::FrobbelFlag10 | LaraState::FrobbelFlag08 | LaraState::FrobbelFlag_UnwalkableDeadlyFloor | LaraState::FrobbelFlag_UnwalkableSteepFloor | LaraState::FrobbelFlag_UnpassableSteepUpslant);
    laraState.neededCeilingDistance = 400;
    laraState.neededFloorDistanceBottom = loader::HeightLimit;
    laraState.neededFloorDistanceTop = -400;

    std::unique_ptr<AbstractStateHandler> nextHandler = nullptr;
    if(newFrame)
    {
        //BOOST_LOG_TRIVIAL(debug) << "Input state: " << loader::toString(m_currentStateHandler->getId());
        nextHandler = m_currentStateHandler->handleInput(laraState);
    }

    m_currentStateHandler->animate(laraState, getCurrentDeltaTime());

    if(nextHandler != nullptr)
    {
        m_currentStateHandler = std::move(nextHandler);
        BOOST_LOG_TRIVIAL(debug) << "New input state override: " << loader::toString(m_currentStateHandler->getId());
    }

    // "slowly" revert rotations to zero
    if(m_rotation.Z < 0)
    {
        m_rotation.Z += makeSpeedValue(364).getScaledExact(getCurrentDeltaTime());
        if(m_rotation.Z >= 0)
            m_rotation.Z = 0;
    }
    else if(m_rotation.Z > 0)
    {
        m_rotation.Z -= makeSpeedValue(364).getScaledExact(getCurrentDeltaTime());
        if(m_rotation.Z <= 0)
            m_rotation.Z = 0;
    }
    m_rotation.X = irr::core::clamp(m_rotation.X, -18200.0f, 18200.0f); // 100 degrees
    m_rotation.Z = irr::core::clamp(m_rotation.Z, -4004.0f, 4004.0f); // 22 degrees

    m_position.X += m_fallSpeed.getScaledExact(getCurrentDeltaTime())/4 * std::sin(util::auToRad(m_rotation.Y)) * std::cos(util::auToRad(m_rotation.X));
    m_position.Y -= m_fallSpeed.getScaledExact(getCurrentDeltaTime())/4                                         * std::sin(util::auToRad(m_rotation.X));
    m_position.Z += m_fallSpeed.getScaledExact(getCurrentDeltaTime())/4 * std::cos(util::auToRad(m_rotation.Y)) * std::cos(util::auToRad(m_rotation.X));

    applyRotation();
    m_lara->setPosition(m_position.toIrrlicht());
    m_lara->updateAbsolutePosition();

    if(!newFrame)
        return;

    auto animCommandOverride = processAnimCommands();
    if(animCommandOverride)
    {
        m_currentStateHandler = std::move(animCommandOverride);
        BOOST_LOG_TRIVIAL(debug) << "New anim command state override: " << loader::toString(m_currentStateHandler->getId());
    }

    // @todo test interactions

    nextHandler = m_currentStateHandler->postprocessFrame(laraState);
    if(nextHandler != nullptr)
    {
        m_currentStateHandler = std::move(nextHandler);
        BOOST_LOG_TRIVIAL(debug) << "New post-processing state override: " << loader::toString(m_currentStateHandler->getId());
    }

    updateFloorHeight(0);
    handleTriggers(laraState.current.floor.lastTriggerOrKill, false);
}

void LaraStateHandler::handleLaraStateSwimming(bool newFrame)
{
    throw std::runtime_error("Swimming handler not yet implemented");
}

irr::u32 LaraStateHandler::getCurrentFrame() const
{
    return m_dispatcher->getCurrentFrame();
}

irr::u32 LaraStateHandler::getAnimEndFrame() const
{
    return m_dispatcher->getAnimEndFrame();
}

void LaraStateHandler::placeOnFloor(const LaraState& state)
{
    m_position.Y += state.current.floor.distance;
}

loader::LaraStateId LaraStateHandler::getCurrentState() const
{
    return m_currentStateHandler->getId();
}

loader::LaraStateId LaraStateHandler::getCurrentAnimState() const
{
    return static_cast<loader::LaraStateId>(m_dispatcher->getCurrentAnimState());
}

LaraStateHandler::~LaraStateHandler() = default;

void LaraStateHandler::animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs)
{
    BOOST_ASSERT(m_lara == node);

    if( m_lastFrameTime < 0 )
        m_lastFrameTime = m_lastEngineFrameTime = m_currentFrameTime = timeMs;

    if( m_lastFrameTime == timeMs )
        return;

    m_currentFrameTime = timeMs;

    static constexpr int FrameTime = 1000 / 30;

    bool isNewFrame = m_lastAnimFrame != getCurrentFrame();

    if( timeMs - m_lastEngineFrameTime >= FrameTime )
    {
        isNewFrame = true;
        m_lastEngineFrameTime -= (timeMs - m_lastEngineFrameTime) / FrameTime * FrameTime;
    }

    if(m_currentStateHandler == nullptr)
    {
        m_currentStateHandler = AbstractStateHandler::create(getCurrentAnimState(), *this);
    }

    if(m_underwaterState == UnderwaterState::OnLand && m_level->m_camera->getCurrentRoom()->isWaterRoom())
    {
        m_air = 1800;
        m_underwaterState = UnderwaterState::Diving;
        m_falling = false;
        m_position.Y += 100;
        updateFloorHeight(0);
        //! @todo stop sound 30
        if(getCurrentAnimState() == LaraStateId::SwandiveBegin)
        {
            m_rotation.X = -util::degToAu(45);
            setTargetState(LaraStateId::UnderwaterDiving);
            if(auto tmp = processAnimCommands())
                m_currentStateHandler = std::move(tmp);
            m_fallSpeed = m_fallSpeed.get() * 2;
        }
        else if(getCurrentAnimState() == LaraStateId::SwandiveEnd)
        {
            m_rotation.X = -util::degToAu(85);
            setTargetState(LaraStateId::UnderwaterDiving);
            if(auto tmp = processAnimCommands())
                m_currentStateHandler = std::move(tmp);
            m_fallSpeed = m_fallSpeed.get() * 2;
        }
        else
        {
            m_rotation.X = -util::degToAu(45);
            playAnimation(loader::AnimationId::FREE_FALL_TO_UNDERWATER, 1895);
            setTargetState(LaraStateId::UnderwaterForward);
            m_currentStateHandler = AbstractStateHandler::create(LaraStateId::UnderwaterDiving, *this);
            if(auto tmp = processAnimCommands())
                m_currentStateHandler = std::move(tmp);
            m_fallSpeed = m_fallSpeed.get() * 3 / 2;
        }

        //! @todo jumpIntoWater();
    }
    else if(m_underwaterState == UnderwaterState::Diving && !m_level->m_camera->getCurrentRoom()->isWaterRoom())
    {
        auto waterSurfaceHeight = getWaterSurfaceHeight();
        if(!waterSurfaceHeight || std::abs(*waterSurfaceHeight - m_position.Y) >= 256)
        {
            m_underwaterState = UnderwaterState::OnLand;
            playAnimation(loader::AnimationId::FREE_FALL_FORWARD, 492);
            setTargetState(LaraStateId::JumpForward);
            m_currentStateHandler = AbstractStateHandler::create(LaraStateId::JumpForward, *this);
            m_fallSpeed = 0;
            //! @todo Check formula
            m_horizontalSpeed = m_horizontalSpeed.get() / 5;
            m_falling = true;
            m_handStatus = 0;
            m_rotation.X = m_rotation.Z = 0;
        }
        else
        {
            m_underwaterState = UnderwaterState::Swimming;
            playAnimation(loader::AnimationId::UNDERWATER_TO_ONWATER, 1937);
            setTargetState(LaraStateId::OnWaterStop);
            m_currentStateHandler = AbstractStateHandler::create(LaraStateId::OnWaterStop, *this);
            m_handStatus = 0;
            m_rotation.X = m_rotation.Z = 0;
            m_position.Y = *waterSurfaceHeight + 1;
            m_swimToDiveKeypressDuration = 11;
            updateFloorHeight(-381);
            //! @todo play sound 36
        }
    }
    else if(m_underwaterState == UnderwaterState::Swimming && !m_level->m_camera->getCurrentRoom()->isWaterRoom())
    {
        m_underwaterState = UnderwaterState::OnLand;
        playAnimation(loader::AnimationId::FREE_FALL_FORWARD, 492);
        setTargetState(LaraStateId::JumpForward);
        m_currentStateHandler = AbstractStateHandler::create(LaraStateId::JumpForward, *this);
        m_fallSpeed = 0;
        //! @todo Check formula
        m_horizontalSpeed = m_horizontalSpeed.get() / 5;
        m_falling = true;
        m_handStatus = 0;
        m_rotation.X = m_rotation.Z = 0;
    }

    if(m_underwaterState == UnderwaterState::OnLand)
    {
        m_air = 1800;
        handleLaraStateOnLand(isNewFrame);
    }
    else if(m_underwaterState == UnderwaterState::Diving)
    {
        if(m_health.get() >= 0)
        {
            m_air.sub(1, getCurrentDeltaTime());
            if(m_air.get() < 0)
            {
                m_air = -1;
                m_health.sub(5, getCurrentDeltaTime());
            }
        }
        handleLaraStateDiving(isNewFrame);
    }
    else if(m_underwaterState == UnderwaterState::Swimming)
    {
        if(m_health.get() >= 0)
        {
            m_air.add(10, getCurrentDeltaTime()).limitMax(1800);
        }
        handleLaraStateSwimming(isNewFrame);
    }

    m_lastFrameTime = m_currentFrameTime;
}

std::unique_ptr<AbstractStateHandler> LaraStateHandler::processAnimCommands()
{
    std::unique_ptr<AbstractStateHandler> nextHandler = nullptr;
    bool newFrame = false;
    if( m_dispatcher->handleTRTransitions() || m_lastAnimFrame != getCurrentFrame() )
    {
        nextHandler = m_currentStateHandler->createWithRetainedAnimation(getCurrentAnimState());
        m_lastAnimFrame = getCurrentFrame();
        newFrame = true;
    }

    const bool isAnimEnd = getCurrentFrame() >= getAnimEndFrame();

    const loader::Animation& animation = getLevel().m_animations[m_dispatcher->getCurrentAnimationId()];
    if( animation.animCommandCount > 0 )
    {
        BOOST_ASSERT(animation.animCommandIndex < getLevel().m_animCommands.size());
        const auto* cmd = &getLevel().m_animCommands[animation.animCommandIndex];
        for( uint16_t i = 0; i < animation.animCommandCount; ++i )
        {
            BOOST_ASSERT(cmd < &getLevel().m_animCommands.back());
            const auto opcode = static_cast<AnimCommandOpcode>(*cmd);
            ++cmd;
            switch( opcode )
            {
            case AnimCommandOpcode::SetPosition:
                if( isAnimEnd && newFrame )
                {
                    moveLocal(
                              cmd[0],
                              cmd[1],
                              cmd[2]
                             );
                }
                cmd += 3;
                break;
            case AnimCommandOpcode::SetVelocity:
                if( isAnimEnd && newFrame )
                {
                    setFallSpeed(m_fallSpeedOverride == 0 ? cmd[0] : m_fallSpeedOverride);
                    m_fallSpeedOverride = 0;
                    setHorizontalSpeed(cmd[1]);
                    setFalling(true);
                }
                cmd += 2;
                break;
            case AnimCommandOpcode::EmptyHands:
                if( isAnimEnd )
                {
                    setHandStatus(0);
                }
                break;
            case AnimCommandOpcode::PlaySound:
                if( getCurrentFrame() == cmd[0] )
                {
                    //! @todo playsound(cmd[1])
                }
                cmd += 2;
                break;
            case AnimCommandOpcode::PlayEffect:
                if( getCurrentFrame() == cmd[0] )
                {
                    BOOST_LOG_TRIVIAL(debug) << "Anim effect: " << int(cmd[1]);
                    if( cmd[1] == 0 && newFrame )
                        m_rotation.Y += util::degToAu(180);
                    else if( cmd[1] == 12 )
                        setHandStatus(0);
                    //! @todo Execute anim effect cmd[1]
                }
                cmd += 2;
                break;
            default:
                break;
            }
        }
    }

    if( m_falling )
    {
        m_horizontalSpeed.addExact(m_dispatcher->getAccelleration(), getCurrentDeltaTime());
        if( getFallSpeed().get() >= 128 )
            m_fallSpeed.addExact(1, getCurrentDeltaTime());
        else
            m_fallSpeed.addExact(6, getCurrentDeltaTime());
    }
    else
    {
        m_horizontalSpeed = m_dispatcher->calculateFloorSpeed();
    }

    move(
         std::sin(util::auToRad(getMovementAngle())) * m_horizontalSpeed.getScaledExact(getCurrentDeltaTime()),
         m_fallSpeed.getScaledExact(getCurrentDeltaTime()),
         std::cos(util::auToRad(getMovementAngle())) * m_horizontalSpeed.getScaledExact(getCurrentDeltaTime())
        );

    m_lara->setPosition(m_position.toIrrlicht());
    m_lara->updateAbsolutePosition();

    return nextHandler;
}

void LaraStateHandler::updateFloorHeight(int dy)
{
    auto pos = getPosition();
    pos.Y += dy;
    auto room = getLevel().m_camera->getCurrentRoom();
    auto sector = getLevel().findSectorForPosition(pos, &room);
    m_level->m_camera->setCurrentRoom(room);
    HeightInfo hi = HeightInfo::fromFloor(sector, pos, getLevel().m_camera);
    setFloorHeight(hi.distance);
}

void LaraStateHandler::handleTriggers(const uint16_t* floorData, bool isDoppelganger)
{
    if( floorData == nullptr )
        return;

    if( loader::extractFDFunction(*floorData) == loader::FDFunction::Death )
    {
        if( !isDoppelganger )
        {
            if( irr::core::equals(getPosition().Y, getFloorHeight(), 1) )
            {
                //! @todo kill Lara
            }
        }

        if( loader::isLastFloordataEntry(*floorData) )
            return;

        ++floorData;
    }

    const auto triggerType = loader::extractTriggerType(*floorData);
    const auto triggerArg = floorData[1];
    auto nextFloorData = floorData + 2;

    //! @todo Find camera target if necessary

    bool doTrigger = false;
    if( !isDoppelganger )
    {
        switch( triggerType )
        {
        case loader::TriggerType::Trigger:
            break;
        case loader::TriggerType::Pad:
        case loader::TriggerType::AntiPad:
            doTrigger = getPosition().Y == getFloorHeight();
            break;
        case loader::TriggerType::Switch:
            //! @todo Handle switch
            ++nextFloorData;
            doTrigger = true;
            return;
        case loader::TriggerType::Key:
            //! @todo Handle key
            ++nextFloorData;
            doTrigger = true;
            return;
        case loader::TriggerType::Pickup:
            //! @todo Handle pickup
            ++nextFloorData;
            doTrigger = true;
            return;
        case loader::TriggerType::Combat:
            doTrigger = getHandStatus() == 4;
            break;
        case loader::TriggerType::Heavy:
        case loader::TriggerType::Dummy:
            return;
        default:
            doTrigger = true;
            break;
        }
    }
    else
    {
        doTrigger = triggerType == loader::TriggerType::Heavy;
    }

    if(!doTrigger)
        return;

    while(true)
    {
        const bool isLast = loader::isLastFloordataEntry(*nextFloorData);
        switch(loader::extractTriggerFunction(*nextFloorData))
        {
        case loader::TriggerFunction::Object:
            ++nextFloorData;
            //! @todo handle object
            break;
        case loader::TriggerFunction::CameraTarget:
            ++nextFloorData;
            ++nextFloorData;
            //! @todo handle camera target
            break;
        case loader::TriggerFunction::LookAt:
            ++nextFloorData;
            //! @todo handle "look at"
            break;
        case loader::TriggerFunction::UnderwaterCurrent:
            ++nextFloorData;
            //! @todo handle underwater current
            break;
        case loader::TriggerFunction::FlipMap:
            ++nextFloorData;
            //! @todo handle flip map
            break;
        case loader::TriggerFunction::FlipOn:
            ++nextFloorData;
            //! @todo handle flip on
            break;
        case loader::TriggerFunction::FlipOff:
            ++nextFloorData;
            //! @todo handle flip off
            break;
        case loader::TriggerFunction::FlipEffect:
            ++nextFloorData;
            //! @todo handle flip effect
            break;
        case loader::TriggerFunction::EndLevel:
            ++nextFloorData;
            //! @todo handle level end
            break;
        case loader::TriggerFunction::PlayTrack:
            ++nextFloorData;
            //! @todo handle "play track"
            break;
        case loader::TriggerFunction::Secret:
            ++nextFloorData;
            //! @todo handle secrets
            break;
        default:
            ++nextFloorData;
            break;
        }

        if(isLast)
            break;
    }

    //! @todo Implement the rest
}

irr::core::aabbox3di LaraStateHandler::getBoundingBox() const
{
    return m_dispatcher->getBoundingBox();
}

boost::optional<int> LaraStateHandler::getWaterSurfaceHeight() const
{
    auto sector = m_level->m_camera->getCurrentRoom()->getSectorByAbsolutePosition(m_position.toInexact());

    if(m_level->m_camera->getCurrentRoom()->isWaterRoom())
    {
        while(true)
        {
            if(sector->roomAbove == 0xff)
                break;

            BOOST_ASSERT(sector->roomAbove < m_level->m_rooms.size());
            const auto& room = m_level->m_rooms[sector->roomAbove];
            if(!room.isWaterRoom())
                break;

            sector = room.getSectorByAbsolutePosition(m_position.toInexact());
        }

        return sector->ceilingHeight * loader::QuarterSectorSize;
    }
    else
    {
        while(true)
        {
            if(sector->roomBelow == 0xff)
                break;

            BOOST_ASSERT(sector->roomBelow < m_level->m_rooms.size());
            const auto& room = m_level->m_rooms[sector->roomBelow];
            if(room.isWaterRoom())
            {
                return sector->floorHeight * loader::QuarterSectorSize;
            }

            sector = room.getSectorByAbsolutePosition(m_position.toInexact());
        }

        return sector->ceilingHeight * loader::QuarterSectorSize;
    }

    return boost::none;
}
