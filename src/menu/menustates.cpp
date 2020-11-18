#include "menustates.h"

#include "engine/audioengine.h"
#include "engine/engine.h"
#include "engine/presenter.h"
#include "menudisplay.h"
#include "menuobject.h"
#include "menuring.h"

namespace menu
{
namespace
{
void rotateForSelection(MenuObject& object)
{
  if(object.rotationY == object.selectedRotationY)
    return;

  if(const auto dy = object.selectedRotationY - object.rotationY; dy > 0_deg && dy < 180_deg)
  {
    object.rotationY += 1024_au;
  }
  else
  {
    object.rotationY -= 1024_au;
  }
  object.rotationY -= object.rotationY % 1024_au;
}

void idleRotation(engine::Engine& engine, MenuObject& object)
{
  if(engine.getPresenter().getInputHandler().getInputState().xMovement == hid::AxisMovement::Null)
  {
    object.rotationY += 256_au;
  }
}

void zeroRotation(MenuObject& object, const core::Angle& speed)
{
  BOOST_ASSERT(speed >= 0_deg);

  if(object.rotationY < 0_deg)
  {
    object.rotationY += speed;
    if(object.rotationY > 0_deg)
      object.rotationY = 0_deg;
  }
  else if(object.rotationY > 0_deg)
  {
    object.rotationY -= speed;
    if(object.rotationY < 0_deg)
      object.rotationY = 0_deg;
  }
}

template<typename Unit, typename Type>
auto exactScale(const qs::quantity<Unit, Type>& value, const core::Frame& x, const core::Frame& max)
{
  const auto f = x.cast<float>() / max.cast<float>();
  return (value.template cast<float>() * f).template cast<Type>();
}

std::vector<std::filesystem::path> getSavegames(const engine::Engine& engine)
{
  if(!std::filesystem::is_directory(engine.getSavegamePath()))
    return {};

  std::vector<std::filesystem::path> result;
  for(const auto& p : std::filesystem::directory_iterator(engine.getSavegamePath()))
  {
    if(!p.is_regular_file() || p.path().extension() != ".meta")
      continue;

    result.emplace_back(p.path());
  }

  return result;
}
} // namespace

void ResetItemTransformMenuState::handleObject(engine::Engine& engine, MenuDisplay& display, MenuObject& object)
{
  if(&object == &display.getCurrentRing().getSelectedObject())
  {
    display.updateMenuObjectDescription(engine, object);
    object.baseRotationX = exactScale(object.selectedBaseRotationX, m_duration, Duration);
    object.rotationX = exactScale(object.selectedRotationX, m_duration, Duration);
    object.positionZ = exactScale(object.selectedPositionZ, m_duration, Duration);
  }
  else
  {
    zeroRotation(object, 256_au);
  }
}

std::unique_ptr<MenuState> ResetItemTransformMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/,
                                                                engine::Engine& /*engine*/,
                                                                MenuDisplay& /*display*/)
{
  if(m_duration != 0_frame)
  {
    m_duration -= 1_frame;
    return nullptr;
  }

  return std::move(m_next);
}

std::unique_ptr<MenuState> FinishItemAnimationMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/,
                                                                 engine::Engine& /*engine*/,
                                                                 MenuDisplay& display)
{
  display.updateRingTitle();

  auto& object = display.getCurrentRing().getSelectedObject();
  if(object.animate())
    return nullptr; // play full animation until its end

  if(object.type == engine::TR1ItemId::PassportOpening)
  {
    object.type = engine::TR1ItemId::PassportClosed;
    object.meshAnimFrame = 0_frame;
  }

  return std::move(m_next);
}

void FinishItemAnimationMenuState::handleObject(engine::Engine& /*engine*/, MenuDisplay& display, MenuObject& object)
{
  if(&object != &display.getCurrentRing().getSelectedObject())
    zeroRotation(object, 256_au);
  else
    rotateForSelection(object);
}

std::unique_ptr<MenuState>
  DeselectingMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/, engine::Engine& /*engine*/, MenuDisplay& display)
{
  display.updateRingTitle();
  return create<IdleRingMenuState>(false);
}

void DeselectingMenuState::handleObject(engine::Engine& /*engine*/, MenuDisplay& display, MenuObject& object)
{
  if(&object != &display.getCurrentRing().getSelectedObject())
    zeroRotation(object, 256_au);
  else
    rotateForSelection(object);
}

DeselectingMenuState::DeselectingMenuState(const std::shared_ptr<MenuRingTransform>& ringTransform,
                                           engine::Engine& engine)
    : MenuState{ringTransform}
{
  engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuOptionEscape, nullptr);
}

std::unique_ptr<MenuState>
  IdleRingMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/, engine::Engine& engine, MenuDisplay& display)
{
  display.updateRingTitle();

  if(engine.getPresenter().getInputHandler().getInputState().xMovement.justChangedTo(hid::AxisMovement::Right)
     && display.getCurrentRing().list.size() > 1)
  {
    engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuMove, nullptr);
    return create<RotateLeftRightMenuState>(true, display.getCurrentRing(), std::move(display.m_currentState));
  }

  if(engine.getPresenter().getInputHandler().getInputState().xMovement.justChangedTo(hid::AxisMovement::Left)
     && display.getCurrentRing().list.size() > 1)
  {
    engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuMove, nullptr);
    return create<RotateLeftRightMenuState>(false, display.getCurrentRing(), std::move(display.m_currentState));
  }

  if(engine.getPresenter().getInputHandler().getInputState().menu.justChangedTo(true) && display.allowMenuClose)
  {
    engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuOptionEscape, nullptr);
    display.inventoryChosen.reset();
    return create<DeflateRingMenuState>(create<DoneMenuState>());
  }

  if(m_autoSelect || engine.getPresenter().getInputHandler().getInputState().action.justChangedTo(true))
  {
    display.passOpen = true;

    auto& currentObject = display.getCurrentRing().getSelectedObject();

    switch(currentObject.type)
    {
    case engine::TR1ItemId::Compass:
      engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuOptionSelect2, nullptr);
      break;
    case engine::TR1ItemId::LarasHomePolaroid:
      engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuHome, nullptr);
      break;
    case engine::TR1ItemId::DirectionKeys:
      engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::LowTone, nullptr);
      break;
    case engine::TR1ItemId::Pistols:
    case engine::TR1ItemId::Shotgun:
    case engine::TR1ItemId::Magnums:
    case engine::TR1ItemId::Uzis:
      engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuOptionSelect1, nullptr);
      break;
    default:
      engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuOptionPopup, nullptr);
      break;
    }

    currentObject.goalFrame = currentObject.openFrame;
    currentObject.animDirection = 1_frame;
    return create<ApplyItemTransformMenuState>();
  }

  if(engine.getPresenter().getInputHandler().getInputState().zMovement.justChangedTo(hid::AxisMovement::Forward)
     && display.currentRingIndex > 0)
  {
    return create<DeflateRingMenuState>(create<SwitchRingMenuState>(display.currentRingIndex - 1, false));
  }
  else if(engine.getPresenter().getInputHandler().getInputState().zMovement.justChangedTo(hid::AxisMovement::Backward)
          && display.currentRingIndex + 1 < display.rings.size())
  {
    return create<DeflateRingMenuState>(create<SwitchRingMenuState>(display.currentRingIndex + 1, false));
  }

  return nullptr;
}

void IdleRingMenuState::handleObject(engine::Engine& engine, MenuDisplay& display, MenuObject& object)
{
  if(&object == &display.getCurrentRing().getSelectedObject())
  {
    display.updateMenuObjectDescription(engine, object);
    idleRotation(engine, object);
  }
  else
  {
    zeroRotation(object, 256_au);
  }
}

std::unique_ptr<MenuState>
  SwitchRingMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/, engine::Engine& /*engine*/, MenuDisplay& display)
{
  if(m_duration != Duration)
  {
    m_duration += 1_frame;
    m_ringTransform->radius -= m_radiusSpeed;
    m_ringTransform->ringRotation -= 180_deg / Duration * 1_frame;
    m_ringTransform->cameraRotX = exactScale(m_targetCameraRotX, m_duration, Duration);
    return nullptr;
  }

  display.currentRingIndex = m_next;
  m_ringTransform->cameraRotX = m_targetCameraRotX;

  return create<InflateRingMenuState>();
}

void SwitchRingMenuState::handleObject(engine::Engine& engine, MenuDisplay& display, MenuObject& object)
{
  if(&object == &display.getCurrentRing().getSelectedObject())
  {
    display.updateMenuObjectDescription(engine, object);
    idleRotation(engine, object);
  }
  else
  {
    zeroRotation(object, 256_au);
  }
}

SwitchRingMenuState::SwitchRingMenuState(const std::shared_ptr<MenuRingTransform>& ringTransform,
                                         size_t next,
                                         bool down)
    : MenuState{ringTransform}
    , m_next{next}
    , m_down{down}
{
}

std::unique_ptr<MenuState>
  SelectedMenuState::onFrame(gl::Image<gl::SRGBA8>& img, engine::Engine& engine, MenuDisplay& display)
{
  auto& currentObject = display.getCurrentRing().getSelectedObject();
  if(currentObject.type == engine::TR1ItemId::PassportClosed)
    return create<PassportMenuState>(display.mode);

  if(currentObject.selectedRotationY == currentObject.rotationY && currentObject.animate())
    return nullptr;

  const bool autoSelect = display.doOptions(img, engine, currentObject);
  if(engine.getPresenter().getInputHandler().getInputState().menu.justChangedTo(true))
  {
    if(display.rings.size() > 1)
    {
      return create<FinishItemAnimationMenuState>(
        create<ResetItemTransformMenuState>(create<DeselectingMenuState>(engine)));
    }
    else
    {
      return create<FinishItemAnimationMenuState>(
        create<ResetItemTransformMenuState>(create<DeflateRingMenuState>(create<DoneMenuState>())));
    }
  }
  else if(autoSelect || engine.getPresenter().getInputHandler().getInputState().action.justChangedTo(true))
  {
    display.inventoryChosen = currentObject.type;
    if(display.mode == InventoryMode::TitleMode
       && (currentObject.type == engine::TR1ItemId::Sunglasses
           || currentObject.type == engine::TR1ItemId::CassettePlayer
           || currentObject.type == engine::TR1ItemId::DirectionKeys
           || currentObject.type == engine::TR1ItemId::Flashlight))
    {
      return create<FinishItemAnimationMenuState>(
        create<ResetItemTransformMenuState>(create<DeselectingMenuState>(engine)));
    }
    else
    {
      return create<FinishItemAnimationMenuState>(
        create<ResetItemTransformMenuState>(create<DeflateRingMenuState>(create<DoneMenuState>())));
    }
  }

  return nullptr;
}

void SelectedMenuState::handleObject(engine::Engine& /*engine*/, MenuDisplay& display, MenuObject& object)
{
  if(&object != &display.getCurrentRing().getSelectedObject())
    zeroRotation(object, 256_au);
  else
    rotateForSelection(object);
}

std::unique_ptr<MenuState>
  InflateRingMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/, engine::Engine& /*engine*/, MenuDisplay& display)
{
  if(m_duration == 0_frame)
  {
    bool doAutoSelect = false;
    if(display.rings.size() == 1 && !display.passOpen)
    {
      doAutoSelect = true;
    }

    return create<IdleRingMenuState>(doAutoSelect);
  }

  m_duration -= 1_frame;
  m_ringTransform->ringRotation
    = display.getCurrentRing().getCurrentObjectAngle() - exactScale(90_deg, m_duration, Duration);
  m_ringTransform->cameraRotX = exactScale(m_initialCameraRotX, m_duration, Duration);
  m_ringTransform->radius += m_radiusSpeed;
  m_ringTransform->cameraPos.Y += m_cameraSpeedY;
  return nullptr;
}

void InflateRingMenuState::handleObject(engine::Engine& engine, MenuDisplay& display, MenuObject& object)
{
  display.clearMenuObjectDescription();
  if(&object == &display.getCurrentRing().getSelectedObject())
  {
    idleRotation(engine, object);
  }
  else
  {
    zeroRotation(object, 256_au);
  }
}

InflateRingMenuState::InflateRingMenuState(const std::shared_ptr<MenuRingTransform>& ringTransform)
    : MenuState{ringTransform}
{
}

void ApplyItemTransformMenuState::handleObject(engine::Engine& engine, MenuDisplay& display, MenuObject& object)
{
  if(&object != &display.getCurrentRing().getSelectedObject())
  {
    zeroRotation(object, 256_au);
    return;
  }

  display.updateMenuObjectDescription(engine, object);
  object.baseRotationX = exactScale(object.selectedBaseRotationX, m_duration, Duration);
  object.rotationX = exactScale(object.selectedRotationX, m_duration, Duration);
  object.positionZ = exactScale(object.selectedPositionZ, m_duration, Duration);

  if(object.rotationY != object.selectedRotationY)
  {
    if(const auto dy = object.selectedRotationY - object.rotationY; dy > 0_deg && dy < 180_deg)
    {
      object.rotationY += 1024_au;
    }
    else
    {
      object.rotationY -= 1024_au;
    }
    object.rotationY -= object.rotationY % 1024_au;
  }
}

std::unique_ptr<MenuState>
  ApplyItemTransformMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/, engine::Engine& /*engine*/, MenuDisplay& display)
{
  display.updateRingTitle();

  if(m_duration != Duration)
  {
    m_duration += 1_frame;
    return nullptr;
  }

  return create<SelectedMenuState>();
}

void DeflateRingMenuState::handleObject(engine::Engine& engine, MenuDisplay& display, MenuObject& object)
{
  if(&object == &display.getCurrentRing().getSelectedObject())
  {
    display.updateMenuObjectDescription(engine, object);
    zeroRotation(object, 256_au);
  }
  else
  {
    idleRotation(engine, object);
  }
}

std::unique_ptr<MenuState>
  DeflateRingMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/, engine::Engine& /*engine*/, MenuDisplay& /*display*/)
{
  if(m_duration == 0_frame)
    return std::move(m_next);

  m_duration -= 1_frame;
  m_ringTransform->ringRotation -= 180_deg / Duration * 1_frame;
  m_ringTransform->radius = exactScale(m_initialRadius, m_duration, Duration);
  m_ringTransform->cameraPos.Y += m_cameraSpeedY;
  return nullptr;
}

DeflateRingMenuState::DeflateRingMenuState(const std::shared_ptr<MenuRingTransform>& ringTransform,
                                           std::unique_ptr<MenuState> next)
    : MenuState{ringTransform}
    , m_next{std::move(next)}
{
}

void DoneMenuState::handleObject(engine::Engine& engine, MenuDisplay& display, MenuObject& object)
{
  if(&object == &display.getCurrentRing().getSelectedObject())
  {
    display.updateMenuObjectDescription(engine, object);
    zeroRotation(object, 256_au);
  }
  else
  {
    idleRotation(engine, object);
  }
}

std::unique_ptr<MenuState>
  DoneMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/, engine::Engine& /*engine*/, MenuDisplay& display)
{
  display.isDone = true;
  return nullptr;
}

void RotateLeftRightMenuState::handleObject(engine::Engine& engine, MenuDisplay& display, MenuObject& object)
{
  if(&object == &display.getCurrentRing().getSelectedObject())
    display.updateMenuObjectDescription(engine, object);
  zeroRotation(object, 512_au);
}

RotateLeftRightMenuState::RotateLeftRightMenuState(const std::shared_ptr<MenuRingTransform>& ringTransform,
                                                   bool left,
                                                   const MenuRing& ring,
                                                   std::unique_ptr<MenuState>&& prev)
    : MenuState{ringTransform}
    , m_rotSpeed{(left ? -ring.getAnglePerItem() : ring.getAnglePerItem()) / Duration}
    , m_prev{std::move(prev)}
{
  m_targetObject = ring.currentObject + (left ? -1 : 1);
  if(m_targetObject == std::numeric_limits<size_t>::max())
  {
    m_targetObject = ring.list.size() - 1;
  }
  if(m_targetObject >= ring.list.size())
  {
    m_targetObject = 0;
  }
}

std::unique_ptr<MenuState>
  RotateLeftRightMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/, engine::Engine& /*engine*/, MenuDisplay& display)
{
  display.clearMenuObjectDescription();
  m_ringTransform->ringRotation += m_rotSpeed * 1_frame;
  m_duration -= 1_frame;
  if(m_duration != 0_frame)
    return nullptr;

  display.getCurrentRing().currentObject = m_targetObject;
  m_ringTransform->ringRotation = display.getCurrentRing().getCurrentObjectAngle();
  return std::move(m_prev);
}

void PassportMenuState::handleObject(engine::Engine& /*engine*/, MenuDisplay& display, MenuObject& object)
{
  if(&object != &display.getCurrentRing().getSelectedObject())
    zeroRotation(object, 256_au);
  else
    rotateForSelection(object);
}

std::unique_ptr<MenuState>
  PassportMenuState::onFrame(gl::Image<gl::SRGBA8>& img, engine::Engine& engine, MenuDisplay& display)
{
  auto& passport = display.getCurrentRing().getSelectedObject();
  passport.type = engine::TR1ItemId::PassportOpening;

  if(passport.selectedRotationY == passport.rotationY && passport.animate())
    return nullptr;

  const bool isInGame = display.mode != InventoryMode::TitleMode && display.mode != InventoryMode::DeathMode;
  const bool hasSavedGames = !getSavegames(engine).empty();

  display.objectTexts[0].reset();
  const auto localFrame = passport.goalFrame - passport.openFrame;
  auto page = localFrame / FramesPerPage;
  hid::AxisMovement forcePageTurn = hid::AxisMovement::Null;
  if(localFrame % FramesPerPage != 0_frame)
  {
    page = -1;
  }
  else if(m_forcePage.value_or(page) != page)
  {
    page = -1;
    if(page < *m_forcePage)
      forcePageTurn = hid::AxisMovement::Right;
    else
      forcePageTurn = hid::AxisMovement::Left;
  }

  switch(page)
  {
  case LoadGamePage:
    if(!hasSavedGames || display.mode == InventoryMode::SaveMode)
    {
      forcePageTurn = hid::AxisMovement::Right;
      break;
    }
    if(m_passportText == nullptr)
    {
      m_passportText = std::make_unique<ui::Label>(0, -16, "Load Game");
      m_passportText->alignX = ui::Label::Alignment::Center;
      m_passportText->alignY = ui::Label::Alignment::Bottom;
    }
    if(engine.getPresenter().getInputHandler().getInputState().action.justChangedTo(true)
       || display.mode == InventoryMode::LoadMode)
    {
      display.objectTexts[0].reset();
      display.objectTexts[2].reset();
      BOOST_LOG_TRIVIAL(error) << "load game dialog not implemented yet";
      return nullptr;
    }
    break;
  case SaveGamePage:
    if(!m_allowSave)
    {
      // can't save when dead, so just skip this page
      if(passport.animDirection == -1_frame)
        forcePageTurn = hid::AxisMovement::Left;
      else
        forcePageTurn = hid::AxisMovement::Right;
      break;
    }
    if(m_passportText == nullptr)
    {
      m_passportText = std::make_unique<ui::Label>(0, -16, m_allowSave && isInGame ? "Save Game" : "New Game");
      m_passportText->alignX = ui::Label::Alignment::Center;
      m_passportText->alignY = ui::Label::Alignment::Bottom;
    }
    if(engine.getPresenter().getInputHandler().getInputState().action.justChangedTo(true)
       || display.mode == InventoryMode::SaveMode)
    {
      if(m_allowSave && isInGame)
      {
        display.objectTexts[0].reset();
        display.objectTexts[2].reset();
        return create<SavegameListMenuState>(std::move(display.m_currentState));
      }
    }
    break;
  case ExitGamePage:
    if(m_passportText == nullptr)
    {
      m_passportText = std::make_unique<ui::Label>(0, -16, !isInGame ? "Exit Game" : "Exit to Title");
      m_passportText->alignX = ui::Label::Alignment::Center;
      m_passportText->alignY = ui::Label::Alignment::Bottom;
    }
    break;
  default: Expects(page == -1); break;
  }

  if(m_passportText != nullptr)
    m_passportText->draw(engine.getPresenter().getTrFont(), img, engine.getPalette());

  if(forcePageTurn == hid::AxisMovement::Left
     || engine.getPresenter().getInputHandler().getInputState().xMovement.justChangedTo(hid::AxisMovement::Left))
  {
    if(hasSavedGames)
    {
      passport.goalFrame -= FramesPerPage;
      passport.animDirection = -1_frame;
      if(passport.goalFrame < passport.openFrame)
      {
        passport.goalFrame = passport.openFrame;
      }
      else
      {
        engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuGamePageTurn, nullptr);
        m_passportText.reset();
      }
      return nullptr;
    }
    else if(m_allowSave)
    {
      passport.goalFrame -= FramesPerPage;
      passport.animDirection = -1_frame;
      if(const auto firstFrame = passport.openFrame + FramesPerPage; passport.goalFrame < firstFrame)
      {
        passport.goalFrame = firstFrame;
      }
      else
      {
        engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuGamePageTurn, nullptr);
        m_passportText.reset();
      }
      return nullptr;
    }
  }
  else if(forcePageTurn == hid::AxisMovement::Right
          || engine.getPresenter().getInputHandler().getInputState().xMovement.justChangedTo(hid::AxisMovement::Right))
  {
    passport.goalFrame += FramesPerPage;
    passport.animDirection = 1_frame;
    if(const auto lastFrame = passport.lastMeshAnimFrame - FramesPerPage - 1_frame; passport.goalFrame > lastFrame)
    {
      passport.goalFrame = lastFrame;
    }
    else
    {
      engine.getPresenter().getAudioEngine().playSoundEffect(engine::TR1SoundEffect::MenuGamePageTurn, nullptr);
      m_passportText.reset();
    }
    return nullptr;
  }
  else if(engine.getPresenter().getInputHandler().getInputState().menu.justChangedTo(true))
  {
    if(!m_allowExit)
      return nullptr;

    return close(display, page, passport);
  }
  else if(engine.getPresenter().getInputHandler().getInputState().action.justChangedTo(true))
  {
    return close(display, page, passport);
  }

  return nullptr;
}

PassportMenuState::PassportMenuState(const std::shared_ptr<MenuRingTransform>& ringTransform, InventoryMode mode)
    : MenuState{ringTransform}
    , m_allowExit{mode != InventoryMode::DeathMode && mode != InventoryMode::TitleMode}
    , m_allowSave{mode != InventoryMode::DeathMode && mode != InventoryMode::TitleMode}
    , m_forcePage{mode == InventoryMode::LoadMode   ? std::optional<int>{0}
                  : mode == InventoryMode::SaveMode ? std::optional<int>{1}
                                                    : std::nullopt}
{
}

std::unique_ptr<MenuState> PassportMenuState::close(MenuDisplay& /*display*/, int page, MenuObject& passport)
{
  m_passportText.reset();

  if(page == ExitGamePage)
  {
    passport.goalFrame = passport.lastMeshAnimFrame - 1_frame;
    passport.animDirection = 1_frame;
  }
  else
  {
    passport.goalFrame = 0_frame;
    passport.animDirection = -1_frame;
  }

  return create<FinishItemAnimationMenuState>(create<SetItemTypeMenuState>(
    engine::TR1ItemId::PassportClosed, create<ResetItemTransformMenuState>(create<IdleRingMenuState>(false))));
}

std::unique_ptr<MenuState>
  SetItemTypeMenuState::onFrame(gl::Image<gl::SRGBA8>& /*img*/, engine::Engine& /*engine*/, MenuDisplay& /*display*/)
{
  return std::move(m_next);
}

void SetItemTypeMenuState::handleObject(engine::Engine& /*engine*/, MenuDisplay& display, MenuObject& object)
{
  if(&object == &display.getCurrentRing().getSelectedObject())
    object.type = m_type;
}

SavegameListMenuState::SavegameListMenuState(const std::shared_ptr<MenuRingTransform>& ringTransform,
                                             std::unique_ptr<MenuState> passport)
    : MenuState{ringTransform}
    , m_passport{std::move(passport)}
{
  for(size_t i = 0; i < totalSlots; ++i)
  {
    const auto line = i % perPage;
    auto lbl = std::make_unique<ui::Label>(0, yOffset + line * lineHeight, "- EMPTY SLOT " + std::to_string(i + 1));
    lbl->alignX = ui::Label::Alignment::Center;
    lbl->alignY = ui::Label::Alignment::Bottom;
    m_labels.emplace_back(std::move(lbl));
  }
}

void SavegameListMenuState::handleObject(engine::Engine& engine, MenuDisplay& /*display*/, MenuObject& /*object*/)
{
}
std::unique_ptr<MenuState>
  SavegameListMenuState::onFrame(gl::Image<gl::SRGBA8>& img, engine::Engine& engine, MenuDisplay& /*display*/)
{
  const auto page = m_selected / perPage;
  const auto first = page * perPage;
  const auto last = std::min(first + perPage, m_labels.size());
  Expects(first < last);
  for(size_t i = first; i < last; ++i)
  {
    const auto& lbl = m_labels.at(i);
    if(m_selected == i)
    {
      lbl->addBackground(pixelWidth - 12, 16, 0, 0);
      lbl->outline = true;
    }
    else
    {
      lbl->removeBackground();
      lbl->outline = false;
    }
    lbl->draw(engine.getPresenter().getTrFont(), img, engine.getPalette());
  }

  if(m_selected > 0
     && engine.getPresenter().getInputHandler().getInputState().zMovement.justChangedTo(hid::AxisMovement::Forward))
  {
    --m_selected;
  }
  else if(m_selected < totalSlots - 1
          && engine.getPresenter().getInputHandler().getInputState().zMovement.justChangedTo(
            hid::AxisMovement::Backward))
  {
    ++m_selected;
  }
  else if(engine.getPresenter().getInputHandler().getInputState().action.justChangedTo(true))
  {
    // TODO implement save game action
    BOOST_LOG_TRIVIAL(warning) << "Save game action not implemented yet";
    return std::move(m_passport);
  }
  else if(engine.getPresenter().getInputHandler().getInputState().menu.justChangedTo(true))
  {
    return std::move(m_passport);
  }

  return nullptr;
}
} // namespace menu