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

            cout << "Composition folder" << compositionMediaBin << endl;
            
			string soundFile = portraits.getValue("soundFile", "");
            newPortrait.setup(type, portraitMediaBin+compositionMediaBin, soundDirectory+soundFile);
            newPortrait.rendererRef = &renderer;
            portraits.popTag();//portrait
            
            allPortraits.push_back( newPortrait );
        }
        portraits.popTag(); //portraits
    }
    
    currentPortrait = -1;
    gotoNextPortrait();

    cam.setup();
    cam.usemouse = true;
    cam.autosavePosition = true;
    cam.speed = 10;
    cam.loadCameraPosition();
    
    for(int i = 0; i < 200; i++){
        ofNode n;
        n.setPosition(ofRandom(-2000,2000), ofRandom(-2000,2000), ofRandom(-2000,2000));
		debugNodes.push_back( n );
    }

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
	if(type == Studio){
    	//do master stuff

    }
}

//--------------------------------------------------------------
void testApp::draw(){
    
	cam.begin();
    for(int i = 0; i < debugNodes.size(); i++){
        debugNodes[i].draw();
    }
            
    renderer.drawPointCloud();
    renderer.drawWireFrame();
    cam.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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