#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){

	ofSetFrameRate(30);
    ofBackground(0);
    
    ofxXmlSettings localSettings;
    if(localSettings.loadFile("localsettings.xml")){
        localSettings.pushTag("settings");
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
            portraits.pushTag("portrait");
            string compositionMediaBin;
			if(type == Studio){
                compositionMediaBin = portraits.getValue("studio:mediaFolder","");
                newPortrait.startFrame = portraits.getValue("studio:startFrame",0);
                newPortrait.endFrame = portraits.getValue("studio:endFrame",0);
            }
            else if(type == Far){
                compositionMediaBin = portraits.getValue("far:mediaFolder","");
                newPortrait.startFrame = portraits.getValue("far:startFrame",0);
                newPortrait.endFrame = portraits.getValue("far:endFrame",0);
            }
            else if(type == Close){
                compositionMediaBin = portraits.getValue("close:mediaFolder","");
                newPortrait.startFrame = portraits.getValue("close:startFrame",0);
                newPortrait.endFrame = portraits.getValue("close:endFrame",0);
            }
            
			string soundFile = portraits.getValue("soundFile", "");
            newPortrait.rendererRef = &renderer; //must be set before setup()
            newPortrait.setup(type, portraitMediaBin+compositionMediaBin, soundDirectory+soundFile);
            portraits.popTag();//portrait
            
            allPortraits.push_back( newPortrait );
        }
        portraits.popTag(); //portraits
    }
    
    currentPortrait = -1;
    gotoNextPortrait();

//	for(int i = 0; i < 200; i++){
//		ofNode n;
//		n.setPosition(ofRandom(-1000,1000),ofRandom(-1000,1000),ofRandom(-1000,1000) );
//		debugNodes.push_back( n );
//	}
    
	cam.setup();
    cam.usemouse = true;
    cam.autosavePosition = true;
    cam.speed = 10;
    cam.loadCameraPosition();
    
}

//--------------------------------------------------------------
void testApp::gotoNextPortrait(){
    if(currentPortrait != -1){
    	allPortraits[currentPortrait].stop();
    }
	currentPortrait = (currentPortrait + 1) % allPortraits.size();
	allPortraits[currentPortrait].resetAndPlay();

    cout << "Playing portrait " << currentPortrait << " with " << 	allPortraits[currentPortrait].videoPlayer.getTotalNumFrames() << endl;    
}

//--------------------------------------------------------------
void testApp::update(){
    //cout << allPortraits[currentPortrait].soundPlayer.getPosition() << endl;
    if(allPortraits[currentPortrait].soundPlayer.getPosition() == 1.0 ){
        gotoNextPortrait();
    }
    
	if(type == Studio){
    	//do master stuff
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
	cam.begin();
    //TODO draw with render settings
    renderer.drawWireFrame();
	ofDrawGrid(100.0f, 5.0f, true);
	for(int i = 0; i < debugNodes.size(); i++){
		debugNodes[i].draw();
	}
    cam.end();

	ofDrawBitmapString("of framerate " + ofToString(ofGetFrameRate()), 30, 30 );
	//allPortraits[currentPortrait].videoPlayer.draw(0,0, 640,360);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == 'j'){
        allPortraits[currentPortrait].soundPlayer.setPosition(.95);
    }
	if(key == ' '){
		renderer.reloadShader();
	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

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