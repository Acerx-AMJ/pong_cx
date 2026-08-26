#include "menuState.hpp"
#include "playState.hpp"
#include <CX/Grid.hpp>
#include <CX/Math/Trigonometry.hpp>

// Constructors

MenuState::MenuState(window_t& window,
                     eventHandler& event,
                     assetManager& asset,
                     audioManager& audio)
   : State(window, event, asset, audio)
{
   // Create grid
   cx::Grid grid (event->get_size(), 8, 8, event->get_center());
   screenTint.create(event->get_size(), event->get_center(), cx::Color(0));

   // Create the pointer
   pointer.set_size(50.f);
   pointer.set_texture(asset->get_texture("ptr_right8"s).get());

   // Create the main menu
   title.create("PONG"s, *asset->get_font("pixel"s), cx::Vec2f(), 110U, 5.f);
   grid.place(title, 4, 2);

   playHuman.create(cx::ButtonStyle::style1(), "Play PvP"s);
   playHuman.on_update(cx::Functions::style1());
   playBot.create(cx::ButtonStyle::style1(), "Play VS Bot"s);
   playBot.on_update(cx::Functions::style1());
   options.create(cx::ButtonStyle::style1(), "Options"s);
   options.on_update(cx::Functions::style1());
   quitBtn.create(cx::ButtonStyle::style1(), "Quit"s);
   quitBtn.on_update(cx::Functions::style1());
   grid.place(playHuman, 4, 4);
   grid.place(playBot, 4, 5);
   grid.place(options, 4, 6);
   grid.place(quitBtn, 4, 7);

   buttons.add_elements(playHuman, playBot, options, quitBtn);

   // Create the bot menu
   botText.create("Select Bot Difficulty:"s, *asset->get_font("pixel"s), cx::Vec2f(), 32u, 5.f);
   grid.place(botText, 4, 2);

   botBack.create(cx::Vec2f(50.f), cx::Vec2f(), cx::Color(200));
   botBack.on_update(cx::Functions::style1());
   botBack.set_texture(asset->get_texture("back8"s).get());
   grid.place(botBack, 0, 0, cx::Grid::Alignment::center);

   botEasy.create(cx::ButtonStyle::style1(), "Easy"s);
   botEasy.on_update(cx::Functions::style1());
   botMedium.create(cx::ButtonStyle::style1(), "Medium"s);
   botMedium.on_update(cx::Functions::style1());
   botHard.create(cx::ButtonStyle::style1(), "Hard"s);
   botHard.on_update(cx::Functions::style1());
   botNightmare.create(cx::ButtonStyle::style1(), "Nightmare"s);
   botNightmare.on_update(cx::Functions::style1());
   grid.place(botEasy, 4, 4);
   grid.place(botMedium, 4, 5);
   grid.place(botHard, 4, 6);
   grid.place(botNightmare, 4, 7);

   botButtons.add_elements(botEasy, botMedium, botHard, botNightmare, botBack);
}

void MenuState::update()
{
   dt = event->get_dt();

   // Handle fade screen
   if (!finishedFading)
   {
      fadeInTimer += dt;
      screenTint.set_color(cx::Color(0).blend(cx::Color(0, 0), fadeInTimer / fadeInTime));

      if (fadeInTimer >= fadeInTime)
      {
         screenTint.set_color(cx::Color(0, 0));
         finishedFading = true;
      }
   }

   if (startFading)
   {
      fadeOutTimer += dt;
      screenTint.set_color(cx::Color(0, 0).blend(cx::Color(0), fadeOutTimer / fadeOutTime));

      if (fadeOutTimer >= fadeOutTime)
      {
         screenTint.set_color(cx::Color(0));
         quit = true;
         return;
      }
   }

   // Update main menu
   if (state == St_t::idle)
   {
      buttons.update(*event, true);

      if (buttons.any_selected())
      {
         pointer.set_center(buttons.get_selected_element().get_center() - cx::Vec2f(175.f, 0.f));
         pointer.move(0.f, cx::sin(++frame * .25f) * 937.f * dt);
      }

      if (playHuman.is_mouse_up())
      {
         startFading = true;
         state = St_t::playingOfflinePVP;
         return;
      }

      if (playBot.is_mouse_up())
      {
         state = St_t::playingAgainstBot;
         botButtons.reset_selected_index();
      }

      if (quitBtn.is_mouse_up())
      {
         quit = true;
         state = St_t::quitting;
         return;
      }
   }

   // Update bot menu
   if (state == St_t::playingAgainstBot)
   {
      botButtons.update(*event, true);

      if (botButtons.any_selected())
      {
         pointer.set_center(botButtons.get_selected_element().get_center() - cx::Vec2f(175.f, 0.f));
         pointer.move(0.f, cx::sin(++frame * .25f) * 937.f * dt);
      }

      if (botBack.is_mouse_up())
         state = St_t::idle;
      else if (botButtons.is_selected_up())
         botDifficulty = botButtons.get_selected_index();
      else if (botEasy.is_mouse_up())
         botDifficulty = 1;
      else if (botMedium.is_mouse_up())
         botDifficulty = 2;
      else if (botHard.is_mouse_up())
         botDifficulty = 3;
      else if (botNightmare.is_mouse_up())
         botDifficulty = 4;

      if (botDifficulty != 0)
      {
         startFading = true;
         return;
      }
   }
}

void MenuState::changeState(std::deque<state_t>& states)
{
   if (state == St_t::playingOfflinePVP || state == St_t::playingAgainstBot)
      states.push_back(std::make_shared<PlayState>(window, event, asset, audio, botDifficulty, 10, 2.75f, 6.f, -.55f, false, false, 1));
}

void MenuState::render()
{
   if (state == St_t::idle || state == St_t::playingOfflinePVP)
   {
      title.render(*window);
      buttons.render(*window);

      if (buttons.any_selected())
         pointer.render(*window);
   }

   if (state == St_t::playingAgainstBot)
   {
      botText.render(*window);
      botButtons.render(*window);

      if (botButtons.any_selected())
         pointer.render(*window);
   }
   screenTint.render(*window);
}
