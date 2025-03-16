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
 punk.set_size(64,128);
 camera.set_viewport(320,240);
 city.prepare(screen.get_width(),camera.get_viewport_height());
 camera.initialize(screen);
 camera.update();
 punk.set_position(0,camera.get_viewport_height()-punk.get_height());
 while (screen.sync())
 {
  if (keyboard.check_hold(1)==true)
  {
   break;
  }
  if (keyboard.check_hold(77)==true)
  {
   punk.increase_x(2);
  }
  if (keyboard.check_hold(75)==true)
  {
   punk.decrease_x(2);
  }
  if (camera.check_horizontal_border(punk.get_box())==true)
  {
   punk.decrease_x(3);
   camera.increase_x();
  }
  if (punk.get_x()<=camera.get_x())
  {
   camera.decrease_x();
  }
  camera.update();
  city.draw();
  punk.draw();
 }
 return 0;
}