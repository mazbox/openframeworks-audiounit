// GUI default dimensions
#define GUI_WIDTH 300
#define GUI_HEIGHT 300


#define NUM_PARAMS 3


#define PLUG_MFR "openFrameworks"
#define PLUG_NAME "ofxAudioPluginExample"

#define PLUG_CLASS_NAME ofxAudioPluginExample

#define BUNDLE_MFR "openFrameworks"
#define BUNDLE_NAME "ofxAudioPluginExample"

#define PLUG_ENTRY ofxAudioPluginExample_Entry
#define PLUG_VIEW_ENTRY ofxAudioPluginExample_ViewEntry

#define PLUG_ENTRY_STR "ofxAudioPluginExample_Entry"
#define PLUG_VIEW_ENTRY_STR "ofxAudioPluginExample_ViewEntry"

#define VIEW_CLASS ofxAudioPluginExample_View
#define VIEW_CLASS_STR "ofxAudioPluginExample_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'oFAx'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'oFAp'

// ProTools stuff
#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'GLN1', 'GLN2'};
#endif

#define PLUG_CHANNEL_IO "1-1 2-2"

#define PLUG_LATENCY 0
#define PLUG_IS_INST 0

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 0

#define PLUG_DOES_STATE_CHUNKS 0



// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "www.olilarkin.co.uk"
#define MFR_EMAIL "spam@me.com"
#define EFFECT_TYPE_VST3 "Fx"
