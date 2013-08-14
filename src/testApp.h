#pragma once

#include "ofMain.h"
#include "ofxBaseAudioPlugin.h"

class testApp : public ofxBaseAudioPlugin {
public:

	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void processAudio(float *ins, float *outs, int length, int inChannels, int outChannels);
};
