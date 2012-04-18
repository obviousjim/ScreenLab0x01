#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofSetFrameRate(60);
    ofBackground(0);
    ofToggleFullscreen();
    
//    leftRect = ofRectangle(0,0, 720, 2160);
//    rightRect = ofRectangle(720,0, 720, 2160) ;
    
    ofxXmlSettings localSettings;
    if(localSettings.loadFile("localsettings.xml")){
        localSettings.pushTag("settings");
        int numScreens = localSettings.getNumTags("screenRect");
        cout << "num screens " << numScreens << endl;
		twoScreens = (numScreens == 2);

        localSettings.pushTag("screenRect");
        leftRect.x = localSettings.getValue("x",0);
        leftRect.y = localSettings.getValue("y",0);
        leftRect.width = localSettings.getValue("w",10);
        leftRect.height = localSettings.getValue("h",10);
        localSettings.popTag();
        cout << "screen one " << leftRect.width << " " << leftRect.height << endl;
		if(twoScreens){
            localSettings.pushTag("screenRect",1);
            rightRect.x = localSettings.getValue("x",0);
            rightRect.y = localSettings.getValue("y",0);
            rightRect.width = localSettings.getValue("w",10);
            rightRect.height = localSettings.getValue("h",10);
            localSettings.popTag();            
            cout << "screen one " << rightRect.width << " " << rightRect.height << endl;
        }
        
        soundDirectory = localSettings.getValue("soundDirectory", "");
        portraitMediaBin = localSettings.getValue("portraitMediaBin", "");
        string typeString = localSettings.getValue("screenType", "");
		if(typeString == "STUDIO"){
        	type = Studio;
        }
        else if(typeString == "CLOSE"){
        	type = Close;
        }
        else {
            type = Far;
        }
        lineWidth = localSettings.getValue("lineWidth",1) ;
        pointSize = localSettings.getValue("pointSize",2) ;
        cout << "line width " << lineWidth << " " << pointSize << endl;
        ofLogNotice("TYPE IS " + typeString);
    }
    else{
        ofLogError("testApp -- couldn't load settings");
    }
    
    ofxXmlSettings portraits;
    if(portraits.loadFile("portraits.xml")){
        portraits.pushTag("portraits");
    	int numPortraits = portraits.getNumTags("portrait");
        for(int i = 0; i < numPortraits; i++){
            ScreenLabPortrait newPortrait;
            portraits.pushTag("portrait", i);
            string compositionMediaBin;
			if(type == Studio){
                compositionMediaBin = portraits.getValue("studio:mediaFolder","");
                newPortrait.startFrame = portraits.getValue("studio:startFrame",0);
                newPortrait.endFrame = portraits.getValue("studio:endFrame",0);
            }
            else if(type == Far){
                compositionMediaBin = portraits.getValue("far:mediaFolder","");
                newPortrait.startFrame = portraits.getValue("far:startFrame",-1);
                newPortrait.endFrame = portraits.getValue("far:endFrame",0);
            }
            else if(type == Close){
                compositionMediaBin = portraits.getValue("close:mediaFolder","");
                newPortrait.startFrame = portraits.getValue("close:startFrame",0);
                newPortrait.endFrame = portraits.getValue("close:endFrame",0);
            }
            
			string soundFile = portraits.getValue("soundFile", "");
            cout << "loading sound file " << soundFile << endl;

            newPortrait.rendererRef = &renderer; //must be set before setup()
            newPortrait.setup(type, portraitMediaBin+compositionMediaBin, soundDirectory+soundFile);
            portraits.popTag();//portrait
            
            allPortraits.push_back( newPortrait );
        }
        portraits.popTag(); //portraits
        
    }
    else{
        ofLogError("Couldn't Load XML File ");
    }
    

    currentLeft = 0;
    currentRight = 0;

    composeMode = false;
    
    currentPortrait = -1;
    gotoNextPortrait();

//	for(int i = 0; i < 200; i++){
//		ofNode n;
//		n.setPosition(ofRandom(-1000,1000),ofRandom(-1000,1000),ofRandom(-1000,1000) );
//		debugNodes.push_back( n );
//	}
    
	leftCam.setup();
    leftCam.usemouse = true;
    leftCam.autosavePosition = true;
    leftCam.speed = 10;
    leftCam.loadCameraPosition();

    rightCam.setup();
    rightCam.usemouse = true;
    rightCam.autosavePosition = true;
    rightCam.speed = 10;
    rightCam.loadCameraPosition();

	checkSwitchCamera(true);
    
    glEnable(GL_DEPTH_TEST);
    
//    lastCameraChangeTimeRight = ofGetElapsedTimef();
//    currentCameraDurationRight = ofRandom(20, 50); //ofRandom(10, 40);
//    lastCameraChangeTimeLeft = ofGetElapsedTimef();
//    currentCameraDurationLeft = ofRandom(20, 50); //ofRandom(10, 40);

}

//--------------------------------------------------------------
void testApp::gotoNextPortrait(){
    if(currentPortrait != -1){
    	allPortraits[currentPortrait].stop();
    }
	currentPortrait = (currentPortrait + 1) % allPortraits.size();
	allPortraits[currentPortrait].resetAndPlay();

    cameraTrackFile = ofToDataPath(allPortraits[currentPortrait].take.mediaFolder + "/SalfordTracks.xml");
    cout << "loading camera track " << cameraTrackFile << endl;
    track.loadFromFile(cameraTrackFile);

    cout << "Playing portrait " << currentPortrait << " with " << 	allPortraits[currentPortrait].videoPlayer.getTotalNumFrames() << endl;    
}

//--------------------------------------------------------------
void testApp::update(){
    //cout << allPortraits[currentPortrait].soundPlayer.getPosition() << endl;
    if(!allPortraits[currentPortrait].soundPlayer.isPlaying()){
        gotoNextPortrait();
    }
    
    //cout << "allPortraits[currentPortrait].soundPlayer.getPosition() == 1.0 " << allPortraits[currentPortrait].soundPlayer.getPosition() << endl;
	if(type == Studio){
    	//do master stuff
    }
    
    checkSwitchCamera();
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofPushStyle();
    ofSetColor(255, 0, 0);
    ofPopStyle();
    
    if(composeMode){
        leftCam.begin(leftRect);
        drawFunc();
        leftCam.end();
        
        rightCam.begin(rightRect);
        drawFunc();
        rightCam.end();
    }
    else{
        normalLeftCam.begin(leftRect);
		drawFunc();
        normalLeftCam.end();
     	if(twoScreens){   
            normalRightCam.begin(rightRect);
            drawFunc();
            normalRightCam.end();        
        }
    }
    
//	ofDrawBitmapString("of framerate " + ofToString(ofGetFrameRate()), 30, 30 );
	//allPortraits[currentPortrait].videoPlayer.draw(0,0, 640,360);
}

void testApp::drawFunc(){
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    glEnable(GL_POINT_SMOOTH); // makes circular points
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
//    glDisable(GL_DEPTH_TEST);		        
    glPointSize( pointSize );
    renderer.drawPointCloud();
    glLineWidth( lineWidth );
    renderer.drawWireFrame();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'j'){
        allPortraits[currentPortrait].soundPlayer.setPosition(.98);
    }
	
    if(key == ' '){
		renderer.reloadShader();
	}
    
	if(key == 'v'){
        float ratio = 720/2160.;
        float newHeight = ofGetHeight();
        float newWidth = ofGetWidth() * ratio;
        leftRect = ofRectangle(0,0, newWidth, newHeight);
        rightRect = ofRectangle(newWidth,0, newWidth, newHeight);
    }
    
    if(key == 'C'){
        composeMode = !composeMode;
    }
    
    if(key == 'R'){
        track.camera = leftCam.applyTranslation ? &leftCam : &rightCam;
		track.sample(track.getSamples().size()); //add a sample
        track.writeToFile(cameraTrackFile);
    }
    if(key == 'P'){
        checkSwitchCamera(true);
    }
    
    if(key == 'N'){
        gotoNextPortrait();
    }
}

//--------------------------------------------------------------
void testApp::checkSwitchCamera(bool force) {
    
//    cout << "attempting switch camera" << endl;
    if(!force && composeMode){
        return;
    }
	
    if(force || currentCameraDurationLeft < ofGetElapsedTimef() - lastCameraChangeTimeLeft){

        lastCameraChangeTimeLeft = ofGetElapsedTimef();
        currentCameraDurationLeft = ofRandom(5, 40); //ofRandom(10, 40);
        if(track.getSamples().size() > 1){
        	track.camera = &normalLeftCam;
            int sample;
            int tries = 0;
            do{
                sample = ofRandom(0, track.getSamples().size());
            } while(sample == currentRight && tries++ < 10);
        	currentRight = sample;
            track.moveCameraToFrame(sample);
            cout << "LEFT sampling camera at frame " << sample << endl;
        }
    }
    if(force || currentCameraDurationRight < ofGetElapsedTimef() - lastCameraChangeTimeRight){
        lastCameraChangeTimeRight = ofGetElapsedTimef();
        currentCameraDurationRight = ofRandom(5, 15); //ofRandom(10, 40);
        if(track.getSamples().size() > 1){
        	track.camera = &normalRightCam;
            int sample;
            int tries = 0;
            do{
                sample = ofRandom(0, track.getSamples().size());                
            } while(sample == currentLeft && tries++ < 10);
            currentLeft = sample;
            cout << "RIGHT sampling camera at frame " << sample << endl;
            track.moveCameraToFrame(sample);        
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	leftCam.usemouse = composeMode && leftRect.inside(x,y); 
    leftCam.applyTranslation = composeMode && leftRect.inside(x,y);
    rightCam.usemouse =  composeMode && rightRect.inside(x,y);
    rightCam.applyTranslation =  composeMode && rightRect.inside(x,y);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}