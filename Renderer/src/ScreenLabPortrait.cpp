//
//  ScreenLabPortrait.cpp
//  ScreenLabRenderer
//
//  Created by James George on 4/16/12.
//  Copyright (c) 2012 FlightPhase. All rights reserved.
//

#include "ScreenLabPortrait.h"
#include "ofxXmlSettings.h"

ScreenLabPortrait::ScreenLabPortrait(){
	startFrame = 0;
    endFrame = -1;
    rendererRef = false;
}

void ScreenLabPortrait::setup(PortraitType _type, string mediaFolder, string soundPath){
    type = _type;
	soundPlayer.loadSound(soundPath);
    
    //TODO: Load from comp directory
    if(take.loadFromFolder(mediaFolder)){
        videoPlayer.loadMovie(take.lowResVideoPath);
        depthImages.loadSequence(take.depthFolder);
        pairing.loadPairingFile(take.pairingsFile);
        if(!pairing.ready()){
            ofLogError("ScreenLabPortrait -- Pairings not ready!");
        }
    }
    else{
        ofLogError("ScreenLabPortrait -- Couldn't load media folder " + mediaFolder);
    }
}

void ScreenLabPortrait::resetAndPlay(){
    videoPlayer.setFrame(startFrame);
    videoPlayer.setVolume(0);
    videoPlayer.play();
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    rendererRef->setup(take.calibrationDirectory);
    rendererRef->setRGBTexture(videoPlayer);
    soundPlayer.setPosition(0);
    soundPlayer.play();
	ofAddListener(ofEvents().update, this, &ScreenLabPortrait::update);
}

void ScreenLabPortrait::stop(){
	videoPlayer.stop();
    soundPlayer.stop();
    ofRemoveListener(ofEvents().update, this, &ScreenLabPortrait::update);
}

void ScreenLabPortrait::update(ofEventArgs& args){
    videoPlayer.update();
    if(videoPlayer.isFrameNew()){
        if(videoPlayer.getCurrentFrame() >= endFrame){
            videoPlayer.setFrame(startFrame);
        }
		else {
            long time = pairing.getDepthFrameForVideoFrame(videoPlayer.getPosition() * videoPlayer.getDuration() * 1000);
            depthImages.selectTime( time );
            rendererRef->setDepthImage(depthImages.getPixels());
            rendererRef->update();
        }        	
    }
}
