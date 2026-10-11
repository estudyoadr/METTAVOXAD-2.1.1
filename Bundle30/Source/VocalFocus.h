#pragma once
// Included inside mv3 after Filter and Envelope. No allocations in process.
// The clean signal remains the reference: multiband colour adds only nonlinear
// residuals, avoiding crossover allpass rotation in the parallel clean path.
class VocalFocus {
 double sr=48000;
 Filter highpass,detectorHighpass,mudBand,weight,presence,shine,essBand,lowColour,upperColour;
 Envelope levelDetector,essDetector,mudDetector;
 std::array<juce::SmoothedValue<float>,8> controls;
 juce::SmoothedValue<float> profile;
 float rmsPower=0,levellerDb=0,catcherDb=0,essDb=0,mudGain=0;
 int clock=0,interval=48;
 std::array<float,8> v{};
 float coefficient(float seconds)const{return float(1-std::exp(-1/(sr*seconds)));}
 void filters(){
  const float tube=profile.getCurrentValue();
  highpass.hp(sr,45+v[0]*.3f);
  weight.peak(sr,110,.72f,v[4]);
  presence.peak(sr,3200-400*tube,.8f,v[5]*.55f);
  shine.shelf(sr,10000,v[5]*.45f);
 }
public:
 float grDb=0,deEssDb=0;
 void prepare(double sampleRate,const std::array<float,8>& values,int timbre){
  sr=sampleRate;clock=0;interval=juce::jmax(1,int(sr*.001));v=values;
  rmsPower=levellerDb=catcherDb=essDb=mudGain=0;grDb=deEssDb=0;
  for(auto* f:{&highpass,&detectorHighpass,&mudBand,&weight,&presence,&shine,&essBand,&lowColour,&upperColour})f->reset();
  for(size_t n=0;n<8;++n){controls[n].reset(sr,.04);controls[n].setCurrentAndTargetValue(values[n]);}
  profile.reset(sr,.06);profile.setCurrentAndTargetValue(timbre==2?1.f:0.f);
  detectorHighpass.hp(sr,120);mudBand.bp(sr,260,.8f);essBand.bp(sr,7000,1.f);
  lowColour.lp(sr,180);upperColour.lp(sr,4200);
  levelDetector.setup(sr,3,90);essDetector.setup(sr,1,65);mudDetector.setup(sr,15,150);
  levelDetector.v=essDetector.v=mudDetector.v=0;filters();
 }
 void configure(const std::array<float,8>& values,int timbre){for(size_t n=0;n<8;++n)controls[n].setTargetValue(values[n]);profile.setTargetValue(timbre==2?1.f:0.f);}
 void process(float& left,float& right){
  for(size_t n=0;n<8;++n)v[n]=controls[n].getNextValue();profile.getNextValue();
  if(++clock>=interval){clock=0;filters();}
  const float dry[2]{left,right};float x[2]{highpass.tick(0,left),highpass.tick(1,right)};
  const float level=levelDetector.tick(juce::jmax(std::abs(x[0]),std::abs(x[1])));
  float mud[2]{mudBand.tick(0,x[0]),mudBand.tick(1,x[1])};
  const float lowMid=mudDetector.tick(juce::jmax(std::abs(mud[0]),std::abs(mud[1])));
  // Only dominant low mids trigger subtraction; restrained by cleanup macro.
  const float desired=juce::jlimit(0.f,.4f,(lowMid/(level+.005f)-.5f)*.7f)*v[0]*.01f;
  mudGain+=(desired-mudGain)*coefficient(desired>mudGain?.018f:.16f);
  for(int c=0;c<2;++c)x[c]=shine.tick(c,presence.tick(c,weight.tick(c,x[c]-mud[c]*mudGain)));
  const float a=detectorHighpass.tick(0,x[0]),b=detectorHighpass.tick(1,x[1]);
  const float power=juce::jmax(a*a,b*b);rmsPower+=(power-rmsPower)*coefficient(.012f);
  const float amount=v[2]*.01f,ratio=1+4*amount;
  const float over=db(std::sqrt(juce::jmax(0.f,rmsPower)))-(-19-7*amount),knee=9;
  const float curved=over<=-knee*.5f?0:over<knee*.5f?(over+knee*.5f)*(over+knee*.5f)/(2*knee):over;
  const float wanted=juce::jmin(18.f,curved*(1-1/ratio));
  levellerDb+=(wanted-levellerDb)*coefficient(wanted>levellerDb?.012f:.16f+levellerDb*.018f);
  const float peak=juce::jmax(std::abs(x[0]),std::abs(x[1]));
  const float caught=juce::jlimit(0.f,4.f,(db(peak)+8)*.65f*amount);
  catcherDb+=(caught-catcherDb)*coefficient(caught>catcherDb?.0015f:.065f);
  grDb=levellerDb+catcherDb;const float dynamics=gain(-grDb+3*amount);
  float ess[2]{essBand.tick(0,x[0]),essBand.tick(1,x[1])};
  const float s=essDetector.tick(juce::jmax(std::abs(ess[0]),std::abs(ess[1])));
  const float desiredEss=juce::jlimit(0.f,4.5f,(s/(level+.008f)-.35f)*7.f);
  essDb+=(desiredEss-essDb)*coefficient(desiredEss>essDb?.0015f:.07f);deEssDb=essDb;
  const float drive=v[3]*.01f,tube=profile.getCurrentValue();
  for(int c=0;c<2;++c){
   float clean=(x[c]-ess[c]*(1-gain(-essDb)))*dynamics;
   const float lo=lowColour.tick(c,clean),upper=upperColour.tick(c,clean),mid=upper-lo,hi=clean-upper;
   const float lowDrive=1+drive*1.5f,midDrive=1+drive*3.f;
   const float bias=.035f+.055f*tube;
   const float biasTanh=std::tanh(bias);
   const float shapedMid=(std::tanh(mid*midDrive+bias)-biasTanh)/(midDrive*(1-biasTanh*biasTanh));
   const float colour=(std::tanh(lo*lowDrive)/lowDrive-lo)*.35f+(shapedMid-mid)*.55f+(std::tanh(hi)-hi)*.1f;
   const float processed=clean+colour*drive;
   x[c]=dry[c]+(processed-dry[c])*v[7]*.01f;
  }
  left=finite(x[0]);right=finite(x[1]);
 }
};
