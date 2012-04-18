//
//  ScreenLabPortrait.cpp
//  ScreenLabRenderer
//
//  Created by James George on 4/16/12.
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
	//soundPlayer.loadMovie(soundPath);
    //soundPlayer.setLoopState(OF_LOOP_NONE);
    
    if(take.loadFromFolder(mediaFolder)){
        videoPlayer.loadMovie(take.lowResVideoPath);
		rendererRef->setTextureScale(640./1920, 360./1080);
        //videoPlayer.loadMovie(take.hiResVideoPath);
        depthImages.loadSequence(take.depthFolder);
        pairing.loadPairingFile(take.pairingsFile);
        if(!pairing.ready()){
            ofLogError("ScreenLabPortrait -- Pairings not ready!");
        }
        
        take.populateRenderSettings();
        if(take.getRenderSettings().size() == 0){
            startFrame = 0;
            endFrame = videoPlayer.getTotalNumFrames();
            ofLogError("ScreenLabPortrait -- No Render Settings!");
        }
        else{
            startFrame = take.getRenderSettings()[0].startFrame;
            endFrame   = take.getRenderSettings()[0].endFrame;
            cout << "found start and end frame " << startFrame << " " << endFrame << endl;
        }
    }
    else{
        ofLogError("ScreenLabPortrait -- Couldn't load media folder " + mediaFolder);
    }
}

void ScreenLabPortrait::resetAndPlay(){
    
    soundPlayer.setVolume(1300);
    soundPlayer.setPosition(0);
    soundPlayer.play();
	
    cout << "sound player duration " << soundPlayer.getDuration() << endl;
    
    //videoPlayer.setSpeed(.33);
    videoPlayer.setFrame(startFrame);
    videoPlayer.setVolume(0);
    videoPlayer.play();
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    
    rendererRef->setup(take.calibrationDirectory);
    rendererRef->setRGBTexture(videoPlayer);
    if(take.getRenderSettings().size() != 0){
    	take.getRenderSettings()[0].applyToRenderer(*rendererRef);
    }
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
