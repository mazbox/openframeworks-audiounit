/**     ___           ___           ___                         ___           ___     
 *     /__/\         /  /\         /  /\         _____         /  /\         /__/|    
 *    |  |::\       /  /::\       /  /::|       /  /::\       /  /::\       |  |:|    
 *    |  |:|:\     /  /:/\:\     /  /:/:|      /  /:/\:\     /  /:/\:\      |  |:|    
 *  __|__|:|\:\   /  /:/~/::\   /  /:/|:|__   /  /:/~/::\   /  /:/  \:\   __|__|:|    
 * /__/::::| \:\ /__/:/ /:/\:\ /__/:/ |:| /\ /__/:/ /:/\:| /__/:/ \__\:\ /__/::::\____
 * \  \:\~~\__\/ \  \:\/:/__\/ \__\/  |:|/:/ \  \:\/:/~/:/ \  \:\ /  /:/    ~\~~\::::/
 *  \  \:\        \  \::/          |  |:/:/   \  \::/ /:/   \  \:\  /:/      |~~|:|~~ 
 *   \  \:\        \  \:\          |  |::/     \  \:\/:/     \  \:\/:/       |  |:|   
 *    \  \:\        \  \:\         |  |:/       \  \::/       \  \::/        |  |:|   
 *     \__\/         \__\/         |__|/         \__\/         \__\/         |__|/   
 *
 *  Description: 
 *				 
 *  ofxBaseAudioPlugin.h, created by Marek Bereza on 12/08/2013.
 */

#pragma once
#include "ofMain.h"
#include "IPlugBase.h"

class ofxBaseAudioPlugin: public ofBaseApp {
public:
	
	virtual void midiMessageReceived() {} // TODO
	
	virtual void processAudio(float *ins, float *outs, int length, int inChannels, int outChannels) = 0;
	
	
	void addParameter(string name, bool &value, string group = "");
	void addParameter(string name, float &value, float min, float max, float step = 0.01, string unit = "", string group = "");
	void addParameter(string name, int &value, int min, int max, string unit = "", string group = "");
	void addParameter(string name, int &value, int numEnums, string group = "");
	
	
	IPlugBase *iplug;
};