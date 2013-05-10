#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
	ofSetFrameRate(30);
    ofBackground(0);
    ofHideCursor();
    
	totalFramesRendered = 0;
    alignMode = false;
    loadedSuccess = false;
    render = false;
    fontSize = 35;
    titlePosition = .85;

    ofxXmlSettings localSettings;
    if(localSettings.loadFile("localsettings.xml")){
        localSettings.pushTag("settings");
        renderer.shift.x = localSettings.getValue("xshift", 0.0);
        renderer.shift.y = localSettings.getValue("yshift", 0.0);
        
        int numScreens = localSettings.getNumTags("screenRect");
        cout << "x/y shift " << renderer.shift << endl;
        cout << "num screens " << numScreens << endl;
        
		twoScreens = (numScreens > 1);
        localSettings.pushTag("screenRect", 0);
        leftRect.x = localSettings.getValue("x",0);
        leftRect.y = localSettings.getValue("y",0);
        leftRect.width = localSettings.getValue("w",10);
        leftRect.height = localSettings.getValue("h",10);

        localSettings.popTag();
        cout << "screen one " << leftRect.x << " " << leftRect.y << " " << leftRect.width << " " << leftRect.height << endl;
		if(twoScreens){
            localSettings.pushTag("screenRect",1);
            rightRect.x = localSettings.getValue("x",0);
            rightRect.y = localSettings.getValue("y",0);
            rightRect.width = localSettings.getValue("w",10);
            rightRect.height = localSettings.getValue("h",10);
            
            localSettings.popTag();            
            ofRectangle unionRect = leftRect.getUnion(rightRect);
            cout << "screen two " << rightRect.x << " " << rightRect.y << " " << rightRect.width << " " << rightRect.height <<  endl;
            cout << " union " << unionRect.x << " " << unionRect.y << " " << unionRect.width << " " << unionRect.height << endl;
            fbo.allocate(unionRect.width, unionRect.height, GL_RGB, 4);
        }
        else{
            fbo.allocate(leftRect.width, leftRect.height, GL_RGB, 4);
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
        lineWidth = localSettings.getValue("lineWidth",1.);
        pointSize = localSettings.getValue("pointSize",2.);
        fontSize = localSettings.getValue("fontSize",35);;
        titlePosition = localSettings.getValue("titlePosition", .85);

    	int receiverPort = localSettings.getValue("receiverPort", 1200);
//        receiver.setup(1200);
        
        cameraPositionDirectory = localSettings.getValue("cameraAngles", "");
        cout << "camera angles " << cameraPositionDirectory << endl;
    }
    else{
        ofLogError("testApp -- error loading settings -- check xml file");
		return;
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
            }
            else if(type == Far){
                compositionMediaBin = portraits.getValue("far:mediaFolder","");
            }
            else if(type == Close){
                compositionMediaBin = portraits.getValue("close:mediaFolder","");
            }
            
			string soundFile = portraits.getValue("soundFile", "");

            
            newPortrait.rendererRef = &renderer; //must be set before setup()
            newPortrait.setup(type, portraitMediaBin+compositionMediaBin, soundDirectory+soundFile);
            
            newPortrait.name = portraits.getValue("name", "noname");

            newPortrait.englishTitles.setup("subtitles/spectacle_" + newPortrait.name + "_edited_english.srt", "subtitles/AxisStd-Regular.otf", fontSize);
            newPortrait.japaneseTitles.setup("subtitles/spectacle_" + newPortrait.name + "_edited_japanese.srt", "subtitles/AxisStd-Regular.otf", fontSize);
            cout << "Loaded " << newPortrait.englishTitles.getNumTitles() << " for name " << newPortrait.name << endl;
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
    currentMiddle = 0;
    if(!twoScreens){
    	currentRight = -1; //disable this index    
    }

    composeMode = false;
    
    currentPortrait = -1;
    gotoNextPortrait();
    
	leftCam.setup();
    leftCam.setScale(1,-1,1);
    leftCam.usemouse = true;
    leftCam.autosavePosition = true;
    leftCam.speed = 10;
    leftCam.loadCameraPosition();

    rightCam.setup();
    rightCam.setScale(1,-1,1);
    rightCam.usemouse = true;
    rightCam.autosavePosition = true;
    rightCam.speed = 10;
    rightCam.loadCameraPosition();

    normalLeftCam.setScale(1,-1,1);
    normalRightCam.setScale(1,-1,1);
    
	checkSwitchCamera(true);
    
    glEnable(GL_DEPTH_TEST);
    loadedSuccess = true;
	ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::gotoNextPortrait(){
    if(currentPortrait != -1){
    	allPortraits[currentPortrait].stop();
    }
	currentPortrait = (currentPortrait + 1) % allPortraits.size();
	allPortraits[currentPortrait].resetAndPlay();

    track.loadFromFile(cameraTrackFile);
    
    cameraTrackFile = ofToDataPath(cameraPositionDirectory + allPortraits[currentPortrait].name + "_CameraTrack.xml", true);
    cout << "loading camera track " << cameraTrackFile << endl;
    track.loadFromFile(cameraTrackFile);
    cout << "Playing portrait " << currentPortrait << " with " << 	allPortraits[currentPortrait].videoPlayer.getTotalNumFrames() << endl;
    
    ofxXmlSettings xyshiftFile;
    xyshiftFile.loadFile(allPortraits[currentPortrait].scene.xyshiftFile);
    renderer.shift.x = xyshiftFile.getValue("xshift", 0.);
    renderer.shift.y = xyshiftFile.getValue("yshift", 0.);

}

//--------------------------------------------------------------
void testApp::gotoPortrait(string name){
	for(int i = 0; i < allPortraits.size(); i++){
    	if(allPortraits[i].name == name){
            if(currentPortrait != -1){
                allPortraits[i].stop();
            }
            currentPortrait = i;
            allPortraits[i].resetAndPlay();
            if(render){
                allPortraits[i].startRender();
                totalFramesRendered = 0;
            }
            return;
        }
    }
    ofLogError("couldn't find name " + name);
}

//--------------------------------------------------------------
void testApp::update(){
	if(!loadedSuccess) return;
   // cout << allPortraits[currentPortrait].soundPlayer.getPosition() << endl;
    
    if(alignMode){
        renderer.shift.x = ofMap(ofGetMouseX(), 0, ofGetWidth(),  -.2, .2, true);
        renderer.shift.y = ofMap(ofGetMouseY(), 0, ofGetHeight(), -.2, .2, true);
    }
    
    /*
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        
        if(m.getAddress() == "/person"){
            gotoPortrait(m.getArgAsString(0));
        }
    }
    */
    
    if(!composeMode && !allPortraits[currentPortrait].soundPlayer.isPlaying()){
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
    
	if(!loadedSuccess){
		ofBackground(255,0,0);
		return;
	}
        
    fbo.begin();
    ofClear(0);
    
    glEnable(GL_DEPTH_TEST);
    if(composeMode){
        ofClear(0);
        leftCam.begin(leftRect);
        drawFunc();
        leftCam.end();
        if(twoScreens){
            rightCam.begin(rightRect);
            drawFunc();
            rightCam.end();
        }
    }
    else{
        ofClear(0);
        normalLeftCam.begin(leftRect);
		drawFunc();
        normalLeftCam.end();
     	if(twoScreens){   
            normalRightCam.begin(rightRect);
            drawFunc();
            normalRightCam.end();        
        }
    }
	glDisable(GL_DEPTH_TEST);

    ofPushStyle();
    
    ofPoint titlePointLeft = ofVec2f(leftRect.getCenter().x, leftRect.getHeight()*titlePosition);
    ofPoint titlePointRight = ofVec2f(rightRect.getCenter().x, rightRect.getHeight()*titlePosition);
    ofSetColor(0);
    allPortraits[currentPortrait].englishTitles.draw(titlePointLeft);
    allPortraits[currentPortrait].japaneseTitles.draw(titlePointRight);
    ofSetColor(255);
    allPortraits[currentPortrait].englishTitles.draw(titlePointLeft+ofVec2f(-2,-2));
    allPortraits[currentPortrait].japaneseTitles.draw(titlePointRight+ofVec2f(-2,-2));
    
    ofPopStyle();
    
    fbo.end();

    
    fbo.draw(0,0);
    
    ofDrawBitmapString("Next Cut Left " + ofToString( currentCameraFramesLeft - (ofGetFrameNum() - lastCameraChangeFrameLeft)), 
                       rightRect.x + rightRect.width + 10, 10);
    if(twoScreens){
        ofDrawBitmapString("Next Cut Right " + ofToString( currentCameraFramesRight - (ofGetFrameNum() - lastCameraChangeFrameRight)), 
                           rightRect.x + rightRect.width + 10, 28);
    }

    if(render){
        char filename[1024];
        sprintf(filename, "%s/frame_%05d.png", allPortraits[currentPortrait].renderFolder.c_str(), totalFramesRendered++);
        fbo.getTextureReference().readToPixels(savePixels.getPixelsRef());
		savePixels.saveImage(filename);
    }

}

void testApp::drawFunc(){
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    glEnable(GL_POINT_SMOOTH); // makes circular points
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
        float ratio = leftRect.width/leftRect.height;
        float newHeight = ofGetHeight();
        float newWidth = ofGetHeight() * ratio;
        leftRect = ofRectangle(0,0, newWidth, newHeight);
        rightRect = ofRectangle(newWidth,0, newWidth, newHeight);
    }
    
    if(key == 'C'){
        composeMode = !composeMode;
        if(composeMode){
            ofShowCursor();
        }
        else {
            ofHideCursor();
        }
    }
    
    if(key == 'R'){
        track.camera = leftCam.applyTranslation ? &leftCam : &rightCam;
        track.addSample();
        track.writeToFile(cameraTrackFile);
    }
    if(key == 'P'){
        checkSwitchCamera(true);
    }
    
    if(key == 'N'){
        gotoNextPortrait();
    }

	if(key == 'f'){
		ofToggleFullscreen();
	}
    
    if(key == 'A'){
        alignMode = !alignMode;
        if(!alignMode){
            ofxXmlSettings xyshiftFile;
            xyshiftFile.loadFile(allPortraits[currentPortrait].scene.xyshiftFile);
            xyshiftFile.setValue("xshift", renderer.shift.x);
            xyshiftFile.setValue("yshift", renderer.shift.y);
            xyshiftFile.saveFile();
        }
    }
    
    if(key == 'S'){
        render = true;
        allPortraits[currentPortrait].startRender();
    }
}

//--------------------------------------------------------------
void testApp::checkSwitchCamera(bool force) {
    
//    cout << "attempting switch camera" << endl;
    if(!force && composeMode){
        return;
    }
	
    //if(force || currentCameraDurationLeft < ofGetElapsedTimef() - lastCameraChangeTimeLeft){
	if(force || currentCameraFramesLeft < ofGetFrameNum() - lastCameraChangeFrameLeft){
        lastCameraChangeFrameLeft = ofGetFrameNum();
        //currentCameraDurationLeft = ofRandom(5, 40); //ofRandom(10, 40);
        currentCameraFramesLeft = ofRandom(2*15, 30*15);
        if(track.getSamples().size() > 1){
        	track.camera = &normalLeftCam;
            int sample;
            int tries = 0;
            do{
                sample = ofRandom(0, track.getSamples().size());
            } while( (sample == currentRight || sample == currentLeft) && tries++ < 10);
        	currentLeft = sample;
//			cout << "LEFT sampling camera at frame " << (sample+1) << " of " << track.getSamples().size() << endl;
            track.moveCameraToFrame(sample);            
        }
    }

    //if(force || currentCameraDurationRight < ofGetElapsedTimef() - lastCameraChangeTimeRight){
    if(force || currentCameraFramesRight < ofGetFrameNum() - lastCameraChangeFrameRight){
        lastCameraChangeFrameRight = ofGetFrameNum();
        //currentCameraDurationRight = ofRandom(5, 15); //ofRandom(10, 40);
        currentCameraFramesRight = ofRandom(2*15, 30*15); //ofRandom(10, 40);
        if(track.getSamples().size() > 1){
            track.camera = &normalRightCam;
            int sample;
            int tries = 0;
            do {
                sample = ofRandom(0, track.getSamples().size());                
            } while((sample == currentLeft || sample == currentRight) && tries++ < 10);
            currentRight = sample;
//            cout << "RIGHT sampling camera at frame " << (sample+1) << " of " << track.getSamples().size() << endl;
            track.moveCameraToFrame(sample);        
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
//    cout << "mouse is " << x << " " << y << " " << leftRect.inside(x,y) << " " << rightRect.inside(x,y) << endl;
	leftCam.usemouse  = leftCam.applyTranslation  = composeMode && leftRect.inside(x,y);
    rightCam.usemouse = rightCam.applyTranslation = composeMode && rightRect.inside(x,y);
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