//
//  ScreenLabPortrait.cpp
//  ScreenLabRenderer
//
//  Created by James George on 4/16/12.
//

#include "ScreenLabPortrait.h"
#include "ofxXmlSettings.h"

ScreenLabPortrait::ScreenLabPortrait(){
    rendererRef = NULL;
    useHighResPlayer = false;
    render = false;
}

void ScreenLabPortrait::setup(PortraitType _type, string mediaFolder, string soundPath){
    type = _type;
    cout << "Setting up portrait " << endl;
    
	soundPlayer.loadMovie(soundPath);
    soundPlayer.setLoopState(OF_LOOP_NONE);
    cout << "media folder " << mediaFolder << " " << soundPath << endl;
//<<<<<<< HEAD
//    if(take.loadFromFolder(mediaFolder)){
//        cout << "Low res video is " << take.lowResVideoPath << endl;
//        videoPlayer.loadMovie(take.lowResVideoPath);
//=======
    if(scene.loadFromFolder(mediaFolder)){
        videoPlayer.loadMovie(scene.videoPath);
//>>>>>>> 7785d622a173e710bd358ffb900bc4ebd357bed5
//		rendererRef->setTextureScale(640./1920, 360./1080);
        //videoPlayer.loadMovie(take.hiResVideoPath);
        depthImages.loadSequence(scene.depthFolder);
        pairing.loadPairingFile(scene.pairingsFile);
        if(!pairing.ready()){
            ofLogError("ScreenLabPortrait -- Pairings not ready!");
        }
        
        renderFolder = mediaFolder + "/renders";
        if(!ofDirectory(renderFolder).exists()){
            ofDirectory(renderFolder).create();
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
    
    cout << "setting up sound player" << endl;
    
    soundPlayer.setVolume(1300);
    soundPlayer.setPosition(0);
    soundPlayer.play();
	soundPlayer.setLoopState(OF_LOOP_NONE);
    
//    rendererRef->setup(take.calibrationDirectory);
    
    cout << "setting up renderer " << endl;
    cout << "sound player duration " << soundPlayer.getDuration() << endl;
    
//<<<<<<< HEAD
    if(useHighResPlayer){
        if(!hiResPlayer.isLoaded()){
            hiResPlayer.loadMovie(scene.alternativeHiResVideoPath);
        }
        hiResPlayer.setSpeed(.5);
        //videoPlayer.setFrame(startFrame);
        hiResPlayer.setPosition(videoTimes.min / videoPlayer.getDuration() );
        hiResPlayer.setVolume(0);
        hiResPlayer.play();
        hiResPlayer.setLoopState(OF_LOOP_NORMAL);
//        rendererRef->setRGBTexture(hiResPlayer);
    }
    else {
        videoPlayer.setSpeed(.5);
        //videoPlayer.setFrame(startFrame);
        videoPlayer.setPosition(videoTimes.min / videoPlayer.getDuration() );
        videoPlayer.setVolume(0);
        videoPlayer.play();
        videoPlayer.setLoopState(OF_LOOP_NORMAL);
//        rendererRef->setRGBTexture(videoPlayer);
    }
    
//=======
//    videoPlayer.setSpeed(.5);
//    videoPlayer.setPosition(videoTimes.min / videoPlayer.getDuration() );
//    videoPlayer.setVolume(0);
//    videoPlayer.play();
//    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    cout << "calibration folder is " << scene.calibrationFolder << endl;
    rendererRef->setup(scene.calibrationFolder);
    rendererRef->setRGBTexture(videoPlayer);
//>>>>>>> 7785d622a173e710bd358ffb900bc4ebd357bed5
	rendererRef->setDepthImage(depthImages.getPixels());
    rendererRef->farClip = 1200;
    if(name == "jenny"){
        rendererRef->farClip = 925;
    }
    else if(name == "lisa"){
    	rendererRef->farClip = 1050;
    }
    
    if(name == "kev"){
//<<<<<<< HEAD
//        rendererRef->xmult = 0.0104762;
//        rendererRef->ymult = 0.0274286;

        rendererRef->xshift = 0.0140476;
        rendererRef->yshift = 0.0293333;
//>>>>>>> 7785d622a173e710bd358ffb900bc4ebd357bed5
    }
    else{
		rendererRef->xshift = 0;
        rendererRef->yshift = 0.0436667;
    }
    rendererRef->setSimplification(2);
	ofAddListener(ofEvents().update, this, &ScreenLabPortrait::update);
}

void ScreenLabPortrait::startRender(){
    hiResPlayer.setSpeed(0);
    render = true;
}

void ScreenLabPortrait::stop(){
    cout << "stopping portraits " << endl;
	videoPlayer.stop();
    hiResPlayer.stop();
    soundPlayer.stop();
    ofRemoveListener(ofEvents().update, this, &ScreenLabPortrait::update);
    cout << "portrait stopped" << endl;
}

void ScreenLabPortrait::update(ofEventArgs& args){
    if(!render && (soundPlayer.getPosition() == 1.0 || lastTime > soundPlayer.getPosition()) ){ 
        stop();
    }
    lastTime = soundPlayer.getPosition();
    ofVideoPlayer& player = useHighResPlayer ? hiResPlayer : videoPlayer;
    if(render){
        player.setFrame(player.getCurrentFrame()+1);
    }
    player.update();
    if(player.isFrameNew()){
        if(player.getPosition() >= videoTimes.max / player.getDuration()){
            player.setPosition(videoTimes.min / player.getDuration() );
        }
		else {
            long time = pairing.getDepthFrameForVideoFrame(player.getPosition() * player.getDuration() * 1000);
            depthImages.selectTime( time );
            filler.close(depthImages.getPixels());
            rendererRef->update();
        }        	
    }
}
