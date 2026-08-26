#ifndef PONG_CXLIB_GAME_HPP
#define PONG_CXLIB_GAME_HPP

#include "state.hpp"

// Game class for managing different states
class Game
{
public:
   // Constructors

   Game();
   ~Game();

   // Update functions

   void createStyles();
   void run();

private:
   window_t window;
   eventHandler event;
   assetManager asset;
   audioManager audio;
   std::deque<state_t> states;
   bool loaded = false;
};

#endif // PONG_CXLIB_GAME_HPP
