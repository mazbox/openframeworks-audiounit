/**
 *  main.cpp
 *
 *  Created by Marek Bereza on 11/08/2013.
 */

#include "ofMain.h"
#include "testApp.h"
#include "resource.h"
#include "IPlugBase.h"
static void boggienoopDeleter(ofAppBaseWindow*){}
static void bogienoopDeleter(ofBaseApp*){}

testApp *ofxAudioEffectMain(ofAppBaseWindow *window, IPlugBase *iplug) {
	
	
	testApp *app = new testApp;
	app->iplug = iplug;
	ofPtr<ofAppBaseWindow> win(window, std::ptr_fun(boggienoopDeleter));
	ofPtr<ofBaseApp> appy(app, std::ptr_fun(bogienoopDeleter));
	ofPtr<ofBaseRenderer> renderer(new ofGLRenderer(false));
	
	//ofSetWindowPtr(win);
	ofSetupOpenGL(win, GUI_WIDTH, GUI_HEIGHT, OF_WINDOW);

	
	ofRunApp(appy);
	return app;
}