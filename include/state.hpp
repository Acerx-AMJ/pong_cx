#ifndef PONG_CXLIB_STATE_HPP
#define PONG_CXLIB_STATE_HPP

#include <CX/AudioManager.hpp>
#include <CX/EventHandler/EventHandler.hpp>
#include <deque>
#include <memory>

class State;

using state_t      = std::shared_ptr<State>;
using window_t     = std::shared_ptr<sf::RenderWindow>;
using eventHandler = std::shared_ptr<cx::EventHandler>;
using assetManager = std::shared_ptr<cx::AssetManager>;
using audioManager = std::shared_ptr<cx::AudioManager>;

using namespace std::string_literals;
using namespace std::string_view_literals;

// State class for creating states used by game
class State
{
public:
   // Constructors

   State(window_t& window,
         eventHandler& event,
         assetManager& asset,
         audioManager& audio);
   virtual ~State() = default;

   // Update functions

   virtual void update() = 0;
   virtual void changeState(std::deque<state_t>& states) = 0;

   bool shouldQuit() const;

   // Render functions

   virtual void render() = 0;

protected:
   std::shared_ptr<sf::RenderWindow>& window;
   std::shared_ptr<cx::EventHandler>& event;
   std::shared_ptr<cx::AssetManager>& asset;
   std::shared_ptr<cx::AudioManager>& audio;
   bool quit = false;
};

#endif // PONG_CXLIB_STATE_HPP
