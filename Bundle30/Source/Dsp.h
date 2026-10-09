#pragma once
#include <juce_dsp/juce_dsp.h>
#include "LegacyEngines.h"
#include <array>
#include <vector>
#include <cmath>
namespace mv3 {
inline float gain(float db) {return std::pow(10.f,db*.05f);}
inline float db(float x) {return 20.f*std::log10(juce::jmax(1.e-9f,x));}
inline float finite(float x) {return std::isfinite(x)?juce::jlimit(-16.f,16.f,x):0.f;}
struct Filter {
 std::array<double,5> a{1,0,0,0,0}; std::array<std::array<double,2>,2> z{};
 void reset(){z={};}
 void set(const std::array<float,6>& c){a={c[0]/c[3],c[1]/c[3],c[2]/c[3],c[4]/c[3],c[5]/c[3]};}
 void hp(double sr,float hz){set(juce::dsp::IIR::ArrayCoefficients<float>::makeHighPass(sr,juce::jlimit(10.f,float(sr*.44),hz)));}
 void lp(double sr,float hz){set(juce::dsp::IIR::ArrayCoefficients<float>::makeLowPass(sr,juce::jlimit(10.f,float(sr*.44),hz)));}
 void bp(double sr,float hz,float q){set(juce::dsp::IIR::ArrayCoefficients<float>::makeBandPass(sr,juce::jlimit(10.f,float(sr*.44),hz),q));}
 void peak(double sr,float hz,float q,float amount){set(juce::dsp::IIR::ArrayCoefficients<float>::makePeakFilter(sr,juce::jlimit(10.f,float(sr*.44),hz),q,gain(amount)));}
 void shelf(double sr,float hz,float amount){set(juce::dsp::IIR::ArrayCoefficients<float>::makeHighShelf(sr,juce::jlimit(10.f,float(sr*.44),hz),.707f,gain(amount)));}
 float tick(int c,float x){auto& s=z[size_t(c)];const double y=a[0]*x+s[0];s[0]=a[1]*x-a[3]*y+s[1];s[1]=a[2]*x-a[4]*y;return finite(float(y));}
};
struct Envelope {
 float v=0,at=.1f,re=.001f;
 void setup(double sr,float attackMs,float releaseMs){at=float(1-std::exp(-1/(sr*attackMs*.001)));re=float(1-std::exp(-1/(sr*releaseMs*.001)));}
 float tick(float x){x=std::abs(x);v+=(x-v)*(x>v?at:re);return v;}
};
struct Delay {
 std::array<std::vector<float>,2> line; int pos=0,size=0;
 void prepare(double sr,double seconds){size=int(sr*seconds)+8;for(auto& l:line)l.assign(size_t(size),0);pos=0;}
 float read(int c,float distance)const {distance=juce::jlimit(1.f,float(size-2),distance);float p=float(pos)-distance;while(p<0)p+=float(size);const int k=int(p);float f=p-k;return line[size_t(c)][size_t(k)]*(1-f)+line[size_t(c)][size_t((k+1)%size)]*f;}
 void push(float l,float r){line[0][size_t(pos)]=finite(l);line[1][size_t(pos)]=finite(r);if(++pos>=size)pos=0;}
};
// 512-point WOLA spectral attenuation. Periodic Hann, 75% overlap, exact
// squared-window normalisation. Storage is allocated only in prepare.
class SpectralClean {
 static constexpr int N=512,H=128; juce::dsp::FFT fft{9};
 std::array<std::array<float,N>,2> input{},output{};
 std::array<float,2*N> work{};std::array<float,N> window{};
 std::array<std::array<float,N/2+1>,2> smooth{};
 int pos=0,count=0;
public:
 void reset(){input={};output={};pos=count=0;for(auto& x:smooth)x.fill(1);for(int n=0;n<N;++n)window[size_t(n)]=.5f-.5f*std::cos(juce::MathConstants<float>::twoPi*n/N);}
 void tick(float& l,float& r,float reduction,float floorDb){
  const float src[2]{l,r};float result[2]{};
  for(int c=0;c<2;++c){input[size_t(c)][size_t(pos)]=src[c];result[c]=output[size_t(c)][size_t(pos)];output[size_t(c)][size_t(pos)]=0;}
  pos=(pos+1)%N;
  if(++count%H==0){for(int c=0;c<2;++c){work.fill(0);for(int n=0;n<N;++n)work[size_t(n)]=input[size_t(c)][size_t((pos+n)%N)]*window[size_t(n)];fft.performRealOnlyForwardTransform(work.data());
   for(int k=0;k<=N/2;++k){float re=work[size_t(2*k)],im=work[size_t(2*k+1)];float magnitude=std::sqrt(re*re+im*im);float threshold=gain(floorDb)*N*.24f;float desired=juce::jlimit(gain(-reduction),1.f,1.f-threshold*threshold/(magnitude*magnitude+1.e-12f));auto& s=smooth[size_t(c)][size_t(k)];s+=(desired-s)*.35f;work[size_t(2*k)]*=s;work[size_t(2*k+1)]*=s;}
   fft.performRealOnlyInverseTransform(work.data());for(int n=0;n<N;++n)output[size_t(c)][size_t((pos+n)%N)]+=work[size_t(n)]*window[size_t(n)]*(2.f/3.f);
  }}l=result[0];r=result[1];
 }
};
// K-weighted momentary/short-term and gated integrated loudness. Up to one
// hour of 100 ms updates retained; 400 ms blocks and two-pass energy gating.
class Loudness {
 Filter pre,rlb;std::vector<double> squares,blocks;
 size_t at=0,filled=0,record=0,records=0;int hop=1,ticks=0,window400=1;double sum=0;
public:
 float momentary=-100,shortTerm=-100,integrated=-100;
 void prepare(double sr){
  squares.assign(size_t(std::ceil(sr*3)),0);blocks.assign(36000,0);at=filled=record=records=0;ticks=0;sum=0;hop=int(sr*.1);window400=int(sr*.4);momentary=shortTerm=integrated=-100;
  double K=std::tan(juce::MathConstants<double>::pi*1681.974450955533/sr),Q=.7071752369554196,Vh=std::pow(10.,3.999843853973347/20.),Vb=std::pow(Vh,.4996667741545416),a0=1+K/Q+K*K;
  pre.a={(Vh+Vb*K/Q+K*K)/a0,2*(K*K-Vh)/a0,(Vh-Vb*K/Q+K*K)/a0,2*(K*K-1)/a0,(1-K/Q+K*K)/a0};pre.reset();
  K=std::tan(juce::MathConstants<double>::pi*38.13547087602444/sr);Q=.5003270373238773;a0=1+K/Q+K*K;
  rlb.a={1,-2,1,2*(K*K-1)/a0,(1-K/Q+K*K)/a0};rlb.reset();
 }
 void tick(float l,float r){double e=0;for(int c=0;c<2;++c){float y=rlb.tick(c,pre.tick(c,c?r:l));e+=double(y)*y;}sum+=e-squares[at];squares[at]=e;at=(at+1)%squares.size();filled=juce::jmin(squares.size(),filled+1);
  if(++ticks<hop)return;ticks=0;if(filled<size_t(window400))return;
  double recent=0;for(int n=0;n<window400;++n)recent+=squares[(at+squares.size()-1-size_t(n))%squares.size()];recent/=window400;
  momentary=level(recent);shortTerm=level(sum/double(juce::jmax(size_t(1),filled)));
  blocks[record]=recent;record=(record+1)%blocks.size();records=juce::jmin(blocks.size(),records+1);
  const double absolute=std::pow(10.,(-70+.691)/10.);double acc=0;size_t valid=0;for(size_t n=0;n<records;++n)if(blocks[n]>=absolute){acc+=blocks[n];++valid;}
  if(!valid){integrated=-100;return;}double relative=acc/double(valid)*.1;acc=0;valid=0;for(size_t n=0;n<records;++n)if(blocks[n]>=juce::jmax(absolute,relative)){acc+=blocks[n];++valid;}integrated=valid?level(acc/double(valid)):-100;
 }
private:static float level(double x){return float(-.691+10*std::log10(juce::jmax(1.e-12,x)));}
};
struct MinimumWindow {
 std::vector<float> values;std::vector<int64_t> times;int head=0,tail=0,length=1,size=1;int64_t clock=0;
 void prepare(int n){length=n;size=n+3;values.assign(size_t(size),1);times.assign(size_t(size),0);head=tail=0;clock=0;}
 float push(float v){while(head!=tail && values[size_t((tail+size-1)%size)]>=v)tail=(tail+size-1)%size;values[size_t(tail)]=v;times[size_t(tail)]=clock;tail=(tail+1)%size;while(head!=tail && times[size_t(head)]<clock-length)head=(head+1)%size;++clock;return values[size_t(head)];}
};
class Engine {
 int kind=0;double sr=48000;std::array<float,8> p{};std::array<juce::SmoothedValue<float>,8> smooth;
 Filter hp,mud,body,air,essDetect,essBand,thumpBand,punchBand,wetLow,wetHigh,dc;
 Envelope level,essEnv,fast,slow,opto,subEnv;float comp=1,optoGain=1,glue=1,phase=0,lfo=0,autoGain=1,limiterGain[2]{1,1};
 float previous[2]{},older[2]{},allpass[2][8]{},prevDiff[2]{};int count=0;
 Delay echo,roomDelay,look;std::array<MinimumWindow,2> minima;
 juce::Reverb reverb;SpectralClean spectral;
 mettavoxad_eng::PitchCorrectionEngine pitch;
 mettavoxad_eng::PitchDetector detector;
 mettavoxad_eng::VocoderEngine vocoder;
 std::array<mettavoxad_eng::GrainShifter,2> micro;
 std::array<juce::dsp::LinkwitzRileyFilter<float>,2> cross;
 juce::dsp::LinkwitzRileyFilter<float> lowAlign;
public:
 float grDb=0,wetDuck=1,pitchHz=0;int lookSamples=0;
 void prepare(int k,double rate,int maxBlock,const std::array<float,8>& values){kind=k;sr=rate;p=values;count=0;comp=optoGain=glue=autoGain=1;limiterGain[0]=limiterGain[1]=1;phase=lfo=0;grDb=0;previous[0]=previous[1]=older[0]=older[1]=0;for(auto& a:allpass)for(auto& z:a)z=0;
  for(auto& f:{&hp,&mud,&body,&air,&essDetect,&essBand,&thumpBand,&punchBand,&wetLow,&wetHigh,&dc})f->reset();
  level.setup(sr,3,120);essEnv.setup(sr,1,70);fast.setup(sr,1,35);slow.setup(sr,25,180);opto.setup(sr,12,240);subEnv.setup(sr,5,100);level.v=essEnv.v=fast.v=slow.v=opto.v=subEnv.v=0;
  spectral.reset();echo.prepare(sr,2.5);roomDelay.prepare(sr,.2);look.prepare(sr,.02);lookSamples=kind==6?int(sr*.005):0;for(auto& m:minima)m.prepare(lookSamples);
  reverb.setSampleRate(sr);reverb.reset();pitch.prepare(sr);detector.prepare(sr);vocoder.prepare(sr);for(auto& m:micro)m.init(juce::jmax(256,int(sr*.04)));
  juce::dsp::ProcessSpec spec{sr,juce::uint32(maxBlock),2};for(auto& c:cross){c.prepare(spec);c.reset();}cross[0].setCutoffFrequency(200);cross[1].setCutoffFrequency(4000);lowAlign.prepare(spec);lowAlign.setType(juce::dsp::LinkwitzRileyFilterType::allpass);lowAlign.setCutoffFrequency(4000);lowAlign.reset();
  for(size_t i=0;i<8;++i){smooth[i].reset(sr,.025);smooth[i].setCurrentAndTargetValue(values[i]);}configure(values);
 }
 void configure(const std::array<float,8>& values){p=values;for(size_t i=0;i<8;++i)smooth[i].setTargetValue(p[i]);
  hp.hp(sr,kind==0?35+values[4]*.9f:55+values[0]*.4f);mud.bp(sr,280,1.1f);dc.hp(sr,15);thumpBand.lp(sr,160);essBand.bp(sr,kind==0?p[3]:7400,1.3f);punchBand.bp(sr,kind==2?p[3]:3200,.8f);
  essDetect.bp(sr,kind==0?p[3]:7400,1.3f);
  body.peak(sr,135,.65f,kind==1?p[4]:kind==5?p[1]:0);air.shelf(sr,kind==5?p[3]:4500,kind==1?p[5]:kind==5?p[2]:0);wetLow.hp(sr,180);wetHigh.lp(sr,kind==4?p[5]:6500);
  pitch.setParams(kind==1 && p[1]>.01f,p[1],120,0,0,0,90,100,0);
  vocoder.setParams(1,100,78,4,60,kind==3?p[2]:0,kind==3?p[5]:0,0,0);
  juce::Reverb::Parameters rv;rv.roomSize=.28f;rv.damping=.65f;rv.wetLevel=1;rv.dryLevel=0;rv.width=.65f;reverb.setParameters(rv);
 }
 void process(float& l,float& r,float bpm){std::array<float,8> v;for(size_t i=0;i<8;++i)v[i]=smooth[i].getNextValue();const float raw[2]{l,r};float d[2]{l,r};const float env=level.tick(.5f*(std::abs(l)+std::abs(r)));const float s=essEnv.tick(.5f*(std::abs(essDetect.tick(0,l))+std::abs(essDetect.tick(1,r))));float reduction=0;
  if(kind==0){spectral.tick(l,r,v[0],v[1]);d[0]=l;d[1]=r;
   for(int c=0;c<2;++c){float x=d[c];float median=.5f*(older[c]+x);float delta=std::abs(previous[c]-median);float threshold=.22f-.0018f*v[5];float cleaned=v[5]>.01f && delta>threshold && std::abs(x-older[c])<threshold?median:previous[c];older[c]=previous[c];previous[c]=x;d[c]=hp.tick(c,cleaned);}
   const float low=.5f*(std::abs(thumpBand.tick(0,d[0]))+std::abs(thumpBand.tick(1,d[1])));float deThump=juce::jlimit(0.f,1.f,(low-env*.75f)*4)*v[4]*.01f;
   reduction=juce::jlimit(0.f,1.f,(db(s+1.e-6f)-db(env+.00001f)+18)/18)*v[2];
   for(int c=0;c<2;++c){float band=essBand.tick(c,d[c]);d[c]-=band*(1-gain(-reduction));d[c]-=thumpBand.tick(c,d[c])*deThump*.7f;}
   if(env>.005f){float target=juce::jlimit(.5f,2.f,.1f/(env+.00001f));autoGain+=(target-autoGain)*float(1-std::exp(-1/(sr*.5)));}for(auto& x:d)x*=1+(autoGain-1)*v[6]*.01f;
  }
  else if(kind==1){for(int c=0;c<2;++c){d[c]=hp.tick(c,d[c]);float m=mud.tick(c,d[c]);float cut=juce::jlimit(0.f,.6f,std::abs(m)*4)*v[0]*.01f;d[c]-=m*cut;d[c]=air.tick(c,body.tick(c,d[c]));}pitch.processStereo(d[0],d[1]);
   const float e=fast.tick(.5f*(std::abs(d[0])+std::abs(d[1])));float threshold=-12-v[2]*.15f,ratio=2+v[2]*.05f;float over=juce::jmax(0.f,db(e)-threshold);float wanted=gain(-over*(1-1/ratio));comp+=(wanted-comp)*(wanted<comp?.04f:.0005f);float optical=opto.tick(e*comp);float opticalGr=juce::jmax(0.f,db(optical)+20)*v[2]*.004f;float wantedOpto=gain(-opticalGr);optoGain+=(wantedOpto-optoGain)*float(1-std::exp(-1/(sr*(wantedOpto<optoGain?.012:.2))));
   float makeup=gain(v[2]*.055f);for(auto& x:d)x*=comp*optoGain*makeup;reduction=-db(comp*optoGain);
   saturate(d,v[3]*.01f,.15f);float bus=juce::jmax(0.f,db(level.v)+14)*.15f;glue+=(gain(-bus)-glue)*.001f;for(auto& x:d)x*=glue;
   float a=wetLow.tick(0,roomDelay.read(0,float(sr*.028))),b=wetLow.tick(1,roomDelay.read(1,float(sr*.031)));roomDelay.push(d[0],d[1]);reverb.processStereo(&a,&b,1);float duck=1/(1+env*8);d[0]+=wetHigh.tick(0,a)*v[6]*.01f*duck;d[1]+=wetHigh.tick(1,b)*v[6]*.01f*duck;
  }
  else if(kind==2){float f=fast.tick(env),sl=slow.tick(env);float attack=juce::jlimit(0.f,1.f,(f-sl)/(sl+.02f));float sustained=1-attack;float amount=v[0]*attack+v[1]*sustained;float g=gain(amount);float expander=env<.004f?juce::jlimit(.2f,1.f,env/.004f):1;
   for(int c=0;c<2;++c){float band=punchBand.tick(c,d[c]);d[c]=d[c]*g*expander+band*attack*v[2]*.004f;}saturate(d,v[2]*.002f,.1f);float cap=gain(v[6]);for(auto& x:d)x=juce::jlimit(-cap,cap,x*gain(v[5]));reduction=juce::jmax(0.f,db(env)-db(.5f*(std::abs(d[0])+std::abs(d[1]))));fast.setup(sr,juce::jmax(1.f,v[4]*.15f),juce::jmax(8.f,v[4]*3));
  }
  else if(kind==3){if(v[0]>.01f||v[1]>.01f)detector.push(.5f*(l+r));pitchHz=detector.lastF0;
   float synth[2]{l,r};if(v[1]>.01f){if((++count&63)==0 && pitchHz>50)vocoder.setCarrierFrequency(pitchHz);vocoder.processStereo(synth[0],synth[1]);}
   const float consonant=juce::jlimit(0.f,1.f,s/(env+.01f));wetDuck=1-.35f*consonant*v[6]*.01f;float synthMix=v[1]*.01f*wetDuck;
   float moving[2]{l*(1-synthMix)+synth[0]*synthMix,r*(1-synthMix)+synth[1]*synthMix};
   lfo+=v[4]/float(sr);if(lfo>=1)lfo-=1;for(int c=0;c<2;++c){float x=moving[c];for(int b=0;b<8;++b){float hz=250.f*std::pow(1.43f,float(b))*(1+.65f*std::sin(juce::MathConstants<float>::twoPi*lfo+float(b)*.35f+float(c)*v[5]*.006f));float t=std::tan(juce::MathConstants<float>::pi*hz/float(sr));float a=(1-t)/(1+t);float y=-a*x+allpass[c][b];allpass[c][b]=x+a*y;x=y;}moving[c]=moving[c]*(1-v[3]*.004f)+x*v[3]*.004f;}
   float left=micro[0].process(moving[0],std::pow(2.f,-v[5]*.08f/1200));float right=micro[1].process(moving[1],std::pow(2.f,v[5]*.08f/1200));float width=v[5]*.003f;moving[0]=moving[0]*(1-width)+left*width;moving[1]=moving[1]*(1-width)+right*width;
   float subFreq=pitchHz>0?pitchHz*.5f:60;while(subFreq>90)subFreq*=.5f;while(subFreq<40)subFreq*=2;phase+=subFreq/float(sr);if(phase>=1)phase-=1;float subLevel=subEnv.tick(env);float subDuck=1/(1+s*15);float sub=std::sin(juce::MathConstants<float>::twoPi*phase)*subLevel*(gain(v[0])-1)*.45f*subDuck;
   for(int c=0;c<2;++c)d[c]=raw[c]*(1-v[7]*.01f)+moving[c]*v[7]*.01f*wetDuck+sub;
   saturate(d,.1f,.1f);
  }
  else if(kind==4){const int rhythm=juce::jlimit(0,3,int(std::round(v[6])));float quarter=float(sr*60/juce::jlimit(30.f,300.f,bpm));float timeL=float(sr)*v[0]*.001f,timeR=timeL*1.1f;if(rhythm){timeR=quarter;timeL=quarter*(rhythm==1?.75f:rhythm==2?1.5f:.5f);}float a=wetHigh.tick(0,wetLow.tick(0,echo.read(0,timeL))),b=wetHigh.tick(1,wetLow.tick(1,echo.read(1,timeR)));
   float mid=(a+b)*.5f,side=(a-b)*.5f*v[2]*.01f;a=mid+side;b=mid-side;a=micro[0].process(a,std::pow(2.f,-v[3]/1200));b=micro[1].process(b,std::pow(2.f,v[3]/1200));echo.push(l+b*v[1]*.01f,r+a*v[1]*.01f);
   wetDuck=gain(-v[4]*juce::jlimit(0.f,1.f,env/.08f));d[0]=l+a*v[7]*.01f*wetDuck;d[1]=r+b*v[7]*.01f*wetDuck;
  }
  else if(kind==5){for(int c=0;c<2;++c)d[c]=body.tick(c,d[c]);saturate(d,v[0]*.01f*(v[6]>.5f?.75f:1.f),v[4]*.006f);float protect=1-juce::jlimit(0.f,.8f,s/(env+.01f)*v[5]*.006f);for(int c=0;c<2;++c){float bright=air.tick(c,d[c]);d[c]+=(bright-d[c])*protect;}reduction=(1-protect)*6;}
  else if(kind==6){float driven[2]{l*gain(v[0]*v[7]*.01f),r*gain(v[0]*v[7]*.01f)};float cap=gain(v[1]-.6f);float req[2]{1,1};for(int c=0;c<2;++c){float x=driven[c];float density=v[5]*v[7]*.0001f;driven[c]=x*(1-density)+std::tanh(x)*density;req[c]=juce::jmin(1.f,cap/(std::abs(driven[c])+1.e-12f));}float link=v[6]*.01f;float common=juce::jmin(req[0],req[1]);for(int c=0;c<2;++c){float target=minima[size_t(c)].push(req[c]*(1-link)+common*link);if(target<limiterGain[c])limiterGain[c]=target;else limiterGain[c]+=(target-limiterGain[c])*float(1-std::exp(-1/(sr*v[2]*.001)));d[c]=look.read(c,float(lookSamples))*limiterGain[c];}look.push(driven[0],driven[1]);reduction=-db(juce::jmin(limiterGain[0],limiterGain[1]));}
  grDb=reduction;
  if(kind!=0 && kind!=3 && kind!=4 && kind!=6){float mix=v[7]*.01f;for(int c=0;c<2;++c)d[c]=raw[c]*(1-mix)+d[c]*mix;}
  l=kind==6?finite(d[0]):dc.tick(0,finite(d[0]));r=kind==6?finite(d[1]):dc.tick(1,finite(d[1]));
 }
private:
 void saturate(float (&x)[2],float drive,float softness){if(drive<.0001f)return;float d=1+drive*5;for(int c=0;c<2;++c){float lo,hi,mid,top;cross[0].processSample(c,x[c],lo,hi);cross[1].processSample(c,hi,mid,top);lo=lowAlign.processSample(c,lo);float lowWet=std::tanh(lo*(1+drive*2))/(1+drive*2),midWet=std::tanh(mid*d)/d,topWet=std::tanh(top*(1+drive))/(1+drive);float clean=lo+mid+top;float shaped=lowWet+midWet+topWet;float safety=1/(1+std::abs(top)*softness*10);x[c]=clean+(shaped-clean)*drive*safety;}}
};
}
