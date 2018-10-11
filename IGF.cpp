// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright (C) 2017-2018 Popov Evgeniy Alekseyevich

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "IGF.h"

unsigned char IGF_Keys[IGF_KEYBOARD];
unsigned char IGF_Buttons[IGF_MOUSE];

LRESULT CALLBACK IGF_Process_Message(HWND window,UINT Message,WPARAM wParam,LPARAM lParam)
{
 switch (Message)
 {
  case WM_CLOSE:
  DestroyWindow(window);
  break;
  case WM_DESTROY:
  PostQuitMessage(0);
  break;
  case WM_CREATE:
  memset(IGF_Keys,IGFKEY_RELEASE,IGF_KEYBOARD);
  memset(IGF_Buttons,IGFKEY_RELEASE,IGF_MOUSE);
  break;
  case WM_LBUTTONDOWN:
  IGF_Buttons[IGF_MOUSE_LEFT]=IGFKEY_PRESS;
  break;
  case WM_LBUTTONUP:
  IGF_Buttons[IGF_MOUSE_LEFT]=IGFKEY_RELEASE;
  break;
  case WM_RBUTTONDOWN:
  IGF_Buttons[IGF_MOUSE_RIGHT]=IGFKEY_PRESS;
  break;
  case WM_RBUTTONUP:
  IGF_Buttons[IGF_MOUSE_RIGHT]=IGFKEY_RELEASE;
  break;
  case WM_MBUTTONDOWN:
  IGF_Buttons[IGF_MOUSE_MIDDLE]=IGFKEY_PRESS;
  break;
  case WM_MBUTTONUP:
  IGF_Buttons[IGF_MOUSE_MIDDLE]=IGFKEY_RELEASE;
  break;
  case WM_KEYDOWN:
  IGF_Keys[IGF_GETSCANCODE(lParam)]=IGFKEY_PRESS;
  break;
  case WM_KEYUP:
  IGF_Keys[IGF_GETSCANCODE(lParam)]=IGFKEY_RELEASE;
  break;
 }
 return DefWindowProc(window,Message,wParam,lParam);
}

void IGF_Show_Error(const char *message)
{
 puts(message);
 exit(EXIT_FAILURE);
}

IGF_Base::IGF_Base()
{
 HRESULT status;
 status=CoInitialize(NULL);
 if(status!=S_OK)
 {
  if(status!=S_FALSE)
  {
   IGF_Show_Error("Can't initialize COM");
  }

 }

}

IGF_Base::~IGF_Base()
{
 CoUninitialize();
}

IGF_Synchronization::IGF_Synchronization()
{
 timer=NULL;
}

IGF_Synchronization::~IGF_Synchronization()
{
 if(timer==NULL)
 {
  CancelWaitableTimer(timer);
  CloseHandle(timer);
 }

}

void IGF_Synchronization::create_timer()
{
 timer=CreateWaitableTimer(NULL,FALSE,NULL);
 if (timer==NULL)
 {
  IGF_Show_Error("Can't create synchronization timer");
 }

}

void IGF_Synchronization::set_timer(const unsigned long int interval)
{
 LARGE_INTEGER start;
 start.QuadPart=0;
 if(SetWaitableTimer(timer,&start,interval,NULL,NULL,FALSE)==FALSE)
 {
  IGF_Show_Error("Can't set timer");
 }

}

void IGF_Synchronization::wait_timer()
{
 WaitForSingleObject(timer,INFINITE);
}

IGF_Engine::IGF_Engine()
{
 window_class.lpszClassName=TEXT("IGF");
 window_class.style=CS_HREDRAW|CS_VREDRAW;
 window_class.lpfnWndProc=(WNDPROC)IGF_Process_Message;
 window_class.hInstance=NULL;
 window_class.hbrBackground=NULL;
 window_class.hIcon=NULL;
 window_class.hCursor=NULL;
 window_class.cbClsExtra=0;
 window_class.cbWndExtra=0;
 window=NULL;
 width=0;
 height=0;
}

IGF_Engine::~IGF_Engine()
{
 if(window!=NULL) CloseWindow(window);
 UnregisterClass(window_class.lpszClassName,window_class.hInstance);
}

HWND IGF_Engine::get_window()
{
 return window;
}

void IGF_Engine::prepare_engine()
{
 window_class.hInstance=GetModuleHandle(NULL);
 if(window_class.hInstance==NULL)
 {
  IGF_Show_Error("Can't get the application instance");
 }
 window_class.hIcon=LoadIcon(NULL,IDI_APPLICATION);
 if (window_class.hIcon==NULL)
 {
  IGF_Show_Error("Can't load the standart program icon");
 }
 window_class.hCursor=LoadCursor(NULL,IDC_ARROW);
 if (window_class.hCursor==NULL)
 {
  IGF_Show_Error("Can't load the standart cursor");
 }
 if (!RegisterClass(&window_class))
 {
  IGF_Show_Error("Can't register window class");
 }

}

void IGF_Engine::create_window()
{
 width=GetSystemMetrics(SM_CXSCREEN);
 height=GetSystemMetrics(SM_CYSCREEN);
 window=CreateWindow(window_class.lpszClassName,NULL,WS_VISIBLE|WS_POPUP,0,0,width,height,NULL,NULL,window_class.hInstance,NULL);
 if (window==NULL)
 {
  IGF_Show_Error("Can't create window");
 }
 EnableWindow(window,TRUE);
 SetFocus(window);
}

void IGF_Engine::capture_mouse()
{
 RECT border;
 if(GetClientRect(window,&border)==FALSE)
 {
  IGF_Show_Error("Can't capture window");
 }
 if(ClipCursor(&border)==FALSE)
 {
  IGF_Show_Error("Can't capture cursor");
 }

}

bool IGF_Engine::process_message()
{
 bool quit;
 MSG Message;
 quit=false;
 while(PeekMessage(&Message,window,0,0,PM_NOREMOVE)==TRUE)
 {
  if(GetMessage(&Message,window,0,0)==TRUE)
  {
   TranslateMessage(&Message);
   DispatchMessage(&Message);
  }
  else
  {
   quit=true;
   break;
  }

 }
 return quit;
}

unsigned long int IGF_Engine::get_width()
{
 return width;
}

unsigned long int IGF_Engine::get_height()
{
 return height;
}

IGF_Frame::IGF_Frame()
{
 frame_width=512;
 frame_height=512;
 frame_line=0;
 buffer_length=0;
 buffer=NULL;
}

IGF_Frame::~IGF_Frame()
{
 if(buffer!=NULL)
 {
  free(buffer);
  buffer=NULL;
 }

}

void IGF_Frame::set_size(const IGF_SURFACE surface)
{
 if(surface==IGF_SURFACE_SMALL)
 {
  frame_width=256;
  frame_height=256;
 }
 if(surface==IGF_SURFACE_LARGE)
 {
  frame_width=512;
  frame_height=512;
 }

}

void IGF_Frame::create_render_buffer()
{
 frame_line=(unsigned long int)sizeof(unsigned int)*frame_width;
 buffer_length=(size_t)frame_width*(size_t)frame_height;
 buffer=(unsigned int*)calloc(buffer_length,sizeof(unsigned int));
 if(buffer==NULL)
 {
  IGF_Show_Error("Can't allocate memory for render buffer");
 }
 else
 {
  buffer_length*=sizeof(unsigned int);
 }

}

unsigned int IGF_Frame::get_rgb(const unsigned int red,const unsigned int green,const unsigned int blue)
{
 return red+(green<<8)+(blue<<16);
}

unsigned long int IGF_Frame::get_frame_line()
{
 return frame_line;
}

unsigned int *IGF_Frame::get_buffer()
{
 return buffer;
}

void IGF_Frame::draw_pixel(const unsigned long int x,const unsigned long int y,const unsigned char red,const unsigned char green,const unsigned char blue)
{
 if((x<frame_width)&&(y<frame_height))
 {
  buffer[(size_t)x+(size_t)y*(size_t)frame_width]=this->get_rgb(blue,green,red);
 }

}

void IGF_Frame::clear_screen()
{
 memset(buffer,0,buffer_length);
}

unsigned long int IGF_Frame::get_frame_width()
{
 return frame_width;
}

unsigned long int IGF_Frame::get_frame_height()
{
 return frame_height;
}

IGF_Render::IGF_Render()
{
 render=NULL;
 target=NULL;
 surface=NULL;
 source=D2D1::RectU(0,0,0,0);
 destanation=D2D1::RectF(0,0,0,0);
 texture=D2D1::RectF(0,0,0,0);
 setting.dpiX=0;
 setting.dpiY=0;
 setting.type=D2D1_RENDER_TARGET_TYPE_HARDWARE;
 setting.pixelFormat=D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_IGNORE);
 setting.usage=D2D1_RENDER_TARGET_USAGE_NONE;
 setting.minLevel=D2D1_FEATURE_LEVEL_9;
 configuration.hwnd=NULL;
 configuration.pixelSize=D2D1::SizeU(0,0);
 configuration.presentOptions=D2D1_PRESENT_OPTIONS_IMMEDIATELY;
}

IGF_Render::~IGF_Render()
{
 if(surface!=NULL) surface->Release();
 if(target!=NULL) target->Release();
 if(render!=NULL) render->Release();
}

void IGF_Render::create_factory()
{
 if(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,&render)!=S_OK)
 {
  IGF_Show_Error("Can't create render");
 }

}

void IGF_Render::create_target()
{
 if(render->CreateHwndRenderTarget(setting,configuration,&target)!=S_OK)
 {
  IGF_Show_Error("Can't create render target");
 }
 target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
}

void IGF_Render::create_surface()
{
 if(target->CreateBitmap(D2D1::SizeU(this->get_frame_width(),this->get_frame_height()),this->get_buffer(),this->get_frame_line(),D2D1::BitmapProperties(setting.pixelFormat,96.0,96.0),&surface)!=S_OK)
 {
  IGF_Show_Error("Can't create render surface");
 }

}

void IGF_Render::set_render_setting()
{
 configuration.hwnd=this->get_window();
 configuration.pixelSize=D2D1::SizeU(this->get_width(),this->get_height());
}

void IGF_Render::set_render()
{
 this->set_render_setting();
 this->create_factory();
 this->create_target();
 this->create_surface();
}

void IGF_Render::destroy_resource()
{
 if(surface!=NULL)
 {
  surface->Release();
  surface=NULL;
 }
 if(target!=NULL)
 {
  target->Release();
  target=NULL;
 }

}

void IGF_Render::recreate_render()
{
 this->destroy_resource();
 this->create_target();
 this->create_surface();
}

void IGF_Render::prepare_surface()
{
 source=D2D1::RectU(0,0,this->get_frame_width(),this->get_frame_height());
 destanation=D2D1::RectF(0,0,this->get_width(),this->get_height());
 texture=D2D1::RectF(0,0,this->get_frame_width(),this->get_frame_height());
}

void IGF_Render::create_render()
{
 this->set_render_setting();
 this->set_render();
 this->prepare_surface();
 this->create_render_buffer();
}

void IGF_Render::refresh()
{
 surface->CopyFromMemory(&source,this->get_buffer(),this->get_frame_line());
 target->BeginDraw();
 target->DrawBitmap(surface,destanation,1.0,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,texture);
 if(target->EndDraw()==(HRESULT)D2DERR_RECREATE_TARGET) this->recreate_render();
}

void IGF_Screen::initialize()
{
 this->prepare_engine();
 this->create_render_buffer();
 this->create_timer();
 this->create_window();
 this->capture_mouse();
 this->create_render();
 this->set_timer(17);
}

void IGF_Screen::initialize(const IGF_SURFACE surface)
{
 this->set_size(surface);
 this->initialize();
}

bool IGF_Screen::update()
{
 this->refresh();
 return this->process_message();
}

bool IGF_Screen::sync()
{
 bool quit;
 quit=this->update();
 this->wait_timer();
 return quit;
}

IGF_Screen* IGF_Screen::get_handle()
{
 return this;
}

IGF_Keyboard::IGF_Keyboard()
{
 preversion=NULL;
}

IGF_Keyboard::~IGF_Keyboard()
{
 if(preversion!=NULL) free(preversion);
}

void IGF_Keyboard::initialize()
{
 preversion=(unsigned char*)calloc(IGF_KEYBOARD,sizeof(unsigned char));
 if(preversion==NULL)
 {
  IGF_Show_Error("Can't allocate memory for keyboard state buffer");
 }

}

bool IGF_Keyboard::check_hold(const unsigned char code)
{
 bool result;
 result=false;
 if(IGF_Keys[code]==IGFKEY_PRESS) result=true;
 preversion[code]=IGF_Keys[code];
 return result;
}

bool IGF_Keyboard::check_press(const unsigned char code)
{
 bool result;
 result=false;
 if(IGF_Keys[code]==IGFKEY_PRESS)
 {
  if(preversion[code]==IGFKEY_RELEASE) result=true;
 }
 preversion[code]=IGF_Keys[code];
 return result;
}

bool IGF_Keyboard::check_release(const unsigned char code)
{
 bool result;
 result=false;
 if(IGF_Keys[code]==IGFKEY_RELEASE)
 {
  if(preversion[code]==IGFKEY_PRESS) result=true;
 }
 preversion[code]=IGF_Keys[code];
 return result;
}

IGF_Mouse::IGF_Mouse()
{
 memset(preversion,IGFKEY_RELEASE,IGF_MOUSE);
}

IGF_Mouse::~IGF_Mouse()
{
 while(ShowCursor(TRUE)<1) ;
}

void IGF_Mouse::show()
{
 while(ShowCursor(TRUE)<1) ;
}

void IGF_Mouse::hide()
{
 while(ShowCursor(FALSE)>-2) ;
}

void IGF_Mouse::set_position(const unsigned long int x,const unsigned long int y)
{
 if(SetCursorPos(x,y)==FALSE)
 {
  IGF_Show_Error("Can't set the mouse cursor position");
 }

}

unsigned long int IGF_Mouse::get_x()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  IGF_Show_Error("Can't get the mouse cursor position");
 }
 return position.x;
}

unsigned long int IGF_Mouse::get_y()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  IGF_Show_Error("Can't get the mouse cursor position");
 }
 return position.y;
}

bool IGF_Mouse::check_hold(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=IGF_MOUSE_MIDDLE)
 {
  if(IGF_Buttons[button]==IGFKEY_PRESS) result=true;
   preversion[button]=IGF_Buttons[button];
 }
 return result;
}

bool IGF_Mouse::check_press(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=IGF_MOUSE_MIDDLE)
 {
  if(IGF_Buttons[button]==IGFKEY_PRESS)
  {
   if(preversion[button]==IGFKEY_RELEASE) result=true;
  }

 }
 preversion[button]=IGF_Buttons[button];
 return result;
}

bool IGF_Mouse::check_release(const unsigned char button)
{
 bool result;
 result=false;
 if(button<=IGF_MOUSE_MIDDLE)
 {
  if(IGF_Buttons[button]==IGFKEY_RELEASE)
  {
   if(preversion[button]==IGFKEY_PRESS) result=true;
  }

 }
 preversion[button]=IGF_Buttons[button];
 return result;
}

IGF_Gamepad::IGF_Gamepad()
{
 length=sizeof(XINPUT_STATE);
 XInputEnable(TRUE);
 memset(&current,0,length);
 memset(&preversion,0,length);
 memset(&vibration,0,sizeof(XINPUT_VIBRATION));
 memset(&battery,0,sizeof(XINPUT_BATTERY_INFORMATION));
 active=0;
}

IGF_Gamepad::~IGF_Gamepad()
{
 XInputEnable(FALSE);
}

bool IGF_Gamepad::read_battery_status()
{
 bool result;
 result=false;
 if(XInputGetBatteryInformation(active,BATTERY_DEVTYPE_GAMEPAD,&battery)==ERROR_SUCCESS) return result;
 return result;
}

void IGF_Gamepad::clear_state()
{
 memset(&current,0,length);
 memset(&preversion,0,length);
}

bool IGF_Gamepad::read_state()
{
 bool result;
 result=false;
 if(XInputGetState(active,&current)==ERROR_SUCCESS) result=true;
 return result;
}

bool IGF_Gamepad::write_state()
{
 bool result;
 result=false;
 if(XInputSetState(active,&vibration)==ERROR_SUCCESS) result=true;
 return result;
}

void IGF_Gamepad::set_motor(const unsigned short int left,const unsigned short int right)
{
 vibration.wLeftMotorSpeed=left;
 vibration.wRightMotorSpeed=right;
}

bool IGF_Gamepad::check_button(XINPUT_STATE &target,const IGF_GAMEPAD_BUTTONS button)
{
 bool result;
 result=false;
 if(target.Gamepad.wButtons&button) result=true;
 return result;
}

bool IGF_Gamepad::check_trigger(XINPUT_STATE &target,const IGF_GAMEPAD_TRIGGERS trigger)
{
 bool result;
 result=false;
 if((trigger==IGF_GAMEPAD_LEFT_TRIGGER)&&(target.Gamepad.bLeftTrigger>=XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) result=true;
 if((trigger==IGF_GAMEPAD_RIGHT_TRIGGER)&&(target.Gamepad.bRightTrigger>=XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) result=true;
 return result;
}

void IGF_Gamepad::set_active(const unsigned long int gamepad)
{
 if(active<XUSER_MAX_COUNT)
 {
  this->clear_state();
  active=gamepad;
 }

}

unsigned long int IGF_Gamepad::get_active()
{
 return active;
}

unsigned long int IGF_Gamepad::get_last_index()
{
 return XUSER_MAX_COUNT-1;
}

unsigned long int IGF_Gamepad::get_maximum_amount()
{
 return XUSER_MAX_COUNT;
}

unsigned long int IGF_Gamepad::get_amount()
{
 unsigned long int old,result;
 result=0;
 old=active;
 for(active=0;active<XUSER_MAX_COUNT;++active)
 {
  if(this->read_state()==true)
  {
   this->clear_state();
   result=active+1;
  }

 }
 active=old;
 return result;
}

bool IGF_Gamepad::check_connection()
{
 return this->read_state();
}

bool IGF_Gamepad::is_wireless()
{
 bool result;
 result=false;
 if(this->read_battery_status()==true)
 {
  if(battery.BatteryType!=BATTERY_TYPE_WIRED) result=true;
 }
 return result;
}

IGF_GAMEPAD_BATTERY_TYPE IGF_Gamepad::get_battery_type()
{
 IGF_GAMEPAD_BATTERY_TYPE result;
 result=IGF_GAMEPAD_BATTERY_TYPE_ERROR;
 if(this->read_battery_status()==true)
 {
  switch (battery.BatteryType)
  {
   case BATTERY_TYPE_ALKALINE:
   result=IGF_GAMEPAD_BATTERY_ALKAINE;
   break;
   case BATTERY_TYPE_NIMH:
   result=IGF_GAMEPAD_BATTERY_NIMH;
   break;
   case BATTERY_TYPE_UNKNOWN:
   result=IGF_GAMEPAD_BATTERY_UNKNOW;
   break;
  }

 }
 return result;
}

IGF_GAMEPAD_BATTERY_LEVEL IGF_Gamepad::get_battery_level()
{
 IGF_GAMEPAD_BATTERY_LEVEL result;
 result=IGF_GAMEPAD_BATTERY_LEVEL_ERROR;
 if(this->read_battery_status()==true)
 {
  switch (battery.BatteryLevel)
  {
   case BATTERY_LEVEL_EMPTY:
   result=IGF_GAMEPAD_BATTERY_EMPTY;
   break;
   case BATTERY_LEVEL_LOW:
   result=IGF_GAMEPAD_BATTERY_LOW;
   break;
   case BATTERY_LEVEL_MEDIUM:
   result=IGF_GAMEPAD_BATTERY_MEDIUM;
   break;
   case BATTERY_LEVEL_FULL:
   result=IGF_GAMEPAD_BATTERY_FULL;
   break;
  }
  if((battery.BatteryType==BATTERY_TYPE_WIRED)||(battery.BatteryType==BATTERY_TYPE_DISCONNECTED)) result=IGF_GAMEPAD_BATTERY_LEVEL_ERROR;
 }
 return result;
}

void IGF_Gamepad::update()
{
 preversion=current;
 if(this->read_state()==false) this->clear_state();
}

bool IGF_Gamepad::check_button_hold(const IGF_GAMEPAD_BUTTONS button)
{
 return this->check_button(current,button);
}

bool IGF_Gamepad::check_button_press(const IGF_GAMEPAD_BUTTONS button)
{
 bool result;
 result=false;
 if(this->check_button(current,button)==true)
 {
  if(this->check_button(preversion,button)==false) result=true;
 }
 return result;
}

bool IGF_Gamepad::check_button_release(const IGF_GAMEPAD_BUTTONS button)
{
 bool result;
 result=false;
 if(this->check_button(current,button)==false)
 {
  if(this->check_button(preversion,button)==true) result=true;
 }
 return result;
}

bool IGF_Gamepad::check_trigger_hold(const IGF_GAMEPAD_TRIGGERS trigger)
{
 return this->check_trigger(current,trigger);
}

bool IGF_Gamepad::check_trigger_press(const IGF_GAMEPAD_TRIGGERS trigger)
{
 bool result;
 result=false;
 if(this->check_trigger(current,trigger)==true)
 {
  if(this->check_trigger(preversion,trigger)==false) result=true;
 }
 return result;
}

bool IGF_Gamepad::check_trigger_release(const IGF_GAMEPAD_TRIGGERS trigger)
{
 bool result;
 result=false;
 if(this->check_trigger(current,trigger)==false)
 {
  if(this->check_trigger(preversion,trigger)==true) result=true;
 }
 return result;
}

unsigned char IGF_Gamepad::get_trigger(const IGF_GAMEPAD_TRIGGERS trigger)
{
 unsigned char result;
 result=0;
 if(trigger==IGF_GAMEPAD_LEFT_TRIGGER) result=current.Gamepad.bLeftTrigger;
 if(trigger==IGF_GAMEPAD_RIGHT_TRIGGER) result=current.Gamepad.bRightTrigger;
 return result;
}

bool IGF_Gamepad::set_vibration(const unsigned short int left,const unsigned short int right)
{
 this->set_motor(left,right);
 return this->write_state();
}

IGF_GAMEPAD_DIRECTION IGF_Gamepad::get_stick_x(const IGF_GAMEPAD_STICKS stick)
{
 IGF_GAMEPAD_DIRECTION result;
 short int control;
 result=IGF_NEUTRAL_DIRECTION;
 if(stick==IGF_GAMEPAD_LEFT_STICK)
 {
  control=SHRT_MAX-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbLX>=control) result=IGF_POSITIVE_DIRECTION;
  if(current.Gamepad.sThumbLX<=-1*control) result=IGF_NEGATIVE_DIRECTION;
 }
 if(stick==IGF_GAMEPAD_RIGHT_STICK)
 {
  control=SHRT_MAX-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbRX>=control) result=IGF_POSITIVE_DIRECTION;
  if(current.Gamepad.sThumbRX<=-1*control) result=IGF_NEGATIVE_DIRECTION;
 }
 return result;
}

IGF_GAMEPAD_DIRECTION IGF_Gamepad::get_stick_y(const IGF_GAMEPAD_STICKS stick)
{
 IGF_GAMEPAD_DIRECTION result;
 short int control;
 result=IGF_NEUTRAL_DIRECTION;
 if(stick==IGF_GAMEPAD_LEFT_STICK)
 {
  control=SHRT_MAX-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbLY>=control) result=IGF_POSITIVE_DIRECTION;
  if(current.Gamepad.sThumbLY<=-1*control) result=IGF_NEGATIVE_DIRECTION;
 }
 if(stick==IGF_GAMEPAD_RIGHT_STICK)
 {
  control=SHRT_MAX-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbRY>=control) result=IGF_POSITIVE_DIRECTION;
  if(current.Gamepad.sThumbRY<=-1*control) result=IGF_NEGATIVE_DIRECTION;
 }
 return result;
}

IGF_Multimedia::IGF_Multimedia()
{
 loader=NULL;
 player=NULL;
 controler=NULL;
 video=NULL;
}

IGF_Multimedia::~IGF_Multimedia()
{
 if(player!=NULL) player->StopWhenReady();
 if(video!=NULL) video->Release();
 if(controler!=NULL) controler->Release();
 if(player!=NULL) player->Release();
 if(loader!=NULL) loader->Release();
}

wchar_t *IGF_Multimedia::convert_file_name(const char *target)
{
 wchar_t *name;
 size_t index,length;
 length=strlen(target);
 name=(wchar_t*)calloc(length+1,sizeof(wchar_t));
 if(name==NULL)
 {
  IGF_Show_Error("Can't allocate memory");
 }
 for(index=0;index<length;++index) name[index]=btowc(target[index]);
 return name;
}

void IGF_Multimedia::open(const wchar_t *target)
{
 player->StopWhenReady();
 if(loader->RenderFile(target,NULL)!=S_OK)
 {
  IGF_Show_Error("Can't load a multimedia file");
 }
 video->put_FullScreenMode(OATRUE);
}

bool IGF_Multimedia::is_end()
{
 bool result;
 long long current,stop;
 result=false;
 if(controler->GetPositions(&current,&stop)==S_OK)
 {
  if(current>=stop) result=true;
 }
 else
 {
  IGF_Show_Error("Can't get the current and the end position");
 }
 return result;
}

void IGF_Multimedia::rewind()
{
 long long position;
 position=0;
 if(controler->SetPositions(&position,AM_SEEKING_AbsolutePositioning,NULL,AM_SEEKING_NoPositioning)!=S_OK)
 {
  IGF_Show_Error("Can't set start position");
 }

}

void IGF_Multimedia::initialize()
{
 if(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void**)&loader)!=S_OK)
 {
  IGF_Show_Error("Can't create a multimedia loader");
 }
 if(loader->QueryInterface(IID_IMediaControl,(void**)&player)!=S_OK)
 {
  IGF_Show_Error("Can't create a multimedia player");
 }
 if(loader->QueryInterface(IID_IMediaSeeking,(void**)&controler)!=S_OK)
 {
  IGF_Show_Error("Can't create a player controler");
 }
 if(loader->QueryInterface(IID_IVideoWindow,(void**)&video)!=S_OK)
 {
  IGF_Show_Error("Can't create a video player");
 }

}

void IGF_Multimedia::load(const char *target)
{
 wchar_t *name;
 name=this->convert_file_name(target);
 this->open(name);
 free(name);
}

bool IGF_Multimedia::check_playing()
{
 OAFilterState state;
 bool result;
 result=false;
 if(player->GetState(INFINITE,&state)==E_FAIL)
 {
  IGF_Show_Error("Can't get the multimedia state");
 }
 else
 {
  if(state==State_Running)
  {
   if(this->is_end()==false) result=true;
  }

 }
 return result;
}

void IGF_Multimedia::stop()
{
 player->StopWhenReady();
}

void IGF_Multimedia::play()
{
 this->stop();
 this->rewind();
 player->Run();
}

IGF_Memory::IGF_Memory()
{
 memset(&memory,0,sizeof(MEMORYSTATUSEX));
 memory.dwLength=sizeof(MEMORYSTATUSEX);
}

IGF_Memory::~IGF_Memory()
{

}

void IGF_Memory::get_status()
{
 if(GlobalMemoryStatusEx(&memory)==FALSE)
 {
  IGF_Show_Error("Can't get the memory status");
 }

}

unsigned long long int IGF_Memory::get_total_memory()
{
 this->get_status();
 return memory.ullTotalPhys;
}

unsigned long long int IGF_Memory::get_free_memory()
{
 this->get_status();
 return memory.ullAvailPhys;
}

IGF_System::IGF_System()
{
 srand(time(NULL));
}

IGF_System::~IGF_System()
{

}

unsigned long int IGF_System::get_random(const unsigned long int number)
{
 return rand()%number;
}

void IGF_System::quit()
{
 exit(EXIT_SUCCESS);
}

void IGF_System::run(const char *command)
{
 system(command);
}

char* IGF_System::read_environment(const char *variable)
{
 return getenv(variable);
}

void IGF_System::enable_logging(const char *name)
{
 if(freopen(name,"wt",stdout)==NULL)
 {
  IGF_Show_Error("Can't create log file");
 }

}

IGF_File::IGF_File()
{
 target=NULL;
}

IGF_File::~IGF_File()
{
 if(target!=NULL) fclose(target);
}

void IGF_File::open(const char *name)
{
 target=fopen(name,"w+b");
 if(target==NULL)
 {
  IGF_Show_Error("Can't open the binary file");
 }

}

void IGF_File::close()
{
 if(target!=NULL) fclose(target);
}

void IGF_File::set_position(const off_t offset)
{
 fseek(target,offset,SEEK_SET);
}

long int IGF_File::get_position()
{
 return ftell(target);
}

long int IGF_File::get_length()
{
 long int result;
 fseek(target,0,SEEK_END);
 result=ftell(target);
 rewind(target);
 return result;
}

void IGF_File::read(void *buffer,const size_t length)
{
 fread(buffer,length,1,target);
}

void IGF_File::write(void *buffer,const size_t length)
{
 fwrite(buffer,length,1,target);
}

bool IGF_File::check_error()
{
 bool result;
 result=false;
 if(ferror(target)!=0) result=true;
 return result;
}

IGF_Timer::IGF_Timer()
{
 interval=0;
 start=time(NULL);
}

IGF_Timer::~IGF_Timer()
{

}

void IGF_Timer::set_timer(const unsigned long int seconds)
{
 interval=seconds;
 start=time(NULL);
}

bool IGF_Timer::check_timer()
{
 bool result;
 time_t stop;
 result=false;
 stop=time(NULL);
 if(difftime(stop,start)>=interval)
 {
  result=true;
  start=time(NULL);
 }
 return result;
}

IGF_Primitive::IGF_Primitive()
{
 color.red=0;
 color.green=0;
 color.blue=0;
 surface=NULL;
}

IGF_Primitive::~IGF_Primitive()
{
 color.red=0;
 color.green=0;
 color.blue=0;
 surface=NULL;
}

void IGF_Primitive::initialize(IGF_Screen *Screen)
{
 surface=Screen;
}

void IGF_Primitive::set_color(const unsigned char red,const unsigned char green,const unsigned char blue)
{
 color.red=red;
 color.green=green;
 color.blue=blue;
}

void IGF_Primitive::draw_line(const unsigned long int x1,const unsigned long int y1,const unsigned long int x2,const unsigned long int y2)
{
 unsigned long int delta_x,delta_y,index,steps;
 float x,y,shift_x,shift_y;
 if (x1>x2)
 {
  delta_x=x1-x2;
 }
 else
 {
  delta_x=x2-x1;
 }
 if (y1>y2)
 {
  delta_y=y1-y2;
 }
 else
 {
  delta_y=y2-y1;
 }
 steps=delta_x;
 if (steps<delta_y) steps=delta_y;
 x=x1;
 y=y1;
 shift_x=(float)delta_x/(float)steps;
 shift_y=(float)delta_y/(float)steps;
 for (index=steps;index>0;--index)
 {
  x+=shift_x;
  y+=shift_y;
  surface->draw_pixel(x,y,color.red,color.green,color.blue);
 }

}

void IGF_Primitive::draw_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height)
{
 unsigned long int stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 this->draw_line(x,y,stop_x,y);
 this->draw_line(x,stop_y,stop_x,stop_y);
 this->draw_line(x,y,x,stop_y);
 this->draw_line(stop_x,y,stop_x,stop_y);
}

void IGF_Primitive::draw_filled_rectangle(const unsigned long int x,const unsigned long int y,const unsigned long int width,const unsigned long int height)
{
 unsigned long int step_x,step_y,stop_x,stop_y;
 stop_x=x+width;
 stop_y=y+height;
 for(step_x=x;step_x<stop_x;++step_x)
 {
  for(step_y=y;step_y<stop_y;++step_y)
  {
   surface->draw_pixel(step_x,step_y,color.red,color.green,color.blue);
  }

 }

}

IGF_Image::IGF_Image()
{
 width=0;
 height=0;
 data=NULL;
}

IGF_Image::~IGF_Image()
{
 if(data!=NULL) free(data);
}

unsigned char *IGF_Image::create_buffer(const size_t length)
{
 unsigned char *result;
 result=(unsigned char*)calloc(length,sizeof(unsigned char));
 if(result==NULL)
 {
  IGF_Show_Error("Can't allocate memory for image buffer");
 }
 return result;
}

void IGF_Image::clear_buffer()
{
 if(data!=NULL)
 {
  free(data);
  data=NULL;
 }

}

FILE *IGF_Image::open_image(const char *name)
{
 FILE *target;
 target=fopen(name,"rb");
 if(target==NULL)
 {
  IGF_Show_Error("Can't open a image file");
 }
 return target;
}

unsigned long int IGF_Image::get_file_size(FILE *target)
{
 unsigned long int length;
 fseek(target,0,SEEK_END);
 length=ftell(target);
 rewind(target);
 return length;
}

void IGF_Image::load_tga(const char *name)
{
 FILE *target;
 size_t index,position,amount,compressed_length,uncompressed_length;
 unsigned char *compressed;
 unsigned char *uncompressed;
 TGA_head head;
 TGA_map color_map;
 TGA_image image;
 this->clear_buffer();
 target=this->open_image(name);
 compressed_length=(size_t)this->get_file_size(target)-18;
 fread(&head,3,1,target);
 fread(&color_map,5,1,target);
 fread(&image,10,1,target);
 if((head.color_map!=0)||(image.color!=24))
 {
  IGF_Show_Error("Invalid image format");
 }
 if(head.type!=2)
 {
  if(head.type!=10)
  {
   IGF_Show_Error("Invalid image format");
  }

 }
 index=0;
 position=0;
 width=image.width;
 height=image.height;
 uncompressed_length=this->get_data_length();
 uncompressed=this->create_buffer(uncompressed_length);
 if(head.type==2)
 {
  fread(uncompressed,uncompressed_length,1,target);
 }
 if(head.type==10)
 {
  compressed=this->create_buffer(compressed_length);
  fread(compressed,compressed_length,1,target);
  while(index<uncompressed_length)
  {
   if(compressed[position]<128)
   {
    amount=compressed[position]+1;
    amount*=3;
    memmove(uncompressed+index,compressed+(position+1),amount);
    index+=amount;
    position+=1+amount;
   }
   else
   {
    for(amount=compressed[position]-127;amount>0;--amount)
    {
     memmove(uncompressed+index,compressed+(position+1),3);
     index+=3;
    }
    position+=4;
   }

  }
  free(compressed);
 }
 fclose(target);
 data=uncompressed;
}

void IGF_Image::load_pcx(const char *name)
{
 FILE *target;
 unsigned long int x,y;
 size_t index,position,line,row,length,uncompressed_length;
 unsigned char repeat;
 unsigned char *original;
 unsigned char *uncompressed;
 PCX_head head;
 this->clear_buffer();
 target=this->open_image(name);
 length=(size_t)this->get_file_size(target)-128;
 fread(&head,128,1,target);
 if((head.color*head.planes!=24)&&(head.compress!=1))
 {
  IGF_Show_Error("Incorrect image format");
 }
 width=head.max_x-head.min_x+1;
 height=head.max_y-head.min_y+1;
 row=3*(size_t)width;
 line=(size_t)head.planes*(size_t)head.plane_length;
 uncompressed_length=row*height;
 index=0;
 position=0;
 original=this->create_buffer(length);
 uncompressed=this->create_buffer(uncompressed_length);
 fread(original,length,1,target);
 fclose(target);
 while (index<length)
 {
  if (original[index]<192)
  {
   uncompressed[position]=original[index];
   position++;
   index++;
  }
  else
  {
   for (repeat=original[index]-192;repeat>0;--repeat)
   {
    uncompressed[position]=original[index+1];
    position++;
   }
   index+=2;
  }

 }
 free(original);
 original=this->create_buffer(uncompressed_length);
 for(x=0;x<width;++x)
 {
  for(y=0;y<height;++y)
  {
   index=(size_t)x*3+(size_t)y*row;
   position=(size_t)x+(size_t)y*line;
   original[index]=uncompressed[position+2*(size_t)head.plane_length];
   original[index+1]=uncompressed[position+(size_t)head.plane_length];
   original[index+2]=uncompressed[position];
  }

 }
 free(uncompressed);
 data=original;
}

unsigned long int IGF_Image::get_width()
{
 return width;
}

unsigned long int IGF_Image::get_height()
{
 return height;
}

size_t IGF_Image::get_data_length()
{
 return (size_t)width*(size_t)height*3;
}

unsigned char *IGF_Image::get_data()
{
 return data;
}

void IGF_Image::destroy_image()
{
 width=0;
 height=0;
 this->clear_buffer();
}

IGF_Canvas::IGF_Canvas()
{
 image=NULL;
 surface=NULL;
 width=0;
 height=0;
 frames=1;
}

IGF_Canvas::~IGF_Canvas()
{
 surface=NULL;
 if(image!=NULL) free(image);
}

void IGF_Canvas::clear_buffer()
{
 if(image!=NULL) free(image);
}

void IGF_Canvas::set_width(const unsigned long int image_width)
{
 width=image_width;
}

void IGF_Canvas::set_height(const unsigned long int image_height)
{
 height=image_height;
}

IGF_Color *IGF_Canvas::create_buffer(const unsigned long int image_width,const unsigned long int image_height)
{
 IGF_Color *result;
 size_t length;
 length=(size_t)image_width*(size_t)image_height;
 result=(IGF_Color*)calloc(length,3);
 if(result==NULL)
 {
  IGF_Show_Error("Can't allocate memory for image buffer");
 }
 return result;
}

void IGF_Canvas::draw_image_pixel(const size_t offset,const unsigned long int x,const unsigned long int y)
{
 surface->draw_pixel(x,y,image[offset].red,image[offset].green,image[offset].blue);
}

size_t IGF_Canvas::get_offset(const unsigned long int start,const unsigned long int x,const unsigned long int y)
{
 return (size_t)start+(size_t)x+(size_t)y*(size_t)width;
}

IGF_Color *IGF_Canvas::get_image()
{
 return image;
}

size_t IGF_Canvas::get_length()
{
 return (size_t)width*(size_t)height;
}

unsigned long int IGF_Canvas::get_image_width()
{
 return width;
}

unsigned long int IGF_Canvas::get_image_height()
{
 return height;
}

void IGF_Canvas::set_frames(const unsigned long int amount)
{
 if(amount>1) frames=amount;
}

unsigned long int IGF_Canvas::get_frames()
{
 return frames;
}

void IGF_Canvas::initialize(IGF_Screen *Screen)
{
 surface=Screen;
}

void IGF_Canvas::load_image(IGF_Image &buffer)
{
 width=buffer.get_width();
 height=buffer.get_height();
 this->clear_buffer();
 image=this->create_buffer(width,height);
 memmove(image,buffer.get_data(),buffer.get_data_length());
 buffer.destroy_image();
}

void IGF_Canvas::mirror_image(const IGF_MIRROR_TYPE kind)
{
 unsigned long int x,y;
 size_t index,index2;
 IGF_Color *mirrored_image;
 mirrored_image=this->create_buffer(width,height);
 if (kind==IGF_MIRROR_HORIZONTAL)
 {
  for (x=0;x<width;++x)
  {
   for (y=0;y<height;++y)
   {
    index=this->get_offset(0,x,y);
    index2=this->get_offset(0,(width-x-1),y);
    mirrored_image[index]=image[index2];
   }

  }

 }
 if(kind==IGF_MIRROR_VERTICAL)
 {
   for (x=0;x<width;++x)
  {
   for (y=0;y<height;++y)
   {
    index=this->get_offset(0,x,y);
    index2=this->get_offset(0,x,(height-y-1));
    mirrored_image[index]=image[index2];
   }

  }

 }
 free(image);
 image=mirrored_image;
}

void IGF_Canvas::resize_image(const unsigned long int new_width,const unsigned long int new_height)
{
 float x_ratio,y_ratio;
 unsigned long int x,y;
 size_t index,index2;
 IGF_Color *scaled_image;
 scaled_image=this->create_buffer(new_width,new_height);
 x_ratio=(float)width/(float)new_width;
 y_ratio=(float)height/(float)new_height;
 for (x=0;x<new_width;++x)
 {
  for (y=0;y<new_height;++y)
  {
   index=(size_t)x+(size_t)y*(size_t)new_width;
   index2=(size_t)(x_ratio*(float)x)+(size_t)width*(size_t)(y_ratio*(float)y);
   scaled_image[index]=image[index2];
  }

 }
 free(image);
 image=scaled_image;
 width=new_width;
 height=new_height;
}

IGF_Background::IGF_Background()
{
 start=0;
 background_width=0;
 background_height=0;
 frame=1;
 current_kind=IGF_NORMAL_BACKGROUND;
}

IGF_Background::~IGF_Background()
{

}

void IGF_Background::set_kind(IGF_BACKGROUND_TYPE kind)
{
 switch(kind)
 {
  case IGF_NORMAL_BACKGROUND:
  background_width=this->get_image_width();
  background_height=this->get_image_height();
  start=0;
  break;
  case IGF_HORIZONTAL_BACKGROUND:
  background_width=this->get_image_width()/this->get_frames();
  background_height=this->get_image_height();
  start=(frame-1)*background_width;
  break;
  case IGF_VERTICAL_BACKGROUND:
  background_width=this->get_image_width();
  background_height=this->get_image_height()/this->get_frames();
  start=(frame-1)*background_width*background_height;
  break;
 }
 current_kind=kind;
}

void IGF_Background::set_target(const unsigned long int target)
{
 if((target>0)&&(target<=this->get_frames()))
 {
  frame=target;
  this->set_kind(current_kind);
 }

}

void IGF_Background::draw_background()
{
 unsigned long int x,y;
 size_t offset;
 for(x=0;x<background_width;++x)
 {
  for(y=0;y<background_height;++y)
  {
   offset=this->get_offset(start,x,y);
   this->draw_image_pixel(offset,x,y);
  }

 }

}

IGF_Sprite::IGF_Sprite()
{
 current_x=0;
 current_y=0;
 sprite_width=0;
 sprite_height=0;
 frame=0;
 start=0;
 current_kind=IGF_SINGE_SPRITE;
}

IGF_Sprite::~IGF_Sprite()
{

}

bool IGF_Sprite::compare_pixels(const IGF_Color &first,const IGF_Color &second)
{
 bool result;
 result=false;
 if ((first.red!=second.red)||(first.green!=second.green))
 {
  result=true;
 }
 else
 {
  if(first.blue!=second.blue) result=true;
 }
 return result;
}

void IGF_Sprite::draw_sprite_pixel(const size_t offset,const unsigned long int x,const unsigned long int y)
{
 if(this->compare_pixels(image[0],image[offset])==true) this->draw_image_pixel(offset,x,y);
}

unsigned long int IGF_Sprite::get_x()
{
 return current_x;
}

unsigned long int IGF_Sprite::get_y()
{
 return current_y;
}

unsigned long int IGF_Sprite::get_width()
{
 return sprite_width;
}

unsigned long int IGF_Sprite::get_height()
{
 return sprite_height;
}

IGF_Sprite* IGF_Sprite::get_handle()
{
 return this;
}

IGF_Box IGF_Sprite::get_box()
{
 IGF_Box target;
 target.x=current_x;
 target.y=current_y;
 target.width=sprite_width;
 target.height=sprite_height;
 return target;
}

void IGF_Sprite::set_kind(const IGF_SPRITE_TYPE kind)
{
 switch(kind)
 {
  case IGF_SINGE_SPRITE:
  sprite_width=this->get_image_width();
  sprite_height=this->get_image_height();
  start=0;
  break;
  case IGF_HORIZONTAL_STRIP:
  sprite_width=this->get_image_width()/this->get_frames();
  sprite_height=this->get_image_height();
  start=(frame-1)*sprite_width;
  break;
  case IGF_VERTICAL_STRIP:
  sprite_width=this->get_image_width();
  sprite_height=this->get_image_height()/this->get_frames();
  start=(frame-1)*sprite_width*sprite_height;
  break;
 }
 current_kind=kind;
}

IGF_SPRITE_TYPE IGF_Sprite::get_kind()
{
 return current_kind;
}

void IGF_Sprite::set_target(const unsigned long int target)
{
 if((target>0)&&(target<=this->get_frames()))
 {
  frame=target;
  this->set_kind(current_kind);
 }

}

void IGF_Sprite::set_position(const unsigned long int x,const unsigned long int y)
{
 current_x=x;
 current_y=y;
}

void IGF_Sprite::clone(IGF_Sprite &target)
{
 this->set_width(target.get_image_width());
 this->set_height(target.get_image_height());
 this->set_frames(target.get_frames());
 this->set_kind(target.get_kind());
 image=this->create_buffer(target.get_image_width(),target.get_image_width());
 memmove(image,target.get_image(),target.get_length());
}

void IGF_Sprite::draw_sprite()
{
 size_t offset;
 unsigned long int sprite_x,sprite_y;
 for(sprite_x=0;sprite_x<sprite_width;++sprite_x)
 {
  for(sprite_y=0;sprite_y<sprite_height;++sprite_y)
  {
   offset=this->get_offset(start,sprite_x,sprite_y);
   this->draw_sprite_pixel(offset,current_x+sprite_x,current_y+sprite_y);
  }

 }

}

IGF_Text::IGF_Text()
{
 current_x=0;
 current_y=0;
 step_x=0;
 sprite=NULL;
}

IGF_Text::~IGF_Text()
{
 sprite=NULL;
}

void IGF_Text::draw_character(const char target)
{
 if((target>31)||(target<0))
 {
  sprite->set_target((unsigned long int)target+1);
  sprite->set_position(step_x,current_y);
  sprite->draw_sprite();
  step_x+=sprite->get_width();
 }

}

void IGF_Text::set_position(const unsigned long int x,const unsigned long int y)
{
 current_x=x;
 current_y=y;
}

void IGF_Text::load_font(IGF_Sprite *font)
{
 sprite=font;
 sprite->set_frames(256);
 sprite->set_kind(IGF_HORIZONTAL_STRIP);
}

void IGF_Text::draw_text(const char *text)
{
 size_t index,length;
 length=strlen(text);
 step_x=current_x;
 for (index=0;index<length;++index)
 {
  this->draw_character(text[index]);
 }

}

bool IGF_Collision::check_horizontal_collision(const IGF_Box &first,const IGF_Box &second)
{
 bool result;
 result=false;
 if((first.x+first.width)>=second.x)
 {
  if(first.x<=(second.x+second.width)) result=true;
 }
 return result;
}

bool IGF_Collision::check_vertical_collision(const IGF_Box &first,const IGF_Box &second)
{
 bool result;
 result=false;
 if((first.y+first.height)>=second.y)
 {
  if(first.y<=(second.y+second.height)) result=true;
 }
 return result;
}

bool IGF_Collision::check_collision(const IGF_Box &first,const IGF_Box &second)
{
 return this->check_horizontal_collision(first,second) || this->check_vertical_collision(first,second);
}