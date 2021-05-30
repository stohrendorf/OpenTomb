#include "levelstats.h"

#include "core/i18n.h"
#include "engine/player.h"
#include "engine/presenter.h"
#include "label.h"
#include "ui/ui.h"

namespace ui
{
void LevelStats::draw(Ui& ui) const
{
  ui.drawBox({0, 0}, m_presenter->getViewport(), gl::SRGBA8{0, 0, 0, 224});

  {
    Label lbl{{0, 0}, m_title};
    lbl.anchorX = Label::Anchor::Center;
    lbl.pos.y = m_presenter->getViewport().y / 2 - 50;
    lbl.draw(ui, m_presenter->getTrFont(), m_presenter->getViewport());
  }

  const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(m_player->timeSpent);
  static constexpr auto Minute = std::chrono::seconds{60};
  static constexpr auto Hour = 60 * Minute;
  if(seconds >= std::chrono::hours{1})
  {
    Label lbl{{0, 0},
              /* translators: TR charmap encoding */ _("TIME TAKEN %1%:%2$02d:%3$02d",
                                                       seconds.count() / Hour.count(),
                                                       (seconds.count() / Minute.count()) % Hour.count(),
                                                       seconds.count() % Minute.count())};
    lbl.anchorX = Label::Anchor::Center;
    lbl.pos.y = m_presenter->getViewport().y / 2 + 70;
    lbl.draw(ui, m_presenter->getTrFont(), m_presenter->getViewport());
  }
  else
  {
    Label lbl{{0, 0},
              /* translators: TR charmap encoding */ _(
                "TIME TAKEN %1$02d:%2$02d", seconds.count() / Minute.count(), seconds.count() % Minute.count())};
    lbl.anchorX = Label::Anchor::Center;
    lbl.pos.y = m_presenter->getViewport().y / 2 + 70;
    lbl.draw(ui, m_presenter->getTrFont(), m_presenter->getViewport());
  }

  {
    Label lbl{{0, 0},
              /* translators: TR charmap encoding */ _("SECRETS %1% of %2%", m_player->secrets, m_totalSecrets)};
    lbl.anchorX = Label::Anchor::Center;
    lbl.pos.y = m_presenter->getViewport().y / 2 + 40;
    lbl.draw(ui, m_presenter->getTrFont(), m_presenter->getViewport());
  }

  {
    Label lbl{{0, 0}, /* translators: TR charmap encoding */ _("PICKUPS %1%", m_player->pickups)};
    lbl.anchorX = Label::Anchor::Center;
    lbl.pos.y = m_presenter->getViewport().y / 2 + 10;
    lbl.draw(ui, m_presenter->getTrFont(), m_presenter->getViewport());
  }

  {
    Label lbl{{0, 0}, /* translators: TR charmap encoding */ _("KILLS %1%", m_player->kills)};
    lbl.anchorX = Label::Anchor::Center;
    lbl.pos.y = m_presenter->getViewport().y / 2 - 20;
    lbl.draw(ui, m_presenter->getTrFont(), m_presenter->getViewport());
  }
}
} // namespace ui
