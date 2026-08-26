#include "loadingState.hpp"
#include "menuState.hpp"
#include <CX/Math/Random.hpp>
#include <CX/Config.hpp>

// Constants

const std::vector<std::string> LoadingState::infoTexts
{
   "Made using the CX library!"s,
   "Using CX "s CX_VERSION_STRING "!"s,
   "Made by Chalcinxx."s,
   "Pong."s,
   "Pong. Or is it?"s,
   "Hard Bot just has more optimized code, he isn't cheating."s,
   "Maybe Hard Bot is cheating after all..."s,
   "Maybe the real victory were the friends we made along the way."s,
   "Who doesn't love pong?"s,
   "Try to hit the ball with the corner of the paddle to make it harder for your opponent to hit it back!"s,
   "Please avoid bouncing off of walls"s,
   "Your velocity slightly applies to the ball!"s,
   "I bet you can't beat the Nightmare Bot."s,
   "Nightmare and Hard Bot throws a tantrum if their speeds are lowered, sorry!"s
};

// Constructors

LoadingState::LoadingState(window_t& window,
                           eventHandler& event,
                           assetManager& asset,
                           audioManager& audio)
   : State(window, event, asset, audio)
{
   // Create loading bar
   loadingScreenBar.create(cx::Vec2f(600.f, 35.f), event->get_center(), 0.f, cx::Color(255), cx::Color(0));
   loadingScreenBar.set_bg_outline_color(cx::Color(255));
   loadingScreenBar.set_bg_outline_thickness(2.5f);

   // Create loading text
   loadingText.create("Loading Textures..."s, *asset->load_font("fonts/pixel.ttf"s), loadingScreenBar.get_center() - cx::Vec2f(0.f, 75.f), 48u);

   // Create random string text
   infoText.create(infoTexts.at(cx::randiu(size_t(0), infoTexts.size() - 1)), *asset->load_font("fonts/pixel.ttf"s), cx::Vec2f(), 18u);
   infoText.wrap(window->getSize().x);
   infoText.set_center(loadingScreenBar.get_center() + cx::Vec2f(0.f, 75.f));

   // Create screen tint
   screenTint.create(event->get_size(), event->get_center(), cx::Color(0, 0));

   // Make sure all of the directories exist
   if (!asset->dir_exists("images/"s))
      throw std::runtime_error("Directory 'assets/images' does not exist, but is vital."s);
   if (!asset->dir_exists("sounds/"s))
      throw std::runtime_error("Directory 'assets/sounds' does not exist, but is vital."s);
   if (!asset->dir_exists("music/"s))
      throw std::runtime_error("Directory 'assets/music' does not exist, but is vital."s);
   if (!asset->dir_exists("fonts/"s))
      throw std::runtime_error("Directory 'assets/fonts' does not exist, but is vital."s);

   // Load textures first
   asset->load_texture_dir_async([this](bool success)
   {
      if (!success)
         throw std::runtime_error("Loading textures wasn't successful."s);

      loadingScreenBar.increment_progress_percent(25);
   }, "images/"s);
}

// Update functions

void LoadingState::update()
{
   const char progress {loadingScreenBar.get_progress_percent()};

   if (progress == 25)
   {
      // Load sounds after loading textures
      loadingScreenBar.increment_progress_percent(1);
      loadingText.set_string("Loading sounds..."s);

      asset->load_sound_dir_async([this](bool success)
      {
         if (!success)
            throw std::runtime_error("Loading sounds wasn't successful.");

         loadingScreenBar.increment_progress_percent(25);
      }, "sounds/"s);
   }
   else if (progress == 51)
   {
      // Load music after loading sounds
      loadingScreenBar.increment_progress_percent(1);
      loadingText.set_string("Loading music..."s);

      asset->load_song_dir_async([this](bool success)
      {
         if (!success)
            throw std::runtime_error("Loading music wasn't successful.");

         loadingScreenBar.increment_progress_percent(25);
      }, "music/"s);
   }
   else if (progress == 77)
   {
      // Load fonts after loading music
      loadingScreenBar.increment_progress_percent(1);
      loadingText.set_string("Loading fonts..."s);

      asset->load_font_dir_async([this](bool success)
      {
         if (!success)
            throw std::runtime_error("Loading fonts wasn't successful.");

         loadingScreenBar.set_progress_percent(100);
         loadingText.set_string("Loading done!"s);
         audio->play_sound("success"s, 1, .75f, 1.25f);
      }, "fonts/"s);
   }
   else if (progress == 100)
   {
      // Wait a bit, fade out and quit the state
      doneTimer += event->get_dt();

      if (doneTimer >= loadingDoneLength)
      {
         fadeTimer += event->get_dt();
         screenTint.set_color(cx::Color(0, 0).blend(cx::Color(0), fadeTimer / loadingFadeLength));
      }

      if (fadeTimer >= loadingFadeLength)
      {
         quit = true;
         screenTint.set_color(cx::Color(0));
      }
   }
}

void LoadingState::changeState(std::deque<state_t>& states)
{
   states.push_back(std::make_unique<MenuState>(window, event, asset, audio));
}

// Render functions

void LoadingState::render()
{
   loadingScreenBar.render(*window);
   loadingText.render(*window);
   infoText.render(*window);
   screenTint.render(*window);
}
