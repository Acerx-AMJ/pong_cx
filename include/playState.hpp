#ifndef PONG_CXLIB_PLAYSTATE_HPP
#define PONG_CXLIB_PLAYSTATE_HPP

#include "state.hpp"
#include <CX/Button/Button.hpp>
#include <CX/Camera.hpp>
#include <CX/NavigationManager.hpp>
#include <CX/Rect/Rect.hpp>
#include <CX/Text/Text.hpp>
#include <CX/Timer/Timer.hpp>

// Last hit enum for managing ball movement
enum class LastHit : char
{
   right = -1,
   nobody,
   left,
};

// Handle balls
struct Ball
{
   cx::Rect ball;
   cx::Timer timer;
   cx::Vec2f dir;
   LastHit last;
};

// Game state for handling the PvP / PvAI mode
class PlayState : public State
{
public:
   // State enum for managing different states
   enum class St_t : char
   {
      playing,
      paused,
      winscreen,
      mainmenu,
      restart,
   };

   // Constructors

   PlayState(window_t& window,
             eventHandler& event,
             assetManager& asset,
             audioManager& audio,
             char botDifficulty,
             unsigned maxPoints = 10u,
             float acceleration = 2.5f,
             float deceleration = 6.f,
             float wallBounce = -.75f,
             bool leftDuo = false,
             bool rightDuo = false,
             unsigned ballCount = 1u,
             bool party = false,
             const cx::Vec2u& partyScore2 = cx::Vec2u());
   ~PlayState() = default;

   // Update functions

   void update() override;

   void updateFadeScreens();
   void updatePausing();
   void updateLeftSideOnePlayer();
   void updateLeftSideTwoPlayers();
   Ball* getOptimalBall();
   void updateRightSideOneBot();
   void updateRightSideOnePlayer();
   void updateRightSideTwoBots();
   void updateRightSideTwoPlayers();
   void updateOneBall(size_t index = 0u);
   void updateMultipleBalls();
   void updateEffects();
   void updateWinScreen();

   void changeState(std::deque<state_t>& states) override;

   // Render functions

   void render() override;

private:
   // Constants

   static constexpr float fadeInTime   = .4f;
   static constexpr float fadeOutTime  = .4f;
   static constexpr float winTimeout   = 3.f;
   static constexpr float ballTimeout  = 1.5f;
   static constexpr float ballSpeed    = 200.f;
   static constexpr float playerSpeed  = 400.f;

   // Variables

   char botDifficulty;
   unsigned maxPoints;
   float acceleration;
   float deceleration;
   float wallBounce;
   bool leftDuo;
   bool rightDuo;
   unsigned ballCount;
   bool party;
   cx::Vec2u partyScore;

   std::vector<Ball> balls;
   cx::Rect player1;
   cx::Rect player2;
   cx::Rect player3;
   cx::Rect player4;

   cx::Text leftScore;
   cx::Text rightScore;
   cx::Rect plusOne;

   cx::Rect pauseButton;
   cx::Text pauseText;
   cx::Button pauseContinue;
   cx::Button pauseRestart;
   cx::Button pauseMainmenu;
   cx::Rect pointer;
   cx::NavigationManager pauseButtons;

   cx::Camera camera;
   cx::Rect screenTint;
   std::array<cx::Rect, 8> line;

   float dt               = 0.f;
   float frame            = 0.f;
   float plusOneRot       = 0.f;
   float fadeOutTimer     = 0.f;
   float fadeInTimer      = 0.f;
   float speedMultiplier  = 1.f;
   bool finishedFading    = false;
   bool startFading       = false;
   St_t state             = St_t::playing;
   cx::Vec2u score;

   float player1Acc       = 0.f;
   float player2Acc       = 0.f;
   float player3Acc       = 0.f;
   float player4Acc       = 0.f;
};

#endif // PONG_CXLIB_PLAYSTATE_HPP
