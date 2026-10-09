#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>
namespace mv3 {
struct Control { const char* id; const char* label; float min,max,step,def; const char* unit; };
struct Preset { const char* name; std::array<float,8> values; };
struct Product { const char* name; const char* slug; const char* role; juce::uint32 colour; std::array<Control,8> controls; std::array<Preset,6> presets; };
inline const std::array<Product,7> products {{
{"De-Con","DeCon","RESTAURACAO / VOZ LIMPA",0xff00d9ff,{{
{"noise","RUIDO",0,24,.1f,8,"dB"},{"floor","PISO DE RUIDO",-80,-25,.1f,-58,"dB"},{"ess","DE-ESS",0,12,.1f,4,"dB"},{"essHz","FAIXA S",4500,9500,10,7200,"Hz"},{"thump","DE-THUMP",0,100,1,35,"%"},{"click","DE-CLICK",0,100,1,20,"%"},{"stage","GAIN STAGING",0,100,1,0,"%"},{"mix","DRY / WET",0,100,1,100,"%"}}},{{
{"Studio Vocal Sanitizer",{8,-58,4,7200,35,20,0,100}},{"Clean Broadcast",{12,-55,5,6800,55,35,25,100}},{"Gentle Narration",{4,-65,2.5f,7500,20,10,0,100}},{"Plosive Rescue",{6,-60,3,7000,90,45,0,100}},{"Bright Mic Control",{6,-60,7,7800,25,20,0,100}},{"Noisy Room",{18,-48,4,6500,45,40,0,100}}}},
{"The Session Strip","SessionStrip","TONALIDADE / DINAMICA / CARATER",0xff36edb0,{{
{"clean","LIMPEZA",0,100,1,25,"%"},{"tune","AFINACAO",0,40,1,0,"%"},{"compress","COMPRESSAO",0,100,1,45,"%"},{"drive","SATURACAO",0,100,1,20,"%"},{"body","CORPO",-6,6,.1f,1.5f,"dB"},{"air","PRESENCA",0,6,.1f,2,"dB"},{"room","SALA",0,30,.1f,8,"%"},{"mix","DRY / WET",0,100,1,100,"%"}}},{{
{"Radio Presence",{25,0,45,20,1.5f,2,8,100}},{"Velvet Commercial",{20,0,30,25,2,1.5f,10,100}},{"Trailer Authority",{35,0,70,35,3,2.5f,4,100}},{"Crystal Narration",{40,0,25,10,0,3,5,100}},{"Pop Vocal Polish",{25,15,50,20,1,2,12,100}},{"Dense Promo",{35,0,80,45,2,3,3,100}}}},
{"Transient Dynamix","TransientDynamix","ATAQUE / SUSTAIN / PUNCH",0xffffaf4b,{{
{"attack","ATAQUE",-6,6,.1f,2.5f,"dB"},{"sustain","SUSTAIN",-6,6,.1f,-1,"dB"},{"punch","PUNCH",0,100,1,25,"%"},{"focus","FOCO",1000,8000,10,3000,"Hz"},{"speed","RESPOSTA",1,40,.1f,8,"ms"},{"clip","CLIP DRIVE",0,12,.1f,1,"dB"},{"ceiling","TETO",-6,-.1f,.1f,-1,"dB"},{"mix","DRY / WET",0,100,1,100,"%"}}},{{
{"In-Your-Face VO",{2.5f,-1,25,3000,8,1,-1,100}},{"Soft Consonants",{-2,1,0,2500,15,0,-1,100}},{"Cut Through Music",{4,-2,50,3800,5,2,-1,100}},{"Narration Detail",{1,0,10,2200,12,0,-1,100}},{"Percussive Drops",{5,-4,65,4500,3,3,-1,100}},{"Natural Punch",{2,-.5f,15,2800,10,0,-1,70}}}},
{"Sonic Matrix","SonicMatrix","SUB / VOX-SYNTH / AURA",0xffb38bff,{{
{"sub","SUB-PUNCH",0,8,.1f,2,"dB"},{"synth","VOX-SYNTH",0,100,1,0,"%"},{"formant","FORMANTES",-8,8,.1f,0,"st"},{"motion","AURA-SHIFT",0,100,1,15,"%"},{"rate","MOVIMENTO",.05f,3,.01f,.2f,"Hz"},{"width","ESTEREO",0,100,1,35,"%"},{"duck","INTELIGIBILIDADE",0,100,1,70,"%"},{"mix","DRY / WET",0,100,1,35,"%"}}},{{
{"Monster Promo",{4,0,-2,20,.12f,30,80,35}},{"Cyborg Hybrid",{3,45,-3,25,.4f,45,85,60}},{"Radio Flyer",{4,25,4,85,1.4f,65,90,70}},{"Subliminal Depth",{1,8,1,8,.15f,25,75,20}},{"Neon Talkbox",{1,80,0,35,.6f,40,90,70}},{"Mono Power Drop",{5,15,-5,15,.18f,0,100,40}}}},
{"SpaceWeaver","SpaceWeaver","DELAY / MICRO-PITCH / DUCKING",0xff58a6ff,{{
{"time","TEMPO",60,900,1,125,"ms"},{"feedback","FEEDBACK",0,65,1,25,"%"},{"spread","ESPALHAMENTO",0,100,1,65,"%"},{"micro","MICRO-PITCH",0,15,.1f,7,"cent"},{"duck","DUCKING",0,24,.1f,12,"dB"},{"tone","COR DO ECO",1500,12000,10,6500,"Hz"},{"rhythm","RITMO",0,3,1,1,"mode"},{"mix","DRY / WET",0,100,1,22,"%"}}},{{
{"3D Spread & Throw",{125,25,65,7,12,6500,1,22}},{"Tight Studio Double",{65,8,40,5,8,8000,0,16}},{"Quarter Note Throw",{125,38,60,7,16,5500,1,28}},{"Dotted Motion",{125,35,75,9,15,7000,2,25}},{"Luxury Depth",{85,12,30,3,10,5000,0,12}},{"Wide Sweeper",{180,45,85,12,18,4500,3,35}}}},
{"Exciter 808","Exciter808","FITA / HARMONICOS / AIR",0xffff7baa,{{
{"tape","FITA",0,100,1,30,"%"},{"body","PESO",0,6,.1f,1,"dB"},{"air","AIR",0,6,.1f,3,"dB"},{"airHz","AIR FREQ",8000,16000,10,14000,"Hz"},{"silk","SEDA",0,100,1,55,"%"},{"ess","S PROTECT",0,100,1,55,"%"},{"speed","FITA IPS",0,1,1,0,"mode"},{"mix","DRY / WET",0,100,1,100,"%"}}},{{
{"Silk & Tape 15ips",{30,1,3,14000,55,55,0,100}},{"Crystal Air",{15,.5f,4,12000,35,75,1,100}},{"Warm Dynamic Mic",{45,2,3.5f,10000,65,60,0,100}},{"Velvet Narrator",{35,1.5f,1.5f,14000,85,50,0,100}},{"Promo Spark",{50,2,4,12000,40,80,1,100}},{"Gentle Finish",{10,.5f,1,15000,70,50,1,70}}}},
{"Maximum Ceiling","MaximumCeiling","LOUDNESS / LOOKAHEAD / TRUE PEAK",0xffff675e,{{
{"drive","DRIVE",0,18,.1f,4,"dB"},{"ceiling","TETO TP",-3,-.1f,.1f,-.3f,"dBTP"},{"release","RELEASE",20,400,1,100,"ms"},{"target","ALVO LUFS",-24,-6,.1f,-8,"LUFS"},{"auto","AUTO LEVEL",0,100,1,0,"%"},{"tone","DENSIDADE",0,100,1,20,"%"},{"link","STEREO LINK",50,100,1,100,"%"},{"mix","DRY / WET",0,100,1,100,"%"}}},{{
{"Radio Wall - Slammed & Clean",{8,-.3f,100,-8,0,25,100,100}},{"Streaming Balanced",{3,-1,140,-14,0,10,100,100}},{"YouTube Voice",{3,-1,120,-14,0,15,100,100}},{"TV Delivery Starting Point",{1,-1,180,-23,0,5,100,100}},{"FM Promo Dense",{6,-1,80,-10,0,30,100,100}},{"Transparent Safety",{0,-1,150,-16,0,0,100,100}}}}
}};
}
