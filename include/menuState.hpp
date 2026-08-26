#ifndef PONG_CXLIB_MENU_STATE_HPP
#define PONG_CXLIB_MENU_STATE_HPP

#include "state.hpp"
#include <CX/NavigationManager.hpp>
#include <CX/Rect/Rect.hpp>
#include <CX/Button/Button.hpp>
#include <CX/Text/Text.hpp>

class MenuState : public State
{
public:
   // Enum for handling main menu states
   enum class St_t : char
   {
      playingOfflinePVP,
      playingAgainstBot,
      idle,
      quitting,
   };

   // Constructors

   MenuState(window_t& window,
             eventHandler& event,
             assetManager& asset,
             audioManager& audio);
   ~MenuState() = default;

   // Update functions

   void update() override;
   void changeState(std::deque<state_t>& states) override;

   // Render functions

   void render() override;

private:
   // Constants

   static constexpr float fadeInTime  = .8f;
   static constexpr float fadeOutTime = .4f;

   // Variables

   cx::Rect screenTint;
   cx::Text title;

   cx::Button playHuman;
   cx::Button playBot;
   cx::Button options;
   cx::Button quitBtn;
   cx::Rect pointer;
   cx::NavigationManager buttons;

   cx::Text botText;
   cx::Rect botBack;
   cx::Button botEasy;
   cx::Button botMedium;
   cx::Button botHard;
   cx::Button botNightmare;
   cx::NavigationManager botButtons;

   float dt               = 0.f;
   float frame            = 0.f;
   float fadeInTimer      = 0.f;
   float fadeOutTimer     = 0.f;
   bool finishedFading    = false;
   bool startFading       = false;
   St_t state             = St_t::idle;
   char botDifficulty     = 0;
};

#endif // PONG_CXLIB_MENU_STATE_HPP
