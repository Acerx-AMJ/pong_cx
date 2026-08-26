#include "state.hpp"

// Constructors

State::State(window_t& window,
             eventHandler& event,
             assetManager& asset,
             audioManager& audio)
   : window(window), event(event), asset(asset), audio(audio) {}

// Update functions

bool State::shouldQuit() const
{
   return quit;
}
