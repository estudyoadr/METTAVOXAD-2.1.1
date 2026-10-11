#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Registry.h"
#include "Dsp.h"
class BundleProcessor final : public juce::AudioProcessor {
public:
 explicit BundleProcessor(int productKind);
 void prepareToPlay(double,int) override;
 void releaseResources() override {}
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 void processBlockBypassed(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override{return true;}
 const juce::String getName() const override{return juce::String(product().name);}
 bool acceptsMidi() const override{return false;} bool producesMidi() const override{return false;} bool isMidiEffect() const override{return false;}
 double getTailLengthSeconds() const override{return kind==4?8:kind==1?2:0;}
 int getNumPrograms() override{return kind==1?11:7;}int getCurrentProgram() override{return program.load();}
 void setCurrentProgram(int) override;const juce::String getProgramName(int) override;void changeProgramName(int,const juce::String&) override{}
 void getStateInformation(juce::MemoryBlock&) override;void setStateInformation(const void*,int) override;
 const mv3::Product& product()const{return mv3::products[size_t(kind)];}int getKind() const{return kind;}
 std::array<float,8> values()const;
 juce::AudioProcessorValueTreeState apvts;
 std::atomic<float> inputDb{-100},leftDb{-100},rightDb{-100},grDb{0},duck{1},correlation{1},lufsM{-100},lufsS{-100},lufsI{-100},tpDb{-100};
 std::atomic<int> program{0};
private:
 static juce::AudioProcessorValueTreeState::ParameterLayout layout(int);
 int kind;int maximum=512,latency=0;double rate=48000,factor=1;
 mv3::Engine engine;mv3::Delay dryDelay;mv3::Loudness loudness;
 std::unique_ptr<juce::dsp::Oversampling<float>> os,tp;
 juce::AudioBuffer<float> work,dry;
 juce::SmoothedValue<float> inputGain,outputGain,active,autoLevel;
 std::atomic<float>* bypassParameter=nullptr;
 std::atomic<float>* micProfile=nullptr;
 std::atomic<float>* focusParameter=nullptr;std::atomic<float>* matchParameter=nullptr;
 float matchInPower=0,matchOutPower=0;juce::SmoothedValue<float> matchGain;
 std::array<std::atomic<float>*,8> params{};
 std::atomic<float>* in=nullptr;std::atomic<float>* out=nullptr;
 void run(juce::AudioBuffer<float>&,bool);
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BundleProcessor)
};
