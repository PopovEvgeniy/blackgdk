// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "blackgdk.h"

int main(void)
{
 long int x,y,screen_width,screen_height;
 char perfomance[8];
 BLACKGDK::Timer timer;
 BLACKGDK::Screen screen;
 BLACKGDK::System System;
 BLACKGDK::Keyboard keyboard;
 BLACKGDK::Gamepad gamepad;
 BLACKGDK::Mouse mouse;
 BLACKGDK::Multimedia media;
 BLACKGDK::Image image;
 BLACKGDK::Background space;
 BLACKGDK::Sprite ship,font;
 BLACKGDK::Text text;
 System.enable_logging("log.txt");
 keyboard.initialize();
 space.initialize(screen.get_handle());
 ship.initialize(screen.get_handle());
 font.initialize(screen.get_handle());
 screen_width=screen.get_frame_width();
 screen_height=screen.get_frame_height();
 x=screen_width/2;
 y=screen_height/2;
 image.load_tga("space.tga");
 space.load_image(image);
 image.load_tga("ship.tga");
 ship.load_sprite(image,HORIZONTAL_STRIP,2);
 image.load_tga("font.tga");
 font.load_image(image);
 text.load_font(font.get_handle());
 text.set_position(font.get_width(),font.get_width());
 screen.clear_screen();
 space.resize_image(screen_width,screen_height);
 space.set_kind(NORMAL_BACKGROUND);
 mouse.hide();
 timer.set_timer(1);
 media.initialize();
 media.load("space.mp3");
 memset(perfomance,0,8);
 screen.initialize();
 while(screen.sync())
 {
  gamepad.update();
  if(media.check_playing()==false) media.play();
  if(mouse.check_press(MOUSE_LEFT)==true) break;
  if(keyboard.check_hold(57)==true) break;
  if(keyboard.check_hold(72)==true) y-=2;
  if(keyboard.check_hold(80)==true) y+=2;
  if(keyboard.check_hold(75)==true) x-=2;
  if(keyboard.check_hold(77)==true) x+=2;
  if(keyboard.check_press(71)==true) ship.mirror_image(MIRROR_HORIZONTAL);
  if(keyboard.check_press(79)==true) ship.mirror_image(MIRROR_VERTICAL);
  if(gamepad.check_button_press(GAMEPAD_X)==true) break;
  if(gamepad.check_button_hold(GAMEPAD_UP)==true) y-=2;
  if(gamepad.check_button_hold(GAMEPAD_DOWN)==true) y+=2;
  if(gamepad.check_button_hold(GAMEPAD_LEFT)==true) x-=2;
  if(gamepad.check_button_hold(GAMEPAD_RIGHT)==true) x+=2;
  if(gamepad.get_stick_x(GAMEPAD_LEFT_STICK)==GAMEPAD_POSITIVE_DIRECTION) x+=2;
  if(gamepad.get_stick_x(GAMEPAD_LEFT_STICK)==GAMEPAD_NEGATIVE_DIRECTION) x-=2;
  if(gamepad.get_stick_y(GAMEPAD_LEFT_STICK)==GAMEPAD_POSITIVE_DIRECTION) y-=2;
  if(gamepad.get_stick_y(GAMEPAD_LEFT_STICK)==GAMEPAD_NEGATIVE_DIRECTION) y+=2;
  if(gamepad.check_trigger_hold(GAMEPAD_LEFT_TRIGGER)==true) gamepad.set_vibration(65535,65535);
  if(gamepad.check_trigger_hold(GAMEPAD_RIGHT_TRIGGER)==true) gamepad.set_vibration(0,0);
  if(gamepad.check_button_press(GAMEPAD_A)==true) ship.mirror_image(MIRROR_HORIZONTAL);
  if(gamepad.check_button_press(GAMEPAD_B)==true) ship.mirror_image(MIRROR_VERTICAL);
  if((x<=0)||(x>=screen_width)) x=screen_width/2;
  if((y<=0)||(y>=screen_height)) y=screen_height/2;
  itoa(screen.get_fps(),perfomance,10);
  space.draw_background();
  text.draw_text(perfomance);
  ship.set_position(x,y);
  ship.draw_sprite();
  if (timer.check_timer()==true)
  {
   ship.step();
  }

 }
 return 0;
}