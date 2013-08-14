#include "ofxAudioPluginExample.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

IOpenGLTestControl *openglControl = NULL;
const int kNumPrograms = 1;



enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,
  
  kGainX = 100,
  kGainY = 100,
  kKnobFrames = 60
};

ofxAudioPluginExample::ofxAudioPluginExample(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(NUM_PARAMS, kNumPrograms, instanceInfo), mGain(1.)
{
  TRACE;
  

  
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight, 30);
  

  
  pGraphics->AttachPanelBackground(&COLOR_BLACK);

  openglControl = new IOpenGLTestControl(this, IRECT(0,0, GUI_WIDTH, GUI_HEIGHT));
  openglControl->initIPlug(pGraphics);
  app = (ofxBaseAudioPlugin*)ofxAudioEffectMain(openglControl, this);
  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(0)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");
  GetParam(0)->SetShape(2.);
  
  

  pGraphics->AttachControl(openglControl);

  
  AttachGraphics(pGraphics);
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

ofxAudioPluginExample::~ofxAudioPluginExample() {}

void ofxAudioPluginExample::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  
  // Mutex is already locked for us.
  if(mGain>1) mGain = 1;
  else if(mGain<0) mGain = 0;

  //double samplesPerBeat = GetSamplesPerBeat();
  //double samplePos = (double) GetSamplePos();
  //double tempo = GetTempo();

  int numInputs = NInChannels();
  int numOutputs = NOutChannels();
  
  // whooops!
  ofxBaseAudioPlugin * plug = (ofxBaseAudioPlugin*)app;
  
  // resize buffers
  if(ins.size()!=numInputs*nFrames) {
    ins.resize(numInputs * nFrames);
  }
  
  if(outs.size()!=numOutputs*nFrames) {
    outs.resize(numOutputs * nFrames);
  }
  
  // interleave
  for(int ch = 0; ch < numInputs; ch++) {
    for(int i = 0; i < nFrames; i++) {
      ins[ch + i*numInputs] = inputs[ch][i];
    }
  }
  
  
  plug->processAudio(&ins[0], &outs[0], nFrames, numInputs, numOutputs);
  

  for(int c = 0; c < numOutputs; c++) {
    for(int i = 0; i < nFrames; i++) {
      outputs[c][i] = outs[c + i*numOutputs];
    }
  }
  
}

void ofxAudioPluginExample::Reset()
{
  TRACE;
  IMutexLock lock(this);
  
  //double sr = GetSampleRate();
}

void ofxAudioPluginExample::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);
  
  switch (paramIdx)
  {
    case 0:
      mGain = GetParam(0)->Value() / 100.;;
      break;
      
    default:
      break;
  }
}