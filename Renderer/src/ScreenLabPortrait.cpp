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
	soundPlayer.loadMovie(soundPath);
    soundPlayer.setLoopState(OF_LOOP_NONE);
    cout << "media folder " << mediaFolder << " " << soundPath << endl;
    if(take.loadFromFolder(mediaFolder)){
        videoPlayer.loadMovie(take.lowResVideoPath);
//		rendererRef->setTextureScale(640./1920, 360./1080);
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
    filler.enable = true;
    filler.setIterations(2);
    filler.setKernelSize(3);

}

void ScreenLabPortrait::resetAndPlay(){
    lastTime = 0;
    
    soundPlayer.setVolume(1300);
    soundPlayer.setPosition(0);
    soundPlayer.play();
	soundPlayer.setLoopState(OF_LOOP_NONE);
    
    cout << "sound player duration " << soundPlayer.getDuration() << endl;
    
    videoPlayer.setSpeed(.5);
    videoPlayer.setFrame(startFrame);
    videoPlayer.setVolume(0);
    videoPlayer.play();
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    
    rendererRef->setup(take.calibrationDirectory);
    rendererRef->setRGBTexture(videoPlayer);
//    if(take.getRenderSettings().size() != 0){
    	//take.getRenderSettings()[0].applyToRenderer(*rendererRef);
//    }
    rendererRef->farClip = 1200;
    if(name == "jenny"){
        rendererRef->farClip = 925;
    }
    rendererRef->setSimplification(2);
	ofAddListener(ofEvents().update, this, &ScreenLabPortrait::update);
}

void ScreenLabPortrait::stop(){
	videoPlayer.stop();
    soundPlayer.stop();
    ofRemoveListener(ofEvents().update, this, &ScreenLabPortrait::update);
}

void ScreenLabPortrait::update(ofEventArgs& args){
    if(soundPlayer.getPosition() == 1.0 || lastTime > soundPlayer.getPosition()){ //looops?
        stop();
    }
    lastTime = soundPlayer.getPosition();
    
    videoPlayer.update();
    if(videoPlayer.isFrameNew()){
        if(videoPlayer.getCurrentFrame() >= endFrame){
            videoPlayer.setFrame(startFrame);
        }
		else {
            long time = pairing.getDepthFrameForVideoFrame(videoPlayer.getPosition() * videoPlayer.getDuration() * 1000);
            depthImages.selectTime( time );
			ofShortPixels& pix = depthImages.getPixels();
            filler.close(pix);
            rendererRef->setDepthImage(pix);
            rendererRef->update();
        }        	
    }
}
