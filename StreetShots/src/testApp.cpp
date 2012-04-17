#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetFrameRate(60);
    ofBackground(0);
    
	scrubMode = false;
    if(!playlist.loadFile("playlist.xml")){
        ofLogError("Error loading playlist xml");
    }
    string takesfolder = playlist.getValue("takesfolder", "");
    cout << "takes folder " << takesfolder << endl;
    int numTakes = playlist.getNumTags("take");
    for(int i = 0; i < numTakes; i++){
        StreetTake take;
        take.xmlIndex = i;
        playlist.pushTag("take", i);
        take.path = ofToDataPath(takesfolder + "/" + playlist.getValue("folder", ""));
        take.images.loadSequence( take.path );        
        take.intime  = playlist.getValue("intime", 0);
		take.outtime = playlist.getValue("outtime", int(take.images.getDurationInMillis()));
        take.soundFile = playlist.getValue("soundFile", "");

        playlist.popTag();
        sequences.push_back(take);
    }
    currentTake = 0;

    cam.setup();
    cam.usemouse = true;
    cam.autosavePosition;
    cam.loadCameraPosition();
	cam.speed = 5;
    
    startTime = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void testApp::update(){
	if(scrubMode){
        long time = ofMap(mouseX, 0, ofGetWidth(), 0, sequences[currentTake].images.getDurationInMillis(), true);
        sequences[currentTake].images.selectTime(time);
    }
}

//--------------------------------------------------------------
void testApp::draw(){
	drawPointcloud();
    if(scrubMode){
        ofSetColor(255);
        ofDrawBitmapString("Current Path " + sequences[currentTake].path, 50, 50);
    }
    else{
        long currentMillis = ofGetElapsedTimeMillis() - startTime + sequences[currentTake].intime;
        if(currentMillis < sequences[currentTake].outtime){
	        sequences[currentTake].images.selectTime(currentMillis);    
        }
        else {
            nextTake();
        }
    }
}

void testApp::drawPointcloud(){

    ofShortPixels& pix = sequences[currentTake].images.getPixels();
    
	glEnable(GL_DEPTH_TEST);
	ofMesh mesh;
	ofRectangle rect = ofRectangle(0,0, ofGetWidth(), ofGetHeight());
    //glEnable(GL_POINT_SMOOTH);
    //glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
    glPointSize(2);

	for(int y = 0; y < 480; y++){
		for(int x = 0; x < 640; x++){
			//0.104200 ref dist 120.000000
			double ref_pix_size = 0.104200;
			double ref_distance = 120.000000;
			double wz = pix.getPixels()[y*640+x];
			double factor = 2 * ref_pix_size * wz / ref_distance;
			double wx = (double)(x - 640/2) * factor;
			double wy = (double)(y - 480/2) * factor;
            mesh.addVertex(ofVec3f(wx,-wy,-wz));
		}
	}
    
    cam.begin(rect);
	mesh.drawVertices();
	cam.end();
    
	glDisable(GL_DEPTH_TEST);	

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
        scrubMode = !scrubMode;
        if(!scrubMode){
            startTime = ofGetElapsedTimeMillis();
        }
    }
    if(scrubMode && key == '['){
        long time = ofMap(mouseX, 0, ofGetWidth(), 0, sequences[currentTake].images.getDurationInMillis(), true);
        playlist.pushTag("take",sequences[currentTake].xmlIndex);
        playlist.setValue("intime", int(time));
        playlist.popTag();
        playlist.saveFile();
        cout << "set in to " << time << endl;
    }
    if(scrubMode && key == ']'){
    	long time = ofMap(mouseX, 0, ofGetWidth(), 0, sequences[currentTake].images.getDurationInMillis(), true);
        playlist.pushTag("take",sequences[currentTake].xmlIndex);
        playlist.setValue("outtime", int(time));
        playlist.popTag();
        playlist.saveFile();
        
        cout << "set out to " << time << endl;
    }
    
    if(scrubMode && key == OF_KEY_LEFT){
        nextTake();
    }
    if(scrubMode && key == OF_KEY_RIGHT){
        previousTake();
    }

    if(key == 'f'){
        ofToggleFullscreen();
    }
}

void testApp::nextTake(){
    currentTake = (currentTake + 1) % sequences.size();   
}

void testApp::previousTake(){
    currentTake--;
    if(currentTake < 0){
        currentTake = sequences.size()-1;
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