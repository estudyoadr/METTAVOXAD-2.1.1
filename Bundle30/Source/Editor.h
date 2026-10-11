#pragma once
#include "Processor.h"
class BundleLook final : public juce::LookAndFeel_V4 {
public:void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&)override;
};
class MatrixPad final : public juce::Component {
 BundleProcessor& processor;
public:explicit MatrixPad(BundleProcessor& p):processor(p){setName("Matrix XY");}
 void paint(juce::Graphics&)override;void mouseDown(const juce::MouseEvent&)override;void mouseDrag(const juce::MouseEvent&)override;void mouseUp(const juce::MouseEvent&)override;
private:void set(const juce::MouseEvent&);bool gesture=false;
};
class BundleEditor final : public juce::AudioProcessorEditor,private juce::Timer {
 BundleProcessor& processor;BundleLook look;
 std::array<juce::Slider,8> knobs;std::array<juce::Label,8> labels;
 juce::Slider input,output;juce::ComboBox presets,timbre;juce::ToggleButton bypass{"BYPASS"},focus{"FOCUS"},match{"MATCH"};juce::Label status;
 MatrixPad pad;std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
 std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment,focusAttachment,matchAttachment;
 std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> timbreAttachment;
public:explicit BundleEditor(BundleProcessor&);~BundleEditor()override;void parentHierarchyChanged()override{
#if JUCE_WINDOWS
if(auto* peer=getPeer())peer->setCurrentRenderingEngine(0);
#endif
}void paint(juce::Graphics&)override;void resized()override;
private:void timerCallback()override;
};
