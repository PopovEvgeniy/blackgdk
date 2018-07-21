// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright © 2017-2018, Popov Evgeniy Alekseyevich

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
  memset(IGF_Keys,IGFKEY_NONE,IGF_KEYBOARD);
  memset(IGF_Buttons,IGFKEY_NONE,IGF_MOUSE);
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
  IGF_Keys[(lParam >> 16)&0x7f]=IGFKEY_PRESS;
  break;
  case WM_KEYUP:
  IGF_Keys[(lParam >> 16)&0x7f]=IGFKEY_RELEASE;
  break;
 }
 return DefWindowProc(window,Message,wParam,lParam);
}

IGF_Base::IGF_Base()
{
 HRESULT status;
 status=CoInitialize(NULL);
 if(status!=S_OK)
 {
  if(status!=S_FALSE)
  {
   puts("Can't initialize COM");
   exit(EXIT_FAILURE);
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
  puts("Can't create synchronization timer");
  exit(EXIT_FAILURE);
 }

}

void IGF_Synchronization::set_timer(const unsigned long int interval)
{
 LARGE_INTEGER start;
 start.QuadPart=0;
 if(SetWaitableTimer(timer,&start,interval,NULL,NULL,FALSE)==FALSE)
 {
  puts("Can't set timer");
  exit(EXIT_FAILURE);
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

void IGF_Engine::prepare_engine()
{
 window_class.hInstance=GetModuleHandle(NULL);
 if(window_class.hInstance==NULL)
 {
  puts("Can't get the application instance");
  exit(EXIT_FAILURE);
 }
 window_class.hIcon=LoadIcon(NULL,IDI_APPLICATION);
 if (window_class.hIcon==NULL)
 {
  puts("Can't load the standart program icon");
  exit(EXIT_FAILURE);
 }
 window_class.hCursor=LoadCursor(NULL,IDC_ARROW);
 if (window_class.hCursor==NULL)
 {
  puts("Can't load the standart cursor");
  exit(EXIT_FAILURE);
 }
 if (RegisterClass(&window_class)==0)
 {
  puts("Can't register window class");
  exit(EXIT_FAILURE);
 }
 width=GetSystemMetrics(SM_CXSCREEN);
 height=GetSystemMetrics(SM_CYSCREEN);
}

void IGF_Engine::create_window()
{
 window=CreateWindow(window_class.lpszClassName,NULL,WS_VISIBLE|WS_POPUP,0,0,width,height,NULL,NULL,window_class.hInstance,NULL);
 if (window==NULL)
 {
  puts("Can't create window");
  exit(EXIT_FAILURE);
 }
 EnableWindow(window,TRUE);
 SetFocus(window);
}

void IGF_Engine::capture_mouse()
{
 RECT border;
 if(GetClientRect(window,&border)==FALSE)
 {
  puts("Can't capture window");
  exit(EXIT_FAILURE);
 }
 if(ClipCursor(&border)==FALSE)
 {
  puts("Can't capture cursor");
  exit(EXIT_FAILURE);
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
 frame_line=frame_width*4;
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

void IGF_Frame::create_render_buffer()
{
 buffer_length=(size_t)frame_width*(size_t)frame_height;
 buffer=(unsigned long int*)calloc(buffer_length,sizeof(unsigned long int));
 if(buffer==NULL)
 {
  puts("Can't allocate memory for render buffer");
  exit(EXIT_FAILURE);
 }
 else
 {
  buffer_length*=sizeof(unsigned long int);
 }

}

unsigned long int IGF_Frame::get_rgb(const unsigned long int red,const unsigned long int green,const unsigned long int blue)
{
 return red+(green<<8)+(blue<<16);
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
  puts("Can't create render");
  exit(EXIT_FAILURE);
 }

}

void IGF_Render::create_target()
{
 if(render->CreateHwndRenderTarget(setting,configuration,&target)!=S_OK)
 {
  puts("Can't create render target");
  exit(EXIT_FAILURE);
 }
 target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
}

void IGF_Render::create_surface()
{
 if(target->CreateBitmap(D2D1::SizeU(frame_width,frame_height),buffer,frame_line,D2D1::BitmapProperties(setting.pixelFormat,96.0,96.0),&surface)!=S_OK)
 {
  puts("Can't create render surface");
  exit(EXIT_FAILURE);
 }

}

void IGF_Render::set_render_setting()
{
 configuration.hwnd=window;
 configuration.pixelSize=D2D1::SizeU(width,height);
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
 source=D2D1::RectU(0,0,frame_width,frame_height);
 destanation=D2D1::RectF(0,0,width,height);
 texture=D2D1::RectF(0,0,frame_width,frame_height);
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
 surface->CopyFromMemory(&source,buffer,frame_line);
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
 preversion=(unsigned char*)calloc(IGF_KEYBOARD,1);
 if(preversion==NULL)
 {
  puts("Can't allocate memory for keyboard state buffer");
  exit(EXIT_FAILURE);
 }

}

bool IGF_Keyboard::check_hold(const unsigned char code)
{
 bool result;
 result=false;
 if(IGF_Keys[code]==IGFKEY_PRESS) result=true;
 return result;
}

bool IGF_Keyboard::check_press(const unsigned char code)
{
 bool result;
 result=false;
 if(IGF_Keys[code]==IGFKEY_PRESS)
 {
  if(preversion[code]!=IGFKEY_PRESS) result=true;
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
  result=true;
  IGF_Keys[code]=IGFKEY_NONE;
 }
 return result;
}

IGF_Mouse::IGF_Mouse()
{
 memset(preversion,IGFKEY_NONE,IGF_MOUSE);
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
  puts("Can't set the mouse cursor position");
  exit(EXIT_FAILURE);
 }

}

unsigned long int IGF_Mouse::get_x()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  puts("Can't get the mouse cursor position");
  exit(EXIT_FAILURE);
 }
 return position.x;
}

unsigned long int IGF_Mouse::get_y()
{
 POINT position;
 if(GetCursorPos(&position)==FALSE)
 {
  puts("Can't get the mouse cursor position");
  exit(EXIT_FAILURE);
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
   if(preversion[button]!=IGFKEY_PRESS) result=true;
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
   result=true;
   IGF_Buttons[button]=IGFKEY_NONE;
  }

 }
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
 if(active<IGF_GAMEPAD_AMOUNT)
 {
  this->clear_state();
  active=gamepad;
 }

}

unsigned long int IGF_Gamepad::get_active()
{
 return active;
}

unsigned long int IGF_Gamepad::get_amount()
{
 unsigned long int old,result;
 result=0;
 old=active;
 for(active=0;active<IGF_GAMEPAD_AMOUNT;++active)
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
  if(battery.BatteryType!=BATTERY_TYPE_DISCONNECTED)
  {
   if(battery.BatteryType!=BATTERY_TYPE_WIRED) result=true;
  }

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
  switch (battery.BatteryType)
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

char IGF_Gamepad::get_stick_x(const IGF_GAMEPAD_STICKS stick)
{
 char result;
 short int control;
 result=0;
 if(stick==IGF_GAMEPAD_LEFT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbLX>=control) result=1;
  if(current.Gamepad.sThumbLX<=-1*control) result=-1;
 }
 if(stick==IGF_GAMEPAD_RIGHT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbRX>=control) result=1;
  if(current.Gamepad.sThumbRX<=-1*control) result=-1;
 }
 return result;
}

char IGF_Gamepad::get_stick_y(const IGF_GAMEPAD_STICKS stick)
{
 char result;
 short int control;
 result=0;
 if(stick==IGF_GAMEPAD_LEFT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbLY>=control) result=1;
  if(current.Gamepad.sThumbLY<=-1*control) result=-1;
 }
 if(stick==IGF_GAMEPAD_RIGHT_STICK)
 {
  control=32767-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
  if(current.Gamepad.sThumbRY>=control) result=1;
  if(current.Gamepad.sThumbRY<=-1*control) result=-1;
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
  puts("Can't allocate memory");
  exit(EXIT_FAILURE);
 }
 for(index=0;index<length;++index) name[index]=btowc(target[index]);
 return name;
}

void IGF_Multimedia::open(const wchar_t *target)
{
 player->StopWhenReady();
 if(loader->RenderFile(target,NULL)!=S_OK)
 {
  puts("Can't load a multimedia file");
  exit(EXIT_FAILURE);
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
  puts("Can't get the current and the end position");
  exit(EXIT_FAILURE);
 }
 return result;
}

void IGF_Multimedia::rewind()
{
 long long position;
 position=0;
 if(controler->SetPositions(&position,AM_SEEKING_AbsolutePositioning,NULL,AM_SEEKING_NoPositioning)!=S_OK)
 {
  puts("Can't set start position");
  exit(EXIT_FAILURE);
 }

}

void IGF_Multimedia::initialize()
{
 if(CoCreateInstance(CLSID_FilterGraph,NULL,CLSCTX_INPROC_SERVER,IID_IGraphBuilder,(void**)&loader)!=S_OK)
 {
  puts("Can't create a multimedia loader");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IMediaControl,(void**)&player)!=S_OK)
 {
  puts("Can't create a multimedia player");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IMediaSeeking,(void**)&controler)!=S_OK)
 {
  puts("Can't create a player controler");
  exit(EXIT_FAILURE);
 }
 if(loader->QueryInterface(IID_IVideoWindow,(void**)&video)!=S_OK)
 {
  puts("Can't create a video player");
  exit(EXIT_FAILURE);
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
  puts("Can't get the multimedia state");
  exit(EXIT_FAILURE);
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
  puts("Can't get the memory status");
  exit(EXIT_FAILURE);
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
  puts("Can't create log file");
  exit(EXIT_FAILURE);
 }

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
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
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
  puts("Can't open a image file");
  exit(EXIT_FAILURE);
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
  puts("Invalid image format");
  exit(EXIT_FAILURE);
 }
 if(head.type!=2)
 {
  if(head.type!=10)
  {
   puts("Invalid image format");
   exit(EXIT_FAILURE);
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
  puts("Incorrect image format");
  exit(EXIT_FAILURE);
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

IGF_Color *IGF_Canvas::create_buffer(const unsigned long int image_width,const unsigned long int image_height)
{
 IGF_Color *result;
 size_t length;
 length=(size_t)image_width*(size_t)image_height;
 result=(IGF_Color*)calloc(length,3);
 if(result==NULL)
 {
  puts("Can't allocate memory for image buffer");
  exit(EXIT_FAILURE);
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

unsigned long int IGF_Canvas::get_width()
{
 return width;
}

unsigned long int IGF_Canvas::get_height()
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
 if (kind==IGF_MIRROR_VERTICAL)
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

void IGF_Background::draw_background_image(const unsigned long int start,const unsigned long int frame_width,const unsigned long int frame_height)
{
 unsigned long int x,y;
 size_t offset;
 for(x=0;x<frame_width;++x)
 {
  for(y=0;y<frame_height;++y)
  {
   offset=this->get_offset(start,x,y);
   this->draw_image_pixel(offset,x,y);
  }

 }

}

void IGF_Background::draw_horizontal_background(const unsigned long int frame)
{
 unsigned long int start,frame_width;
 frame_width=width/frames;
 start=(frame-1)*frame_width;
 this->draw_background_image(start,frame_width,height);
}

void IGF_Background::draw_vertical_background(const unsigned long int frame)
{
 unsigned long int start,frame_height;
 frame_height=height/frames;
 start=(frame-1)*frame_height*width;
 this->draw_background_image(start,width,frame_height);
}

void IGF_Background::draw_background()
{
 this->draw_horizontal_background(1);
}

IGF_Sprite::IGF_Sprite()
{
 current_x=0;
 current_y=0;
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

void IGF_Sprite::clone(IGF_Sprite &target)
{
 size_t length;
 frames=target.get_frames();
 width=target.get_sprite_width();
 height=target.get_sprite_height();
 length=(size_t)width*(size_t)height*3;
 image=this->create_buffer(width,height);
 memmove(image,target.get_image(),length);
}

void IGF_Sprite::draw_sprite_frame(const unsigned long int x,const unsigned long int y,const unsigned long int frame)
{
 unsigned long int sprite_x,sprite_y,start,frame_width;
 size_t offset;
 current_x=x;
 current_y=y;
 frame_width=width/frames;
 start=(frame-1)*frame_width;
 for(sprite_x=0;sprite_x<frame_width;++sprite_x)
 {
  for(sprite_y=0;sprite_y<height;++sprite_y)
  {
   offset=this->get_offset(start,sprite_x,sprite_y);
   this->draw_sprite_pixel(offset,x+sprite_x,y+sprite_y);
  }

 }

}

void IGF_Sprite::draw_sprite(const unsigned long int x,const unsigned long int y)
{
 current_x=x;
 current_y=y;
 this->draw_sprite_frame(x,y,1);
}

unsigned long int IGF_Sprite::get_x()
{
 return current_x;
}

unsigned long int IGF_Sprite::get_y()
{
 return current_y;
}

unsigned long int IGF_Sprite::get_sprite_width()
{
 return width/frames;
}

unsigned long int IGF_Sprite::get_sprite_height()
{
 return height;
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
 target.width=width/frames;
 target.height=height;
 return target;
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
  sprite->draw_sprite_frame(step_x,current_y,(unsigned long int)target+1);
  step_x+=sprite->get_sprite_width();
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