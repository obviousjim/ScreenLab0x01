//
//  ScreenLabPortrait.cpp
//  ScreenLabRenderer
//
//  Created by James George on 4/16/12.
//

#include "ScreenLabPortrait.h"
#include "ofxXmlSettings.h"

ScreenLabPortrait::ScreenLabPortrait(){
//	startFrame = 0;
//    endFrame = -1;
    rendererRef = NULL;
}

void ScreenLabPortrait::setup(PortraitType _type, string mediaFolder, string soundPath){
    type = _type;
	soundPlayer.loadMovie(soundPath);
    soundPlayer.setLoopState(OF_LOOP_NONE);
    cout << "media folder " << mediaFolder << " " << soundPath << endl;
    if(scene.loadFromFolder(mediaFolder)){
        videoPlayer.loadMovie(scene.videoPath);
//		rendererRef->setTextureScale(640./1920, 360./1080);
        //videoPlayer.loadMovie(take.hiResVideoPath);
        depthImages.loadSequence(scene.depthFolder);
        pairing.loadPairingFile(scene.pairingsFile);
        if(!pairing.ready()){
            ofLogError("ScreenLabPortrait -- Pairings not ready!");
        }
        
        videoTimes = pairing.getStartAndEndTimes(videoPlayer, depthImages);
        
//        scene.populateRenderSettings();
//        if(take.getRenderSettings().size() == 0){
//            startFrame = 0;
//            endFrame = videoPlayer.getTotalNumFrames();
//            ofLogError("ScreenLabPortrait -- No Render Settings!");
//        }
//        else{
//            startFrame = take.getRenderSettings()[0].startFrame;
//            endFrame   = take.getRenderSettings()[0].endFrame;
//            cout << "found start and end frame " << startFrame << " " << endFrame << endl;
//        }
    }
    else{
        ofLogError("ScreenLabPortrait -- Couldn't load media folder " + mediaFolder);
    }
    filler.enable = true;
    filler.setIterations(3);
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
    videoPlayer.setPosition(videoTimes.min / videoPlayer.getDuration() );
    videoPlayer.setVolume(0);
    videoPlayer.play();
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    
    rendererRef->setup(scene.calibrationFolder);
    rendererRef->setRGBTexture(videoPlayer);
	rendererRef->setDepthImage(depthImages.getPixels());
//    if(take.getRenderSettings().size() != 0){
    	//take.getRenderSettings()[0].applyToRenderer(*rendererRef);
//    }
    rendererRef->farClip = 1200;
    if(name == "jenny"){
        rendererRef->farClip = 925;
    }
    else if(name == "lisa"){
    	rendererRef->farClip = 1050;
    }
    
    if(name == "kev"){
        rendererRef->xshift = 0.0140476;
        rendererRef->yshift = 0.0293333;
    }
    else{
		rendererRef->xshift = 0;
        rendererRef->yshift = 0.0436667;
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
        if(videoPlayer.getPosition() >= videoTimes.max / videoPlayer.getDuration()){
            videoPlayer.setFrame(videoTimes.min / videoPlayer.getDuration() );
        }
		else {
            long time = pairing.getDepthFrameForVideoFrame(videoPlayer.getPosition() * videoPlayer.getDuration() * 1000);
            depthImages.selectTime( time );
			//ofShortPixels& pix = depthImages.getPixels();
            //filler.close(pix);
            filler.close(depthImages.getPixels());
//            rendererRef->setDepthImage(pix);
            rendererRef->update();
        }        	
    }
}
