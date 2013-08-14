/**
 *  ofxBaseAudioPlugin.cpp
 *
 *  Created by Marek Bereza on 12/08/2013.
 */

#include "ofxBaseAudioPlugin.h"
int paramPos = 0;
void ofxBaseAudioPlugin::addParameter(string name, bool &value, string group) {
	iplug->GetParam(paramPos++)->InitBool(name.c_str(), value, "", group.c_str());
}


void ofxBaseAudioPlugin::addParameter(string name, float &value, float min, float max, float step, string unit, string group) {
	iplug->GetParam(paramPos++)->InitDouble(name.c_str(), value, min, max, step, unit.c_str(), group.c_str());
}

void ofxBaseAudioPlugin::addParameter(string name, int &value, int min, int max, string unit, string group) {
	iplug->GetParam(paramPos++)->InitInt(name.c_str(), value, min, max, unit.c_str(), group.c_str());
}

void ofxBaseAudioPlugin::addParameter(string name, int &value, int numEnums, string group) {
	iplug->GetParam(paramPos++)->InitEnum(name.c_str(), value, numEnums, "", group.c_str());
}