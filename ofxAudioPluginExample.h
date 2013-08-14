#ifndef __ofxAudioPluginExample__
#define __ofxAudioPluginExample__

#include "IPlug_include_in_plug_hdr.h"
#include "ofMain.h"
#include "testApp.h"
testApp *ofxAudioEffectMain(ofAppBaseWindow *window, IPlugBase *iplug);

#ifdef OS_OSX
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>

void ofGLReadyCallback();

class IOpenGLTestControl : public IControl, public ofAppBaseWindow {
private:
  
  // 
  CGLContextObj mGLContext;
  // pixel data
  WDL_TypedBuf<unsigned char> mData;
  int width;
  int height;
  ofFbo fbo;
  ofPixels pix;
  
public:
  void initIPlug(IGraphics *g) {
    g->HandleMouseOver(true);
    /*g->Resize(width, height);
    mRECT.L = 0;
    mRECT.T = 0;
    mRECT.R = width;
    mRECT.B = height;
    mData.Resize(mRECT.W() * mRECT.H() * 4);*/
  }
  
  
  //------------------------------------------------------------
  void setupOpenGL(int w, int h, int screenMode){
    setContext();
    ofGLReadyCallback();
    restoreContext();
    width = w;
    height = h;
  }
  

  
  //--------------------------------------------
  void runAppViaInfiniteLoop(ofBaseApp * appPtr){
    app = appPtr;

    
    ofNotifySetup();
    ofNotifyUpdate();
    
    // doesn't actually run an infinite loop
  }

  
  
  
  
  
  
  
  ofBaseApp *app;
  
  bool bEnableSetupScreen;
  IOpenGLTestControl(IPlugBase* pPlug, IRECT pR):	IControl(pPlug, pR, -1) {
    app = NULL;
    createContext();
    mData.Resize(mRECT.W() * mRECT.H() * 4);
        bEnableSetupScreen = true;
  }
  
  ~IOpenGLTestControl()
  {
    destroyContext();
  }
  
  long setContext()
  {
    // Set the current context
    
    if(CGLSetCurrentContext(mGLContext))
    {
      DBGMSG("Could not make context current.");
      return 1;
    }
    
    return 0;
  }
  
  void restoreContext()
  {
    CGLSetCurrentContext(NULL);
  }
  
  long createContext()
  {
    const GLubyte *glstring;
    
    GLint npix;
    CGLPixelFormatObj PixelFormat;
    
    const CGLPixelFormatAttribute attributes[] =
    {
      //kCGLPFAOffScreen,
      //      kCGLPFAColorSize, (CGLPixelFormatAttribute)8,
      //      kCGLPFADepthSize, (CGLPixelFormatAttribute)16,
      kCGLPFAAccelerated, (CGLPixelFormatAttribute)0
    };
    
    // Create context if none exists
    
    CGLChoosePixelFormat(attributes, &PixelFormat, &npix);
    
    if (PixelFormat == NULL)
    {
      DBGMSG("Could not get pixel format.");
      return 1;
    }
    
    CGLCreateContext(PixelFormat, NULL, &mGLContext);
    
    if (mGLContext == NULL)
    {
      DBGMSG("Could not create rendering context.");
      return 1;
    }
    
    // Set the current context
    
    if(setContext())
      return 1;
    
    // Check OpenGL functionality:
    glstring = glGetString(GL_EXTENSIONS);
    
    if(!gluCheckExtension((const GLubyte *)"GL_EXT_framebuffer_object", glstring))
    {
      DBGMSG("The GL_EXT_framebuffer_object extension is not supported on this system.");
      return 1;
    }
    
    restoreContext();
    
    return 0;
  }
  

  void OnMouseDown(int x, int y, IMouseMod* pMod) {
    ofNotifyMousePressed(x, y, pMod->L?0:1);
  }
  void OnMouseUp(int x, int y, IMouseMod* pMod) {
    ofNotifyMouseReleased(x, y, pMod->L?0:1);
  }
  
  void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod) {
    ofNotifyMouseDragged(x, y, pMod->L?0:1);
  }
  // For efficiency, mouseovers/mouseouts are ignored unless you call IGraphics::HandleMouseOver.
  void OnMouseOver(int x, int y, IMouseMod* pMod) {
    ofNotifyMouseMoved(x, y) ;
  }
  
  void destroyContext()
  {
    if (mGLContext != NULL)
      CGLDestroyContext(mGLContext);
  }
  
  int getWidth() {
    return width;
  }
  int getHeight() {
    return height;
  }
  ofPoint	getWindowSize(){
    return ofPoint(width, height);
  }
  
  void	enableSetupScreen(){
    bEnableSetupScreen = true;
  }
	void	disableSetupScreen(){
    bEnableSetupScreen = false;
  }

  
  
  bool Draw(IGraphics* pGraphics)
  {
  
    width = mRECT.W();
    height = mRECT.H();

    // Set context
    if (setContext())
      return false;
    
    
    fbo.allocate(width, height);
    pix.allocate(width, height, 4); // do we need this bit?
    fbo.begin();
  //  ofClear(255, 0, 0, 255);
    ofClear(0, 0, 0, 0);
//    app->draw();
    ofNotifyDraw();
    fbo.end();
    fbo.readToPixels(pix);
    restoreContext();
    
    unsigned char* val = pix.getPixels();//mData.Get();
    
    for(int v=0; v<height; v++)
    {
      for(int h=0; h<width; h++)
      {
        LICE_pixel pix = LICE_RGBA(*val, *(val + 1), *(val + 2), *(val + 3));
        LICE_PutPixel(pGraphics->GetDrawBitmap(), mRECT.L + h, mRECT.T + v, pix, 1., LICE_BLIT_MODE_COPY | LICE_BLIT_USE_ALPHA);
        val += 4;
      }
    }
    ofNotifyUpdate();
    return true;
  }
  
  bool IsDirty()
  {
    return true;
  }
  
};
#endif
class ofxAudioPluginExample : public IPlug
{
public:
  ofxBaseAudioPlugin *app;
  ofxAudioPluginExample(IPlugInstanceInfo instanceInfo);
  ~ofxAudioPluginExample();
  
  void Reset();
  void OnParamChange(int paramIdx);
  
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
  vector<float> ins, outs;
private:
  
  double mGain;
};

#endif
