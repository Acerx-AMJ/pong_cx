#ifndef PONG_CXLIB_LOADING_STATE_HPP
#define PONG_CXLIB_LOADING_STATE_HPP

#include "state.hpp"
#include <CX/Bar/Bar.hpp>
#include <CX/Rect/Rect.hpp>
#include <CX/Text/Text.hpp>

class LoadingState : public State
{
public:
   // Constructors

   LoadingState(window_t& window,
                eventHandler& event,
                assetManager& asset,
                audioManager& audio);
   ~LoadingState() = default;

   // Update functions

   void update() override;
   void changeState(std::deque<state_t>& states) override;

   // Render functions

   void render() override;

private:
   // Constants

   static constexpr float loadingDoneLength = 1.f;
   static constexpr float loadingFadeLength = .8f;
   static const std::vector<std::string> infoTexts;

   // Variables

   cx::Rect screenTint;
   cx::Bar loadingScreenBar;
   cx::Text loadingText;
   cx::Text infoText;

   float doneTimer = 0.f;
   float fadeTimer = 0.f;
};

#endif // PONG_CXLIB_LOADING_STATE_HPP
