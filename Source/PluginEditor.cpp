#include "PluginEditor.h"
namespace {
const char* names[]{"VOCAL","AUTOTUNE","MASTER","VOZ ROBO"};
const char* toggles[]{"vocalOn","atOn","masterOn","vocOn"};
const char* tabNames[]{"tab_vocal","tab_autotune","tab_master","tab_robot"};
const uint32_t colours[]{0xff00d9ff,0xffb38bff,0xff36edb0,0xffffaf4b};
const char* ids[4][8]{
 {"macroPeso","macroCalor","macroClareza","macroPresenca","macroCompressao","macroProfundidade","macroHarmonia","macroSaida"},
 {"atAmount","atSpeed","atTranspose","atHumanise","atMix","atOutput","atKey","atScale"},
 {"mstLowDb","mstMidDb","mstHighDb","mstMbAmount","mstExciter","mstWidth","mstLoudness","mstCeilingDb"},
 {"vocMix","vocTone","vocTexture","vocFormant","vocNote","vocWidth","vocDepth","vocOutput"}};
const char* titles[4][8]{
 {"PESO","CALOR","CLAREZA","PRESENCA","COMPRESSAO","PROFUNDIDADE","HARMONIA","SAIDA (dB)"},
 {"INTENSIDADE","VELOCIDADE (ms)","TRANSPOSICAO","HUMANIZACAO","MISTURA","SAIDA (dB)","TONALIDADE","ESCALA"},
 {"GRAVE (dB)","MEDIO (dB)","AGUDO (dB)","MULTIBANDA","EXCITER","LARGURA","LOUDNESS","TETO (dB)"},
 {"MISTURA","TIMBRE","TEXTURA","FORMANTES","PORTADORA","LARGURA","ESPACO","SAIDA (dB)"}};
}
MettavoxadAudioProcessorEditor::MettavoxadAudioProcessorEditor(MettavoxadAudioProcessor& p):AudioProcessorEditor(&p),processor(p) {
    setOpaque(true);setLookAndFeel(&premiumLook);
    for(int m=0;m<4;++m) {
        for(int k=0;k<8;++k) setupKnob(m,k,ids[m][k],titles[m][k]);
        tabs[m].setName(tabNames[m]);tabs[m].setButtonText(names[m]);tabs[m].setClickingTogglesState(true);tabs[m].setRadioGroupId(42);
        tabs[m].setColour(juce::TextButton::buttonColourId,juce::Colour(0xff101c24));
        tabs[m].setColour(juce::TextButton::buttonOnColourId,juce::Colour(colours[m]).withMultipliedBrightness(0.3f));
        tabs[m].setColour(juce::TextButton::textColourOnId,juce::Colour(colours[m]));
        tabs[m].onClick=[this,m]{showTab(m);};addAndMakeVisible(tabs[m]);
        auto& button=moduleButtons[m];button.setClickingTogglesState(true);button.setName(toggles[m]);
        button.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff18232d));
        button.setColour(juce::TextButton::buttonOnColourId,juce::Colour(colours[m]).withMultipliedBrightness(0.35f));
        button.setColour(juce::TextButton::textColourOnId,juce::Colour(colours[m]));addAndMakeVisible(button);
        buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.apvts,toggles[m],button));
        button.onClick=[this,m] {
            if(!moduleButtons[m].getToggleState()) return;
            auto restore=[this](const char* id,float value) {if(auto* param=processor.apvts.getParameter(id)) if(param->getValue()<=0) param->setValueNotifyingHost(param->convertTo0to1(value));};
            if(m==1) {restore("atAmount",85);restore("atMix",100);}
            if(m==3) {restore("vocMode",1);restore("vocMix",85);}
            timerCallback();
        };
        soloButtons[m].setButtonText("SOLO");soloButtons[m].setName("solo_"+juce::String(m));
        soloButtons[m].setColour(juce::TextButton::buttonColourId,juce::Colour(0xff192c38));
        soloButtons[m].onClick=[this,m] {
            processor.soloModule(m);
            if(m==3 && processor.apvts.getRawParameterValue("vocMode")->load()<0.5f) processor.applyModulePreset(3,0);
            timerCallback();
        };addAndMakeVisible(soloButtons[m]);
        presets[m].setName("preset_"+juce::String(m));presets[m].addItemList(p.getModulePresetNames(m),1);
        presets[m].setTextWhenNothingSelected("Escolha um preset / Personalizado");
        presets[m].setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff12222d));
        presets[m].setColour(juce::ComboBox::textColourId,juce::Colour(colours[m]));
        presets[m].onChange=[this,m] {const int index=presets[m].getSelectedId()-1;if(index>=0) processor.applyModulePreset(m,index);timerCallback();};
        addAndMakeVisible(presets[m]);
    }
    robotMode.setName("vocMode");
    if(auto* param=dynamic_cast<juce::AudioParameterChoice*>(p.apvts.getParameter("vocMode"))) robotMode.addItemList(param->choices,1);
    robotMode.setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff12222d));
    robotMode.setColour(juce::ComboBox::textColourId,juce::Colour(colours[3]));addAndMakeVisible(robotMode);
    modeAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.apvts,"vocMode",robotMode);
    addAndMakeVisible(hq);addAndMakeVisible(bypass);addAndMakeVisible(status);
    hq.setName("qualityOversample");bypass.setName("globalBypass");
    buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.apvts,"qualityOversample",hq));
    buttonAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.apvts,"globalBypass",bypass));
    status.setJustificationType(juce::Justification::centred);
    status.setFont(juce::Font(10.0f,juce::Font::bold));
    tabs[0].setToggleState(true,juce::dontSendNotification);showTab(0);
    setResizable(true,true);setResizeLimits(660,330,990,495);setSize(660,330);
    timerCallback();startTimerHz(20);
}
MettavoxadAudioProcessorEditor::~MettavoxadAudioProcessorEditor(){stopTimer();setLookAndFeel(nullptr);}
void MettavoxadAudioProcessorEditor::setupKnob(int m,int slot,const char* id,const char* title) {
    auto& knob=knobs[m*8+slot];auto& label=labels[m*8+slot];
    knob.setName(id);knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow,false,76,16);
    knob.setColour(juce::Slider::rotarySliderFillColourId,juce::Colour(colours[m]));
    label.setText(title,juce::dontSendNotification);label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(10.0f,juce::Font::bold));label.setColour(juce::Label::textColourId,juce::Colour(0xffa9c3d1));
    addAndMakeVisible(knob);addAndMakeVisible(label);
    sliderAttachments.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.apvts,id,knob));
    if(auto* parameter=processor.apvts.getParameter(id)) {
        const auto& range=parameter->getNormalisableRange();
        if(range.start<0 && range.end>0) knob.getProperties().set("neutral",0.0);
        if(auto* choice=dynamic_cast<juce::AudioParameterChoice*>(parameter)) {
            const auto choices=choice->choices;
            knob.textFromValueFunction=[choices](double value){return choices[juce::jlimit(0,choices.size()-1,static_cast<int>(std::round(value)))];};
            knob.valueFromTextFunction=[choices](const juce::String& text){return static_cast<double>(juce::jmax(0,choices.indexOf(text)));};
        }
    }
    if(juce::String(id)=="atSpeed" || juce::String(id)=="mstCeilingDb") knob.getProperties().set("reverseIntensity",true);
    if(juce::String(id)=="mstWidth") knob.getProperties().set("neutral",100.0);
}
void MettavoxadAudioProcessorEditor::showTab(int index) {
    currentTab=juce::jlimit(0,3,index);
    for(int m=0;m<4;++m) {
        presets[m].setVisible(m==currentTab);
        for(int k=0;k<8;++k){knobs[m*8+k].setVisible(m==currentTab);labels[m*8+k].setVisible(m==currentTab);}
    }
    robotMode.setVisible(currentTab==3);resized();timerCallback();repaint();
}
void MettavoxadAudioProcessorEditor::timerCallback() {
    const bool off=bypass.getToggleState();
    int count=0;
    for(int m=0;m<4;++m) {
        const bool active=moduleButtons[m].getToggleState();if(active) ++count;
        moduleButtons[m].setButtonText(juce::String(names[m])+(active?" ON":" OFF"));
        const bool processing=active && !off && (m!=3 || processor.apvts.getRawParameterValue("vocMode")->load()>0);
        for(int k=0;k<8;++k) {
            auto& knob=knobs[m*8+k];
            if(!knob.getProperties().contains("moduleOn") || static_cast<bool>(knob.getProperties()["moduleOn"])!=processing) {knob.getProperties().set("moduleOn",processing);knob.repaint();}
        }
        presets[m].setSelectedId(processor.getModulePresetIndex(m)+1,juce::dontSendNotification);
    }
    juce::String message;
    if(off) message="BYPASS GLOBAL ATIVO: AUDIO ORIGINAL";
    else if(count==0) message="TODAS AS ABAS DESLIGADAS: AUDIO ORIGINAL";
    else {
        message="MODULOS ATIVOS: ";for(int m=0;m<4;++m) if(moduleButtons[m].getToggleState()) message+=juce::String(names[m])+"  ";
        if(currentTab==1) {
            const float hz=processor.livePitchHz.load();message+=hz>0?"| PITCH "+juce::String(hz,1)+" Hz":"| AGUARDANDO VOZ MONOFONICA";
        }
    }
    status.setText(message,juce::dontSendNotification);status.setColour(juce::Label::textColourId,juce::Colour(off?0xffff8b8b:0xff82d6df));
    repaint();
}
void MettavoxadAudioProcessorEditor::paint(juce::Graphics& g) {
    const float w=static_cast<float>(getWidth()),h=static_cast<float>(getHeight());
    const auto accent=juce::Colour(colours[currentTab]);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff18212d),0,0,juce::Colour(0xff070b12),w,h,false));g.fillRect(getLocalBounds());
    g.setColour(juce::Colour(0xffe6f7ff));g.setFont(juce::Font(19.0f,juce::Font::bold));
    g.drawText("METTAVOX AD",14,7,190,23,juce::Justification::centredLeft);
    g.setColour(accent);g.setFont(juce::Font(9.0f));
    g.drawText("2.1.1  /  VOCAL DESIGN  /  24 PRESETS",14,30,270,13,juce::Justification::centredLeft);
    const juce::Rectangle<float> card(12,109,w-76,h-136);
    g.setColour(juce::Colour(0xff0c1420));g.fillRoundedRectangle(card,10);
    g.setColour(accent.withAlpha(0.32f));g.drawRoundedRectangle(card,10,1);
    const float signal=juce::jlimit(0.0f,1.0f,(processor.meterOutputRmsDb.load()+60.0f)/60.0f);
    for(int m=0;m<4;++m) {
        const bool on=moduleButtons[m].getToggleState() && !bypass.getToggleState();
        const float x=18.0f+m*((w-28.0f)/4.0f);
        g.setColour(juce::Colour(colours[m]).withAlpha(on?0.35f+0.65f*signal:0.12f));
        g.fillEllipse(x,49,4,4);
    }
    g.setColour(juce::Colour(0xff7aabba));g.setFont(juce::Font(9.0f));
    g.drawText("OUT",getWidth()-57,113,45,14,juce::Justification::centred);
    const float meterHeight=h-174.0f;
    const float levels[]{processor.meterOutputLeftDb.load(),processor.meterOutputRightDb.load()};
    for(int c=0;c<2;++c) {
        const float x=w-49+c*18.0f;
        g.setColour(juce::Colour(0xff1b2935));g.fillRoundedRectangle(x,134,10,meterHeight,3);
        const float fill=juce::jlimit(0.0f,1.0f,(levels[c]+60.0f)/60.0f)*meterHeight;
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xffffb84c),x,134,juce::Colour(0xff36edb0),x,134+meterHeight,false));
        if(fill>0) g.fillRoundedRectangle(x,134+meterHeight-fill,10,fill,3);
        g.setColour(juce::Colour(0xff7aabba));g.drawText(c==0?"L":"R",static_cast<int>(x)-2,getHeight()-37,14,12,juce::Justification::centred);
    }
}
void MettavoxadAudioProcessorEditor::resized() {
    const int w=getWidth(),h=getHeight(),gap=6,cell=(w-28-3*gap)/4;
    hq.setBounds(w-262,11,70,24);bypass.setBounds(w-190,11,178,24);
    for(int m=0;m<4;++m) {
        const int x=14+m*(cell+gap);
        moduleButtons[m].setBounds(x,56,cell-41,22);soloButtons[m].setBounds(x+cell-39,56,39,22);
        tabs[m].setBounds(x,83,cell,22);
        presets[m].setBounds(22,117,currentTab==3?(w-106)/2:w-96,23);
    }
    robotMode.setBounds(28+(w-106)/2,117,(w-106)/2,23);
    const int knobCell=(w-96)/4,row=(h-177)/2;
    for(int m=0;m<4;++m) for(int k=0;k<8;++k) {
        const int x=22+(k%4)*knobCell,y=145+(k/4)*row;
        labels[m*8+k].setBounds(x,y,knobCell,13);
        knobs[m*8+k].setBounds(x,y+14,knobCell,row-15);
    }
    status.setBounds(14,h-23,w-28,18);
}
