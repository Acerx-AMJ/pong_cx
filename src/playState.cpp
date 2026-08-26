#include "playState.hpp"
#include "menuState.hpp"
#include <CX/Grid.hpp>
#include <CX/Math/Math.hpp>
#include <CX/Math/Trigonometry.hpp>
#include <CX/Ray/Ray.hpp>

// Constructors

PlayState::PlayState(window_t& window,
                     eventHandler& event,
                     assetManager& asset,
                     audioManager& audio,
                     char botDifficulty,
                     unsigned maxPoints,
                     float acceleration,
                     float deceleration,
                     float wallBounce,
                     bool leftDuo,
                     bool rightDuo,
                     unsigned ballCount,
                     bool party,
                     const cx::Vec2u& partyScore2)
   : State(window, event, asset, audio), botDifficulty(botDifficulty),
   maxPoints(maxPoints), acceleration(acceleration), deceleration(deceleration),
   wallBounce(wallBounce), leftDuo(leftDuo), rightDuo(rightDuo), ballCount(ballCount),
   party(party), partyScore(partyScore2)
{
   cx::Grid grid (event->get_size(), 8, 8, event->get_center());

   // Create balls
   for (unsigned i = 1; i <= ballCount; ++i)
   {
      cx::Rect ball (cx::Vec2f(25.f));
      ball.set_texture(asset->get_texture("ball"s).get());
      grid.place(ball, 4, 4);
      balls.push_back(Ball{ball});
   }

   // Create the left side players
   if (leftDuo)
   {
      player1.set_size(100.f, 50.f);
      grid.place(player1, 0, 2);
      player1.move(-25.f, 0.f);

      player2.set_size(100.f, 50.f);
      grid.place(player2, 0, 6);
      player2.move(-25.f, 0.f);
   }
   else
   {
      player1.set_size(100.f, 75.f);
      grid.place(player1, 0, 4);
      player1.move(-25.f, 0.f);
   }

   // Create the right side players
   if (rightDuo)
   {
      player3.set_size(100.f, 50.f);
      grid.place(player3, 8, 2);
      player3.move(25.f, 0.f);

      player4.set_size(100.f, 50.f);
      grid.place(player4, 8, 6);
      player4.move(25.f, 0.f);
   }
   else
   {
      player3.set_size(100.f, 75.f);
      grid.place(player3, 8, 4);
      player3.move(25.f, 0.f);
   }

   leftScore.create("0"s, *asset->get_font("pixel"s), cx::Vec2f(), 75u);
   leftScore.set_color(cx::Color(125));
   grid.place(leftScore, 2, 2);

   rightScore.create("0"s, *asset->get_font("pixel"s), cx::Vec2f(), 75u);
   rightScore.set_color(cx::Color(125));
   grid.place(rightScore, 6, 2);

   plusOne.create(cx::Vec2f(100.f), event->get_size() * 2u);
   plusOne.set_texture(asset->get_texture("plus_one"s).get());

   pauseButton.create(cx::Vec2f(50.f), cx::Vec2f(), cx::Color(200));
   pauseButton.on_update(cx::Functions::style1());
   pauseButton.set_texture(asset->get_texture("pause8"s).get());
   grid.place(pauseButton, 0, 0, cx::Grid::Alignment::center);

   pauseText.create("PAUSED"s, *asset->get_font("pixel"s), cx::Vec2f(), 110u, 5.f);
   grid.place(pauseText, 4, 2);

   pauseContinue.create(cx::ButtonStyle::style1(), "Continue"s);
   pauseContinue.on_update(cx::Functions::style1());
   grid.place(pauseContinue, 4, 4);

   pauseRestart.create(cx::ButtonStyle::style1(), "Restart"s);
   pauseRestart.on_update(cx::Functions::style1());
   grid.place(pauseRestart, 4, 5);

   pauseMainmenu.create(cx::ButtonStyle::style1(), "Main Menu"s);
   pauseMainmenu.on_update(cx::Functions::style1());
   grid.place(pauseMainmenu, 4, 6);

   pointer.set_size(50.f);
   pointer.set_texture(asset->get_texture("ptr_right8"s).get());
   pauseButtons.add_elements(pauseContinue, pauseRestart, pauseMainmenu);

   camera.set_size(event->get_size());
   camera.set_center(event->get_center());
   screenTint.create(event->get_size(), event->get_center(), cx::Color(0));

   for (char i {}; i < 8; ++i)
   {
      cx::Rect dot (cx::Vec2f(20.f), cx::Vec2f(), cx::Color(125));
      grid.place(dot, 4, i, cx::Grid::Alignment::left);
      line[i] = dot;
   }
}

// Update functions

void PlayState::update()
{
   dt = event->get_dt();
   updateFadeScreens();

   updatePausing();

   // Handle game logic
   if (state == St_t::playing)
   {
      if (leftDuo)
         updateLeftSideTwoPlayers();
      else
         updateLeftSideOnePlayer();

      for (auto& b : balls)
         b.ball.set_color(cx::Color(255));

      if (rightDuo)
      {
         if (botDifficulty == 0)
            updateRightSideTwoPlayers();
         else
            updateRightSideTwoBots();
      }
      else
      {
         if (botDifficulty == 0)
            updateRightSideOnePlayer();
         else
            updateRightSideOneBot();
      }

      if (ballCount > 1)
         updateMultipleBalls();
      else
         updateOneBall();

      // Handle transition to win screen
      if (score.x >= maxPoints || score.y >= maxPoints)
      {
         camera.shake(7.5f, 1.5f, 200.f);
         audio->play_saved_sound("win"s);
         state = St_t::winscreen;

         pauseText.set_char_size(60u);
         pauseText.set_string((score.x >= maxPoints ? "Player1 Won!"s : (botDifficulty == 0 ? "Player2 Won!"s : "Bot Won!"s)));
         pauseContinue.set_string("Play Again"s);
         pauseRestart.set_string("Main Menu"s);
         pauseButtons.remove_element(3u);
      }
   }

   updateEffects();
   updateWinScreen();
   camera.update(dt);
}

void PlayState::updateFadeScreens()
{
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
}

void PlayState::updatePausing()
{
      if (state == St_t::playing || state == St_t::paused)
   {
      pauseButton.update(event->get_mouse_state(cx::Mouse::left), true);

      if (pauseButton.is_mouse_up() || event->is_released(cx::Key::esc, cx::Controller::x))
      {
         audio->play_saved_sound("hit"s);
         pauseButtons.set_selected_index(1u);
         state = (state == St_t::playing ? St_t::paused : St_t::playing);

         for (auto& ball : balls)
            ball.timer.set_paused(state == St_t::paused);
      }
   }

   if (state == St_t::paused)
   {
      pauseButtons.update(*event, true);

      if (pauseContinue.is_mouse_up())
      {
         state = St_t::playing;

         for (auto& ball : balls)
            ball.timer.resume();
      }
      else if (pauseRestart.is_mouse_up())
      {
         startFading = true;
         state = St_t::restart;
         return;
      }
      else if (pauseMainmenu.is_mouse_up())
      {
         startFading = true;
         state = St_t::mainmenu;
         return;
      }
   }
}

void PlayState::updateLeftSideOnePlayer()
{
   float player1Dir = event->get_axis(cx::Key::s, cx::Key::w, cx::ControllerAxis::ls_y);

   if (player1Dir != 0.f)
   {
      player1Acc += player1Dir * acceleration * dt;
      player1Acc = cx::clamp(player1Acc, -acceleration, acceleration);
   }
   else if (player1Acc > 0.f)
      player1Acc = cx::max(player1Acc - deceleration * dt, 0.f);
   else
      player1Acc = cx::min(player1Acc + deceleration * dt, 0.f);

   const float player1Target = player1.get_center_y() + player1Acc * playerSpeed * dt;

   if (player1Target < player1.get_height() * .5f)
   {
      player1.set_center(player1.get_center_x(), player1.get_height() * .5f);
      player1Acc *= wallBounce;
   }
   else if (player1Target > event->get_height() - player1.get_height() * .5f)
   {
      player1.set_center(player1.get_center_x(), event->get_height() - player1.get_height() * .5f);
      player1Acc *= wallBounce;
   }
   else
      player1.set_center(player1.get_center_x(), player1Target);
}

void PlayState::updateLeftSideTwoPlayers()
{
   // Up
   float player1Dir = event->get_axis(cx::Key::s, cx::Key::w, cx::ControllerAxis::ls_y);

   if (player1Dir != 0.f)
   {
      player1Acc += player1Dir * acceleration * dt;
      player1Acc = cx::clamp(player1Acc, -acceleration, acceleration);
   }
   else if (player1Acc > 0.f)
      player1Acc = cx::max(player1Acc - deceleration * dt, 0.f);
   else
      player1Acc = cx::min(player1Acc + deceleration * dt, 0.f);

   const float player1Target = player1.get_center_y() + player1Acc * playerSpeed * dt;

   if (player1Target < player1.get_height() * .5f)
   {
      player1.set_center(player1.get_center_x(), player1.get_height() * .5f);
      player1Acc *= wallBounce;
   }
   else if (player1Target > event->get_center_y() - player1.get_height() * .5f)
   {
      player1.set_center(player1.get_center_x(), event->get_center_y() - player1.get_height() * .5f);
      player1Acc *= wallBounce;
   }
   else
      player1.set_center(player1.get_center_x(), player1Target);

   // Bottom
   float player2Dir = event->get_axis(cx::Key::k, cx::Key::i, cx::Controller::down, cx::Controller::up);

   if (player2Dir != 0.f)
   {
      player2Acc += player2Dir * acceleration * dt;
      player2Acc = cx::clamp(player2Acc, -acceleration, acceleration);
   }
   else if (player2Acc > 0.f)
      player2Acc = cx::max(player2Acc - deceleration * dt, 0.f);
   else
      player2Acc = cx::min(player2Acc + deceleration * dt, 0.f);

   const float player2Target = player2.get_center_y() + player2Acc * playerSpeed * dt;

   if (player2Target < player2.get_height() * .5f + event->get_center_y())
   {
      player2.set_center(player2.get_center_x(), player2.get_height() * .5f + event->get_center_y());
      player2Acc *= wallBounce;
   }
   else if (player2Target > event->get_height() - player2.get_height() * .5f)
   {
      player2.set_center(player2.get_center_x(), event->get_height() - player2.get_height() * .5f);
      player2Acc *= wallBounce;
   }
   else
      player2.set_center(player2.get_center_x(), player2Target);
}

Ball* PlayState::getOptimalBall()
{
   if (ballCount == 1)
      return &balls.at(0);

   Ball* ball = nullptr;
   float furthest = 0.f;

   for (auto& b : balls)
   {
      if (b.last == LastHit::right) continue;
      float distance = b.ball.get_center_x();

      if (distance > furthest)
      {
         ball = &b;
         furthest = distance;
      }
   }
   return ball;
}

void PlayState::updateRightSideOneBot()
{
   Ball* ball = getOptimalBall();
   float player3Dir = 0.f;

   if (botDifficulty == 1)
   { // Easy bot
      static float reactionTimer = 0.f;
      static float last = 0.f;
      reactionTimer -= dt;

      if (reactionTimer <= 0.f && ball)
      {
         reactionTimer = cx::randf(.01f, .07f);
         player3Dir = (ball->ball.get_center_y() > player3.get_center_y() ? 1.f : -1.f);
         last = player3Dir;
      }
      else player3Dir = last;
   }
   else if (botDifficulty == 2 && ball)
   { // Medium bot
      player3Dir = (ball->ball.get_center_y() > player3.get_center_y() ? 1.f : -1.f);

      if ((player3Acc > 0.f && player3Dir < 0.f) || (player3Acc < 0.f && player3Dir > 0.f))
         player3Dir = 0.f;
   }
   else if (botDifficulty == 3 && ball)
   { // Hard bot
      const float prediction = ball->ball.get_center_y() + ball->dir.normalize().y * dt * ballSpeed * speedMultiplier;
      player3Dir = (prediction > player3.get_center_y() ? 1.f : -1.f);

      const float distanceTop    = player3.get_top();
      const float distanceBottom = event->get_size().y - (player3.get_top() + player3.get_height());
      const float threshold      = player3.get_height() * 2.f;

      if (distanceTop < threshold && player3Dir < 0.f)
         player3Dir *= (distanceTop / threshold) * .15f;

      if (distanceBottom < threshold && player3Dir > 0.f)
         player3Dir *= (distanceBottom / threshold) * .15f;

      if ((player3Acc > 0.f && player3Dir < 0.f) || (player3Acc < 0.f && player3Dir > 0.f))
         player3Acc *= .6f - cx::min(speedMultiplier * .025f, .6f);
      else
         player3Acc *= 1.02f + speedMultiplier * .025f;
   }
   else
   { // Nightmare bot
      static const cx::Vec4f dangerZone (float(event->get_size().x), -500.f, 500.f, float(event->get_size().y + 1000.f));
      static const cx::Vec4f upZone (0.f, -50.f, float(event->get_size().x), 50.f);
      static const cx::Vec4f downZone (0.f, float(event->get_size().y), float(event->get_size().x), 50.f);
      static cx::Ray ray (cx::Vec2f(), 0.f, 1'000'000'000.f);
      static float nightmareTarget = 0.f;
      static float timer = 0.f;
      timer -= dt;

      if (ball && ((ball->last == LastHit::nobody && ball->dir.x > 0.f) || ball->last == LastHit::left) && timer <= 0.f)
      {
         timer = .1f;

         ray.set_origin(ball->ball.get_center());
         ray.set_direction(ball->dir);

         cx::Ray currentRay = ray;
         bool hitWall = false;

         while (!hitWall)
         {
            std::optional<cx::Ray> reflected = currentRay.reflect((currentRay.get_direction().y > 0.f ? downZone : upZone));

            if (reflected.has_value())
               currentRay = reflected.value();
            else
            {
               const std::optional<cx::Vec2f> reflectedTarget = currentRay.collision_point(dangerZone);
               hitWall = true;

               if (reflectedTarget.has_value())
                  nightmareTarget = reflectedTarget->y;
            }
         }
      }

      if (nightmareTarget != 0.f)
      {
         player3Dir = (nightmareTarget > player3.get_center_y() ? 1.f : -1.f);

         const float distanceTop    = player3.get_top();
         const float distanceBottom = event->get_size().y - (player3.get_top() + player3.get_height());
         const float threshold      = player3.get_height() * 2.f;

         if (distanceTop < threshold && player3Dir < 0.f)
            player3Dir *= (distanceTop / threshold) * .15f;

         if (distanceBottom < threshold && player3Dir > 0.f)
            player3Dir *= (distanceBottom / threshold) * .15f;

         if ((player3Acc > 0.f && player3Dir < 0.f) || (player3Acc < 0.f && player3Dir > 0.f))
            player3Acc *= .6f - cx::min(speedMultiplier * .025f, .6f);
         else
            player3Acc *= 1.02f + speedMultiplier * .08f;
      }
   }

   if (player3Dir != 0.f)
   {
      player3Acc += player3Dir * acceleration * dt;
      player3Acc = cx::clamp(player3Acc, -acceleration, acceleration);
   }
   else if (player3Acc > 0.f)
      player3Acc = cx::max(player3Acc - deceleration * dt, 0.f);
   else
      player3Acc = cx::min(player3Acc + deceleration * dt, 0.f);

   const float player3Target = player3.get_center_y() + player3Acc * playerSpeed * dt;

   if (player3Target < player3.get_height() * .5f)
   {
      player3.set_center(player3.get_center_x(), player3.get_height() * .5f);
      player3Acc *= wallBounce;
   }
   else if (player3Target > event->get_size().y - player3.get_height() * .5f)
   {
      player3.set_center(player3.get_center_x(), event->get_size().y - player3.get_height() * .5f);
      player3Acc *= wallBounce;
   }
   else
      player3.set_center(player3.get_center_x(), player3Target);
}

void PlayState::updateRightSideOnePlayer()
{
   float player3Dir = event->get_axis(cx::Key::down, cx::Key::up, cx::ControllerAxis::rs_y);

   if (player3Dir != 0.f)
   {
      player3Acc += player3Dir * acceleration * dt;
      player3Acc = cx::clamp(player3Acc, -acceleration, acceleration);
   }
   else if (player3Acc > 0.f)
      player3Acc = cx::max(player3Acc - deceleration * dt, 0.f);
   else
      player3Acc = cx::min(player3Acc + deceleration * dt, 0.f);

   const float player3Target = player3.get_center_y() + player3Acc * playerSpeed * dt;

   if (player3Target < player3.get_height() * .5f)
   {
      player3.set_center(player3.get_center_x(), player3.get_height() * .5f);
      player3Acc *= wallBounce;
   }
   else if (player3Target > event->get_size().y - player3.get_height() * .5f)
   {
      player3.set_center(player3.get_center_x(), event->get_size().y - player3.get_height() * .5f);
      player3Acc *= wallBounce;
   }
   else
      player3.set_center(player3.get_center_x(), player3Target);
}

void PlayState::updateRightSideTwoBots()
{

}

void PlayState::updateRightSideTwoPlayers()
{
   // Up
   float player3Dir = event->get_axis(cx::Key::down, cx::Key::up, cx::ControllerAxis::rs_y);

   if (player3Dir != 0.f)
   {
      player3Acc += player3Dir * acceleration * dt;
      player3Acc = cx::clamp(player3Acc, -acceleration, acceleration);
   }
   else if (player3Acc > 0.f)
      player3Acc = cx::max(player3Acc - deceleration * dt, 0.f);
   else
      player3Acc = cx::min(player3Acc + deceleration * dt, 0.f);

   const float player3Target = player3.get_center_y() + player3Acc * playerSpeed * dt;

   if (player3Target < player3.get_height() * .5f)
   {
      player3.set_center(player3.get_center_x(), player3.get_height() * .5f);
      player3Acc *= wallBounce;
   }
   else if (player3Target > event->get_center_y() - player3.get_height() * .5f)
   {
      player3.set_center(player3.get_center_x(), event->get_center_y() - player3.get_height() * .5f);
      player3Acc *= wallBounce;
   }
   else
      player3.set_center(player3.get_center_x(), player3Target);

   // Bottom
   float player4Dir = event->get_axis(cx::Key::num3, cx::Key::num8, cx::Controller::a, cx::Controller::y);

   if (player4Dir != 0.f)
   {
      player4Acc += player4Dir * acceleration * dt;
      player4Acc = cx::clamp(player4Acc, -acceleration, acceleration);
   }
   else if (player4Acc > 0.f)
      player4Acc = cx::max(player4Acc - deceleration * dt, 0.f);
   else
      player4Acc = cx::min(player4Acc + deceleration * dt, 0.f);

   const float player4Target = player4.get_center_y() + player4Acc * playerSpeed * dt;

   if (player4Target < player4.get_height() * .5f + event->get_center_y())
   {
      player4.set_center(player4.get_center_x(), player4.get_height() * .5f + event->get_center_y());
      player4Acc *= wallBounce;
   }
   else if (player4Target > event->get_height() - player4.get_height() * .5f)
   {
      player4.set_center(player4.get_center_x(), event->get_height() - player4.get_height() * .5f);
      player4Acc *= wallBounce;
   }
   else
      player4.set_center(player4.get_center_x(), player4Target);
}

void PlayState::updateOneBall(size_t index)
{
   auto& ball = balls.at(index);

   if (ball.last == LastHit::nobody && ball.dir.empty() && ball.timer.get_elapsed() >= ballTimeout)
      ball.dir = cx::Vec2f((cx::randb() == 0 ? -1.f : 1.f), cx::randf(-1.f, 1.f));
   else if (ball.dir.empty() && ball.timer.get_elapsed() >= ballTimeout)
      ball.dir = cx::Vec2f(static_cast<float>(ball.last), cx::randf(-1.f, 1.f));

   const float moveValue = dt * ballSpeed * speedMultiplier;
   float stepSize = 1.f / (moveValue * .1f + 1.f);
   stepSize = cx::clamp(stepSize, .2f, 1.f);

   cx::Vec2f stepDir = ball.dir.normalize() * (moveValue * stepSize);

   for (float step = 0.f; step < 1.f; step += stepSize)
   {
      const cx::Vec2f ball1Target (ball.ball.get_center() + stepDir);

      if (ball1Target.y < ball.ball.get_height() * .5f)
      {
         audio->play_saved_sound("hit"s);
         ball.ball.set_center(ball.ball.get_center_x(), ball.ball.get_height() * .5f);
         ball.dir.y = -ball.dir.y;
      }

      if (ball1Target.y + ball.ball.get_height() * .5f > event->get_size().y)
      {
         audio->play_saved_sound("hit"s);
         ball.ball.set_center(ball.ball.get_center_x(), event->get_size().y - ball.ball.get_height() * .5f);
         ball.dir.y = -ball.dir.y;
      }

      const bool plr1Collision = player1.colliding(ball.ball);
      const bool plr2Collision = (!plr1Collision && leftDuo && player2.colliding(ball.ball));
      const bool plr3Collision = player3.colliding(ball.ball);
      const bool plr4Collision = (!plr3Collision && rightDuo && player4.colliding(ball.ball));

      if (ball.last != LastHit::left && (plr1Collision || plr2Collision))
      {
         const float hitPos = (ball.ball.get_center_y() - (plr1Collision ? player1.get_center_y() : player2.get_center_y())) / (player1.get_height() * .5f);
         ball.dir.x = cx::abs(ball.dir.x);
         ball.dir.y = hitPos + ((plr1Collision ? player1Acc : player2Acc) * ballSpeed * dt * 0.1f);
         ball.last = LastHit::left;
         audio->play_saved_sound("hit"s);

         if (stepSize > .2f)
            speedMultiplier *= 1.f + .1f / ballCount;
      }
      else if (ball.last != LastHit::right && (plr3Collision || plr4Collision))
      {
         const float hitPos = (ball.ball.get_center_y() - (plr3Collision ? player3.get_center_y() : player4.get_center_y())) / (player3.get_height() * .5f);
         ball.dir.x = -cx::abs(ball.dir.x);
         ball.dir.y = hitPos + ((plr3Collision ? player3Acc : player4Acc) * ballSpeed * dt * 0.1f);
         ball.last = LastHit::right;
         audio->play_saved_sound("hit"s);

         if (stepSize > .2f)
            speedMultiplier *= 1.f + .1f / ballCount;
      }
      
      if (ball1Target.x < ball.ball.get_width() * .5f)
      {
         camera.shake(10.f, 1.f, 200.f);
         audio->play_saved_sound("scored"s);
         score.y++;
         rightScore.set_string(std::to_string(score.y));

         plusOne.set_center(rightScore.get_center());
         plusOne.set_scale(cx::randf(1.75f, 2.25f));
         plusOneRot = cx::randf(0.f, 3.6f);

         ball.ball.set_center(event->get_center());
         ball.dir.clear();
         ball.timer.restart();
         speedMultiplier = 1.f;
      }
      else if (ball1Target.x + ball.ball.get_width() * .5f > event->get_size().x)
      {
         camera.shake(10.f, 1.f, 200.f);
         audio->play_saved_sound("scored"s);
         score.x++;
         leftScore.set_string(std::to_string(score.x));

         plusOne.set_center(leftScore.get_center());
         plusOne.set_scale(cx::randf(1.75f, 2.25f));
         plusOneRot = cx::randf(0.f, 3.6f);

         ball.ball.set_center(event->get_center());
         ball.dir.clear();
         ball.timer.restart();
         speedMultiplier = 1.f;
      }
      else
         ball.ball.move(ball.dir.normalize() * (moveValue * stepSize));
   }
}

void PlayState::updateMultipleBalls()
{
   for (size_t i = 0; i < ballCount; ++i)
      updateOneBall(i);
}

void PlayState::updateEffects()
{
   if (state != St_t::paused && plusOne.get_top() < event->get_size().y + plusOne.get_height() * 2.f)
   {
      plusOne.set_scale((plusOne.get_scale() * .99f).max(1.f));
      plusOne.move(cx::sin(++frame * plusOneRot * dt) * 100.f * dt, dt * 150.f);
      plusOne.rotate(plusOneRot);
   }

   if (pauseButtons.any_selected())
   {
      pointer.set_center(pauseButtons.get_selected_element().get_center() - cx::Vec2f(175.f, 0.f));
      pointer.move(0.f, cx::sin(++frame * .25f) * 15.f);
   }
}

void PlayState::updateWinScreen()
{
   if (state == St_t::winscreen)
   {
      pauseButtons.update(*event, true);

      if (pauseContinue.is_mouse_up())
      {
         startFading = true;
         state = St_t::restart;
         return;
      }
      else if (pauseRestart.is_mouse_up())
      {
         startFading = true;
         state = St_t::mainmenu;
         return;
      }
   }
}

void PlayState::changeState(std::deque<state_t>& states)
{
   if (state == St_t::restart)
      states.push_back(std::make_unique<PlayState>(window, event, asset, audio, botDifficulty));
   else if (state == St_t::mainmenu)
      states.push_back(std::make_unique<MenuState>(window, event, asset, audio));
}

// Render functions

void PlayState::render()
{
   camera.apply(*window);

   if (state == St_t::playing)
   {
      leftScore.render(*window);
      rightScore.render(*window);
   }

   for (const auto& dot : line)
      dot.render(*window);

   plusOne.render(*window);
   pauseButton.render(*window);

   player1.render(*window);
   player3.render(*window);

   if (leftDuo)
      player2.render(*window);

   if (rightDuo)
      player4.render(*window);

   if (state != St_t::winscreen)
      for (const auto& ball : balls)
         ball.ball.render(*window);

   if (state == St_t::paused || state == St_t::winscreen)
   {
      pauseText.render(*window);
      pauseButtons.render(*window);

      if (pauseButtons.any_selected())
         pointer.render(*window);
   }

   screenTint.render(*window);
}
