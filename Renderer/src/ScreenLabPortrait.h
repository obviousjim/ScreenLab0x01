//
//  ScreenLabPortrait.cpp
//  ScreenLabRenderer
//
//  Created by James George on 4/16/12.
//

#pragma once

#include "ofMain.h"
#include "ofxRGBDVideoDepthSequence.h"
#include "ofxRGBDRenderer.h"
#include "ofxRGBDScene.h"
#include "ofxDepthImageSequence.h"
#include "ofxDepthHoleFiller.h"
//#include "ofxOsc.h"

typedef enum  {
	Studio,
    Far,
    Close
} PortraitType;

class ScreenLabPortrait {
  public:
	ScreenLabPortrait();
    void setup(PortraitType type, string mediaFolder, string soundPath);
    
    string name; //used for osc
    void resetAndPlay();
    void stop();
	void update(ofEventArgs& args);

    float lastTime;
    
    ofRange videoTimes;
    ofxRGBDRenderer* rendererRef;
    ofxRGBDScene scene;
    ofxDepthImageSequence depthImages;
    ofxRGBDVideoDepthSequence pairing;
    
    bool useHighResPlayer;
    PortraitType type;
  	ofVideoPlayer videoPlayer;
    ofVideoPlayer hiResPlayer;
	ofVideoPlayer soundPlayer;
    ofxDepthHoleFiller filler;
  
    string renderFolder;
    void startRender();
    bool render;
};