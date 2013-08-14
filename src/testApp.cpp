

#include "testApp.h"

void log(string l) {
	ofstream myfile;
	myfile.open ("/Users/marek/Desktop/log.txt", ios::app| ios::out);
	myfile << l << "\n";
	myfile.close();
}

//ofVideoGrabber video;
float gain = 0;
float freq = 500;
bool bypass = false;

//--------------------------------------------------------------
void testApp::setup(){
//	video.initGrabber(640, 480);
	addParameter("Gain", gain, 0, 100);
	addParameter("Bypass", bypass);
	addParameter("Frequency", freq, 100, 10000, 0.1, "Hz");
}

//--------------------------------------------------------------
void testApp::update(){
//	video.update();
}

//--------------------------------------------------------------
void testApp::draw(){
//	video.draw(0, 0, 300, 300);
	
	ofSetColor(ofRandom(255), ofRandom(255), ofRandom(255));
	ofRect(0, 0, ofGetWidth(), ofGetHeight()/2);
	ofSetColor(255);
	ofDrawBitmapString(ofToString(ofGetFrameRate(), 0) + " fps", 20, 20);
	ofCircle(mouseX, mouseY, 10);
}


float mx =  0;
void testApp::processAudio(float *ins, float *outs, int length, int inChannels, int outChannels) {
	for(int i = 0; i < outChannels * length; i++) {
		outs[i] = ofMap(i, 0, length, -0.3, 0.3, true);
	}
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	mx = ofMap(x, 0, ofGetWidth(), 0, 1, true);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
