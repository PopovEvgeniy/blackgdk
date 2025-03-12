#include "blackgdk.h"

int main()
{
 BLACKGDK::Graphics::Screen screen;
 BLACKGDK::Graphics::Scene city;
 BLACKGDK::Graphics::Cartoon punk;
 BLACKGDK::Graphics::Text text;
 BLACKGDK::Graphics::Camera camera;
 BLACKGDK::Input::Keyboard keyboard;
 BLACKGDK::Input::Mouse mouse;
 keyboard.initialize();
 mouse.hide();
 screen.initialize();
 city.load("city.tga");
 punk.load("punk.tga");
 text.load_font("font.tga");
 city.prepare(screen);
 punk.set_size(192,384);
 punk.set_position(0,screen.get_height()-punk.get_height());
 camera.set_viewport(600,screen.get_height());
 camera.initialize(screen.get_width(),screen.get_height());
 while (screen.sync())
 {
  if (keyboard.check_hold(1)==true)
  {
   break;
  }
  if (keyboard.check_hold(77)==true)
  {
   punk.increase_x(4);
   camera.increase_x(6);
  }
  if (keyboard.check_hold(75)==true)
  {
   punk.decrease_x(4);
   camera.decrease_x(6);
  }
  camera.update();
  city.draw();
  punk.draw();
 }
 return 0;
}