#include "blackgdk.h"

int main()
{
 char perfomance[8];
 bool limit;
 BLACKGDK::Common::Timer timer;
 BLACKGDK::Input::Keyboard keyboard;
 BLACKGDK::Input::Gamepad gamepad;
 BLACKGDK::Input::Mouse mouse;
 BLACKGDK::Misc::Audio media;
 BLACKGDK::Graphics::Screen screen;
 BLACKGDK::Graphics::Parallax sky;
 BLACKGDK::Graphics::Ribbon ship;
 BLACKGDK::Graphics::Text text;
 BLACKGDK::Tools::enable_logging("log.txt");
 keyboard.initialize();
 screen.initialize();
 sky.load("sky.tga");
 sky.prepare(screen);
 sky.set_vertical_speed(0.001f);
 ship.load("ship.tga",2);
 ship.set_position(screen.get_width()/2,screen.get_height()/2);
 text.load_font("font.tga");
 text.set_position(text.get_font_width(),text.get_font_height());
 mouse.hide();
 timer.set_timer(1);
 media.initialize();
 media.load("space.mp3");
 memset(perfomance,0,8);
 limit=true;
 while (screen.sync(limit))
 {
  gamepad.update();
  media.play_loop();
  if (mouse.check_press(BLACKGDK::MOUSE_LEFT)==true)
  {
   break;
  }
  if (keyboard.check_hold(57)==true)
  {
   break;
  }
  if (keyboard.check_hold(59)==true)
  {
   sky.disable_mirror();
  }
  if (keyboard.check_hold(60)==true)
  {
   sky.horizontal_mirror();
  }
  if (keyboard.check_hold(61)==true)
  {
   sky.vertical_mirror();
  }
  if (keyboard.check_hold(62)==true)
  {
   sky.complex_mirror();
  }
  if (keyboard.check_hold(63)==true)
  {
   limit=true;
  }
  if (keyboard.check_hold(64)==true)
  {
   limit=false;
  }
  if (keyboard.check_hold(72)==true)
  {
   ship.decrease_y();
  }
  if (keyboard.check_hold(80)==true)
  {
   ship.increase_y();
  }
  if (keyboard.check_hold(75)==true)
  {
   ship.decrease_x();
  }
  if (keyboard.check_hold(77)==true)
  {
   ship.increase_x();
  }
  if (gamepad.check_button_hold(BLACKGDK::GAMEPAD_A)==true)
  {
   break;
  }
  if (gamepad.check_button_hold(BLACKGDK::GAMEPAD_UP)==true)
  {
   ship.decrease_y();
  }
  if (gamepad.check_button_hold(BLACKGDK::GAMEPAD_DOWN)==true)
  {
   ship.increase_y();
  }
  if (gamepad.check_button_hold(BLACKGDK::GAMEPAD_LEFT)==true)
  {
   ship.decrease_x();
  }
  if (gamepad.check_button_hold(BLACKGDK::GAMEPAD_RIGHT)==true)
  {
   ship.increase_x();
  }

  if (gamepad.get_left_stick_x()==BLACKGDK::GAMEPAD_NEGATIVE_DIRECTION)
  {
   ship.decrease_x();
  }
  if (gamepad.get_left_stick_x()==BLACKGDK::GAMEPAD_POSITIVE_DIRECTION)
  {
   ship.increase_x();
  }
  if (gamepad.get_left_stick_y()==BLACKGDK::GAMEPAD_NEGATIVE_DIRECTION)
  {
   ship.decrease_y();
  }
  if (gamepad.get_left_stick_y()==BLACKGDK::GAMEPAD_POSITIVE_DIRECTION)
  {
   ship.increase_y();
  }
  if (gamepad.check_trigger_hold(BLACKGDK::GAMEPAD_LEFT_TRIGGER)==true)
  {
   gamepad.set_vibration(USHRT_MAX,USHRT_MAX);
  }
  if (gamepad.check_trigger_hold(BLACKGDK::GAMEPAD_RIGHT_TRIGGER)==true)
  {
   gamepad.disable_vibration();
  }
  if (screen.check_horizontal_border(ship.get_box())==true)
  {
   ship.decrease_x();
  }
  if (screen.check_vertical_border(ship.get_box())==true)
  {
   ship.decrease_y();
  }
  itoa(screen.get_fps(),perfomance,10);
  sky.draw();
  text.print(perfomance);
  ship.draw();
  if (timer.check_timer()==true)
  {
   ship.step();
  }

 }
 return 0;
}