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
    
    showTitle = false; //are we showing atitle
    showTitleLeft = false; //are we showing it on the left (or right?)
    titlesRemaining = 0; // how many titles do we have left to show
    showEnglish = false; //are we showing english (or japanese)

}

void ScreenLabPortrait::setup(PortraitType _type, string mediaFolder, string soundPath){
    type = _type;
	soundPlayer.loadMovie(soundPath);
    soundPlayer.setLoopState(OF_LOOP_NONE);

    if(scene.loadFromFolder(mediaFolder)){
        videoPlayer.loadMovie(scene.videoPath);
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
    
    soundPlayer.setVolume(1.0);
    soundPlayer.setPosition(0);
    soundPlayer.play();
	soundPlayer.setLoopState(OF_LOOP_NONE);
    
    cout << "setting up renderer " << endl;
    cout << "sound player duration " << soundPlayer.getDuration() << endl;
    
    if(useHighResPlayer){
        if(!hiResPlayer.isLoaded()){
            hiResPlayer.loadMovie(scene.alternativeHiResVideoPath);
        }
        hiResPlayer.setSpeed(.5);
        hiResPlayer.setPosition(videoTimes.min / videoPlayer.getDuration() );
        hiResPlayer.setVolume(0);
        hiResPlayer.play();
        hiResPlayer.setLoopState(OF_LOOP_NORMAL);
        rendererRef->setRGBTexture(hiResPlayer);
    }
    else {
        videoPlayer.setSpeed(.5);
        videoPlayer.setPosition(videoTimes.min / videoPlayer.getDuration() );
        videoPlayer.setVolume(0);
        videoPlayer.play();
        videoPlayer.setLoopState(OF_LOOP_NORMAL);
    }

    rendererRef->setup(scene.calibrationFolder);
    rendererRef->setRGBTexture(videoPlayer);
	rendererRef->setDepthImage(depthImages.getPixels());
    
    rendererRef->farClip = 1200;
    if(name == "jenny"){
        rendererRef->farClip = 925;
    }
    else if(name == "lisa"){
    	rendererRef->farClip = 1050;
    }
    
    rendererRef->setSimplification(2);
	ofAddListener(ofEvents().update, this, &ScreenLabPortrait::update);
}

void ScreenLabPortrait::startRender(){
    hiResPlayer.setSpeed(0);
    render = true;
}

void ScreenLabPortrait::drawTitles(int x, int y){
    englishTitles.setTimeInSeconds(soundPlayer.getPosition() * soundPlayer.getDuration());
    if(englishTitles.isTitleNew()){
        if(showTitle){
            titlesRemaining--;
            if(titlesRemaining <= 0){
                showTitle = false;
            }
        }
        else{
            showTitle = ofRandomuf() > .5;
            if(showTitle){
                titlesRemaining = 2;
                showEnglish = ofRandomuf() > .5;
                showTitleLeft = ofRandomuf() > .5;
            }
        }
        cout << "NEW TITLE " << endl;
    }
    if(showEnglish){
	    englishTitles.draw(x,y);
    }
    else{
        japaneseTitles.draw(x, y);
    }
}

void ScreenLabPortrait::stop(){
	ofRemoveListener(ofEvents().update, this, &ScreenLabPortrait::update);
    
	videoPlayer.stop();
    hiResPlayer.stop();
    soundPlayer.stop();
}

void ScreenLabPortrait::update(ofEventArgs& args){
    if(!render && (soundPlayer.getPosition() == 1.0 || lastTime > soundPlayer.getPosition()) ){ 
        stop();
    }
    
    englishTitles.setTimeInSeconds(soundPlayer.getPosition()*soundPlayer.getDuration());
    japaneseTitles.setTimeInSeconds(soundPlayer.getPosition()*soundPlayer.getDuration());
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
