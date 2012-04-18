#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
    
	cam.setFarClip(10000.0f);
    ofSetFrameRate(60);
    ofBackground(0);
       
	ofToggleFullscreen();
	//scrubMode = true;
	scrubMode = false;
	//debugView = true;
	debugView = false;

	rx.setup(1200);
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
		//take.outtime = take.images.getDurationInMillis();
		cout << take.intime << " " << take.outtime << " " << take.images.getDurationInMillis() << endl;
        take.soundFile = playlist.getValue("soundFile", "");

        playlist.popTag();
        sequences.push_back(take);
    }
    currentTake = 0;

    startTime = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------

void loadMatrix(ofxOscMessage & msg, ofMatrix4x4 & matrix)
{
	for(int i = 0; i < 16; i++){
		matrix.getPtr()[i] = msg.getArgAsFloat(i);
	}
}

void testApp::update(){

	while(rx.hasWaitingMessages()){
		ofxOscMessage msg;
		rx.getNextMessage(&msg);

		if(msg.getAddress() == "/view")
		{
			loadMatrix(msg, view);
		}
		if(msg.getAddress() == "/projection")
		{
			loadMatrix(msg, projection);			
		}
	}

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
		//cout << "start time " << startTime << " millis " << currentMillis << " " << sequences[currentTake].images.getDurationInMillis() << endl;
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
    

	if (debugView) {
		cam.begin(rect);
	} else {
		ofPushView();
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(projection.getPtr());
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(view.getPtr());
	}

	glEnable(GL_DEPTH_FUNC);
	ofPushMatrix();
	ofTranslate(2.0f, 1.0f, 0.0f);
	ofScale(0.001f, 0.001f, 0.001f);
	mesh.drawVertices();

	ofPopMatrix();
	if (debugView){
		
		ofPushStyle();
		ofSetColor(100,50,50);
		ofDrawGrid(10,5,true);
		ofSetColor(255);
		ofPushMatrix();
		glMultMatrixf((view * projection).getInverse().getPtr());
		ofNoFill();
		ofBox(2.0f);
		ofPopMatrix();
		ofPopStyle();
	}
	glDisable(GL_DEPTH_FUNC);

	if (debugView)
		cam.end();
	else
		ofPopView();
    
	glDisable(GL_DEPTH_TEST);	

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == 'd')
		debugView ^= true;

	if(key == ' '){
        scrubMode = !scrubMode;
        if(!scrubMode){
            startTime = ofGetElapsedTimeMillis();
        }
    }
    if(scrubMode && key == '['){
        long time = ofMap(mouseX, 0, ofGetWidth(), 0, sequences[currentTake].images.getDurationInMillis(), true);
   		sequences[currentTake].intime = time;

		playlist.pushTag("take",sequences[currentTake].xmlIndex);
        playlist.setValue("intime", int(time));
        playlist.popTag();
        playlist.saveFile();
        cout << "set in to " << time << endl;
    }
    if(scrubMode && key == ']'){
    	long time = ofMap(mouseX, 0, ofGetWidth(), 0, sequences[currentTake].images.getDurationInMillis(), true);
		sequences[currentTake].outtime = time;
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
	cout << currentTake << " switched  " << endl;
	startTime = ofGetElapsedTimeMillis();
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