#pragma once
#include "LegacyVstAbi.h"
#include <windows.h>
#include <thread>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <iostream>
namespace native_checks {
inline void pump(legacy::Effect* effect,int ms){const auto until=GetTickCount64()+ms;do{MSG msg;while(PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessageW(&msg);}effect->dispatcher(effect,19,0,0,nullptr,0);Sleep(5);}while(GetTickCount64()<until);}
inline void capture(HWND child,const std::string& path){
 RECT r{};GetClientRect(child,&r);const int width=r.right,height=r.bottom;if(width<100||height<100)throw std::runtime_error("Native editor too small");
 BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=width;info.bmiHeader.biHeight=-height;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
 auto screen=GetDC(child);auto dc=CreateCompatibleDC(screen);void* pixels=nullptr;auto bitmap=CreateDIBSection(screen,&info,DIB_RGB_COLORS,&pixels,nullptr,0);auto previous=SelectObject(dc,bitmap);
 PatBlt(dc,0,0,width,height,BLACKNESS);
 // Native WM_PRINTCLIENT invokes the actual peer's GDI painting path. No
 // JUCE off-screen component snapshot substitutes for the host attachment.
 SendMessageW(child,WM_PRINTCLIENT,reinterpret_cast<WPARAM>(dc),PRF_CLIENT|PRF_CHILDREN);
 const auto* colours=static_cast<const unsigned int*>(pixels);std::set<unsigned int> distinct;
 for(int y=0;y<height;y+=5)for(int x=0;x<width;x+=5)distinct.insert(colours[y*width+x]&0xffffff);
 const auto number=distinct.size();
 if(!path.empty()){
  BITMAPFILEHEADER file{};file.bfType=0x4d42;file.bfOffBits=sizeof(file)+sizeof(BITMAPINFOHEADER);file.bfSize=file.bfOffBits+width*height*4;
  std::ofstream out(path,std::ios::binary);out.write(reinterpret_cast<const char*>(&file),sizeof(file));out.write(reinterpret_cast<const char*>(&info.bmiHeader),sizeof(BITMAPINFOHEADER));out.write(static_cast<const char*>(pixels),width*height*4);
 }
 SelectObject(dc,previous);DeleteObject(bitmap);DeleteDC(dc);ReleaseDC(child,screen);
 if(number<32)throw std::runtime_error("Native editor is blank or unpainted");
 std::cout<<"PASS native HWND paints "<<number<<" distinct colours\n";
}
inline void editor(legacy::Effect* e,const std::string& prefix){
 using SetContext=HANDLE(WINAPI*)(HANDLE);auto set=reinterpret_cast<SetContext>(GetProcAddress(GetModuleHandleW(L"user32.dll"),"SetThreadDpiAwarenessContext"));
 for(int mode=0;mode<3;++mode){
  HANDLE previous=nullptr;if(set)previous=set(reinterpret_cast<HANDLE>(static_cast<intptr_t>(mode==0?-1:mode==1?-2:-4)));
  legacy::Rect* rect=nullptr;if(!e->dispatcher(e,13,0,0,&rect,0)||!rect||rect->right<500||rect->bottom<300)throw std::runtime_error("Invalid editor rectangle");
  const int width=rect->right,height=rect->bottom;RECT frame{0,0,width,height};AdjustWindowRect(&frame,WS_OVERLAPPEDWINDOW,FALSE);
  auto parent=CreateWindowExW(0,L"STATIC",L"MettaVoxAD native host test",WS_OVERLAPPEDWINDOW,40,40,frame.right-frame.left,frame.bottom-frame.top,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);
  if(!parent)throw std::runtime_error("Host HWND creation failed");ShowWindow(parent,SW_SHOWNOACTIVATE);
  if(e->dispatcher(e,14,0,0,reinterpret_cast<void*>(1),0))throw std::runtime_error("Invalid HWND accepted");
  for(int reopen=0;reopen<2;++reopen){
   if(!e->dispatcher(e,14,0,0,parent,0))throw std::runtime_error("Editor attach failed");pump(e,160);
   auto child=GetWindow(parent,GW_CHILD);if(!child||GetParent(child)!=parent||!(GetWindowLongPtrW(child,GWL_STYLE)&WS_CHILD))throw std::runtime_error("Missing native child attachment");
   capture(child,prefix.empty()?"":prefix+"_dpi"+std::to_string(mode)+"_open"+std::to_string(reopen)+".bmp");
   e->setParameter(e,0,reopen?.8f:.2f);pump(e,80);capture(child,"");
   e->dispatcher(e,15,0,0,nullptr,0);pump(e,15);if(GetWindow(parent,GW_CHILD))throw std::runtime_error("Editor child leaked after close");
  }
  DestroyWindow(parent);if(set&&previous)set(previous);
 }
 std::cout<<"PASS editor attaches, repaints, changes parameters, closes/reopens across three DPI contexts\n";
}
}
