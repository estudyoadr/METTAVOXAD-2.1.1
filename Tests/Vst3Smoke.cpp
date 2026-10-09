#include <juce_audio_utils/juce_audio_utils.h>
#include "NativeEditorChecks.h"
#include <iostream>
#include <cmath>
int main(int argc,char** argv){try{
 juce::ScopedJuceInitialiser_GUI gui;
 if(argc<2)throw std::runtime_error("VST3 path required");
 juce::VST3PluginFormat format;
 for(int arg=1;arg<argc;++arg){
  juce::OwnedArray<juce::PluginDescription> descriptions;format.findAllTypesForFile(descriptions,argv[arg]);
  if(descriptions.size()!=1)throw std::runtime_error("VST3 scanner did not find exactly one effect");
  juce::String error;auto plugin=format.createInstanceFromDescription(*descriptions[0],48000,256,error);
  if(!plugin)throw std::runtime_error(error.toStdString());
  plugin->prepareToPlay(48000,256);juce::AudioBuffer<float> b(2,256);juce::MidiBuffer midi;
  double energy=0;for(int block=0;block<24;++block){for(int ch=0;ch<2;++ch)for(int j=0;j<256;++j)b.setSample(ch,j,.12f*std::sin(float((block*256+j)*.03)));plugin->processBlock(b,midi);for(int j=0;j<256;++j){auto x=b.getSample(0,j);if(!std::isfinite(x))throw std::runtime_error("VST3 audio invalid");energy+=double(x)*x;}}
  if(energy<=.001)throw std::runtime_error("VST3 audio silent");
  RECT frame{0,0,1000,700};AdjustWindowRect(&frame,WS_OVERLAPPEDWINDOW,FALSE);
  auto parent=CreateWindowExW(0,L"STATIC",L"MettaVoxAD VST3 native host",WS_OVERLAPPEDWINDOW,20,20,frame.right-frame.left,frame.bottom-frame.top,nullptr,nullptr,GetModuleHandleW(nullptr),nullptr);ShowWindow(parent,SW_SHOWNOACTIVATE);
  for(int pass=0;pass<2;++pass){
   std::unique_ptr<juce::AudioProcessorEditor> editor(plugin->createEditorIfNeeded());if(!editor)throw std::runtime_error("VST3 editor missing");
   editor->addToDesktop(0,parent);editor->setVisible(true);
   auto until=GetTickCount64()+200;while(GetTickCount64()<until){MSG msg;while(PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)){TranslateMessage(&msg);DispatchMessageW(&msg);}Sleep(5);}
   auto child=GetWindow(parent,GW_CHILD);if(!child)throw std::runtime_error("VST3 editor not attached");
   // The JUCE host wraps the plugin's HWND in one extra native child.
   while(auto nested=GetWindow(child,GW_CHILD)){RECT r{};GetClientRect(nested,&r);if(r.right<500||r.bottom<300)break;child=nested;}
   native_checks::capture(child,"");
   editor.reset();
  }
  DestroyWindow(parent);plugin->releaseResources();
  std::cout<<"PASS VST3 scanner, factory, audio and native editor reopen: "<<descriptions[0]->name<<"\n";
 }
 return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<"\n";return 1;}}
