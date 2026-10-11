#include "Dsp.h"
#include <iostream>
#include <stdexcept>
void require(bool ok,const char* msg){if(!ok)throw std::runtime_error(msg);std::cout<<"PASS "<<msg<<'\n';}
double energy(double sr,float amplitude,float compress,float drive,int profile=1){
 mv3::VocalFocus p;std::array<float,8> values{30,0,compress,drive,1.2f,2.8f,0,100};p.prepare(sr,values,profile);double sum=0;const int samples=int(sr*1.2);
 for(int n=0;n<samples;++n){float t=float(n/sr);float l=amplitude*(std::sin(6.283185307f*180*t)+.25f*std::sin(6.283185307f*540*t)),r=l;p.process(l,r);if(!std::isfinite(l)||std::abs(l)>4)throw std::runtime_error("Invalid focus output");if(n>samples/2)sum+=double(l)*l;}
 return std::sqrt(sum/double(samples/2));
}
int main(){try{
 const double lo=energy(48000,.04f,70,0),hi=energy(48000,.16f,70,0);const double jump=20*std::log10(hi/lo);std::cout<<"FOCUS LEVEL JUMP "<<jump<<" dB\n";require(jump>1 && jump<10,"Focus controls 12 dB level jump without flattening it");
 const double freeLo=energy(48000,.04f,0,0),freeHi=energy(48000,.16f,0,0);require(20*std::log10(freeHi/freeLo)>11,"Compression zero retains source-level differences");
 require(std::abs(20*std::log10(energy(48000,.1f,0,.001f)/energy(48000,.1f,0,0)))<.01,"Colour is continuous through drive zero");
 for(double sr:{44100.,48000.,96000.,192000.})require(std::abs(20*std::log10(energy(sr,.1f,65,25)/energy(48000,.1f,65,25)))<.3,"Focus sample-rate consistency within 0.3 dB");
 mv3::Engine e;std::array<float,8> v{30,0,60,24,1.2f,2.8f,0,100};e.setVocalProfile(1);e.setFocusEnabled(true);e.prepare(1,192000,1024,v);
 float peak=0;for(int n=0;n<192000;++n){if(n==48000){e.setVocalProfile(2);v[4]=6;v[5]=6;e.configure(v);}if(n==96000){e.setFocusEnabled(false);e.configure(v);}if(n==144000){e.setFocusEnabled(true);e.configure(v);}float l=.15f*std::sin(6.283185307f*225*n/192000),r=l;e.process(l,r,120);if(!std::isfinite(l)||!std::isfinite(r))throw std::runtime_error("Mode transition invalid");peak=std::max(peak,std::abs(l-r));}
 require(peak<1.e-6,"Profile and mode automation preserve centred mono voice");
 mv3::VocalFocus silent;silent.prepare(48000,v,2);for(int n=0;n<48000;++n){float l=0,r=0;silent.process(l,r);if(l!=0||r!=0)throw std::runtime_error("Focus generates sound on silence");}require(true,"Focus generates no signal on digital silence");
 std::cout<<"ALL VOCAL CHECKS PASSED\n";return 0;
}catch(const std::exception& e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}}
