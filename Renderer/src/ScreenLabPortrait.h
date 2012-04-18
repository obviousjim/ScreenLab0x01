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
#include "ofxRGBDMediaTake.h"
#include "ofxDepthImageSequence.h"
#include "ofxRGBDRenderSettings.h"
#include "ofxDepthHoleFiller.h"

typedef enum  {
	Studio,
    Far,
    Close
} PortraitType;

class ScreenLabPortrait {
  public:
	ScreenLabPortrait();
    void setup(PortraitType type, string mediaFolder, string soundPath);
    
    void resetAndPlay();
    void stop();
	void update(ofEventArgs& args);

    int startFrame;
    int endFrame;
    float lastTime;
    
    ofxRGBDRenderer* rendererRef;
    ofxRGBDMediaTake take;
    ofxDepthImageSequence depthImages;
    ofxRGBDVideoDepthSequence pairing;
    
    PortraitType type;
  	ofVideoPlayer videoPlayer;
	ofVideoPlayer soundPlayer;
    ofxDepthHoleFiller filler;
};