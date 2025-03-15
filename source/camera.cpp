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
 punk.set_size(64,128);
 camera.set_viewport(320,240);
 city.prepare(camera.get_viewport_width()*4,camera.get_viewport_height());
 camera.initialize(screen);
 camera.update();
 punk.set_position(0,city.get_height()-punk.get_height());
 while (screen.sync())
 {
  if (keyboard.check_hold(1)==true)
  {
   break;
  }
  if (keyboard.check_hold(77)==true)
  {
   punk.increase_x();
  }
  if (keyboard.check_hold(75)==true)
  {
   punk.decrease_x();
  }
  if ((punk.get_x()+punk.get_width())>=camera.get_highest_x())
  {
   punk.decrease_x(6);
  }
  if (punk.get_x()<=camera.get_x())
  {
   camera.decrease_x();
  }
  if (camera.get_highest_x()>=city.get_width())
  {
   camera.decrease_x();
  }
  camera.update();
  city.draw();
  punk.draw();
 }
 return 0;
}