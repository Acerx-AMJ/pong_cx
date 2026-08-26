#include "game.hpp"
#include "loadingState.hpp"
#include <CX/Math/Random.hpp>
#include <CX/Button/ButtonStyle.hpp>
#include <CX/UIElement/UIElement.hpp>

// Constructors

Game::Game()
{
   cx::seed_random();

   window = std::make_shared<sf::RenderWindow>(sf::VideoMode(900, 500), "Pong CXlib"s, sf::Style::Fullscreen);//sf::Style::Titlebar | sf::Style::Close);
   window->setVerticalSyncEnabled(true);
   window->setFramerateLimit(60u);

   asset = std::make_shared<cx::AssetManager>("assets/"s);
   event = std::make_shared<cx::EventHandler>(*window);
   audio = std::make_shared<cx::AudioManager>(*asset);

   states.push_back(std::make_shared<LoadingState>(window, event, asset, audio));
}

Game::~Game()
{
   window->close();
}

// Update functions

void Game::createStyles()
{

   audio->save_sound("click"s, "click3"s, 1, .75f, 1.25f);
   audio->save_sound("hit"s, {"hit2"s, "hit3"s}, 1, .75f, 1.25f);
   audio->save_sound("scored"s, {"pickup"s, "powerUp"s, "success"s}, 1, .75f, 1.25f);
   audio->save_sound("win"s, "win"s, 1, .75f, 1.25f);
   audio->save_sound("start"s, "horn"s, 1, .75f, 1.25f);

   // Create styles
   cx::ButtonStyle::style1() = cx::ButtonStyle{cx::Vec2f(200.f, 50.f), asset->get_font("pixel"s), 16, cx::Color(200), cx::Color(255), cx::Color(0), cx::Color(0), 0.f, 1.5f, asset->get_texture("button8"s)};
   cx::Functions::style1() = {std::make_shared<std::function<void(cx::UIElement&)>>([this](cx::UIElement& e)
   {
      if (e.is_mouse_down() && !e.is_mouse_up())
      {
         e.set_color(cx::Color(125));
         e.set_scale((e.get_scale() * .75f).min(.75f));
      }

      if (e.is_mouse_up())
      {
         e.set_color(cx::Color(255));
         e.set_scale(1.1f);
         audio->play_saved_sound("hit"s);
      }

      if (e.started_hovering())
         audio->play_saved_sound("click"s);

      if (e.is_hovering() && !e.is_mouse_down())
      {
         e.set_color(cx::Color(255));
         e.set_scale((e.get_scale() * 1.1f).max(1.25f));
      }
      else if (!e.is_hovering())
      {
         e.set_color(cx::Color(200));
         e.set_scale((e.get_scale() * .9f).min(1.f));
      }
   })};
}

void Game::run()
{
   while (window->isOpen())
   {
      // Update states
      if (states.front()->shouldQuit())
      {
         if (!loaded)
         {
            loaded = true;
            createStyles();
         }

         states.front()->changeState(states);
         states.front().reset();
         states.pop_front();
      }

      if (states.size() == 0)
      {
         window->close();
         return;
      }

      // Update game
      audio->update();
      event->update();
      states.front()->update();

      // Render the game
      window->clear();
      states.front()->render();
      window->display();
   }
}
