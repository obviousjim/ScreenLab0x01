#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(60);
    
    receiver.setup(12000);
    
    viewReceived = false;
    projectionReceived = false;
    
//    generateNodes();
    
    cam.setup();
    cam.usemouse = true;
    cam.speed = 2;
    cam.autosavePosition = true;
    cam.loadCameraPosition();
    
}

//--------------------------------------------------------------
void testApp::generateNodes(){
    
    nodes.clear();
    for(int i = 0; i < 200; i++){
        ofNode n;
        n.setPosition(ofRandom(-200,200), ofRandom(-200,200), ofRandom(-200,200));
    	nodes.push_back(n);
    }
}

//--------------------------------------------------------------
void testApp::update(){
	if(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        if(m.getAddress() == "/view"){
            ofMatrix4x4 mat;
        	for(int i = 0; i < 16; i++){
                mat.getPtr()[i] = m.getArgAsFloat(i);
            }
            view.setTransformMatrix(mat);
            viewReceived = true;
        }
        else if(m.getAddress() == "/projection"){
        	for(int i = 0; i < 16; i++){
            	receivedMat[i] = m.getArgAsFloat(i);
            }
            projectionReceived = true;
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    
	ofRectangle leftHalf = ofRectangle(0,0, ofGetWidth()/2, ofGetHeight());
    ofRectangle rightHalf = ofRectangle(ofGetWidth()/2,0, ofGetWidth()/2, ofGetHeight());
    
    cam.begin(leftHalf);
    drawScene();
    cam.end();
    
    if(projectionReceived && viewReceived){
        ofPushView();
        ofViewport(rightHalf);
        glPushMatrix();
        glMultMatrixf(receivedMat);

        drawScene();
        
        glPopMatrix();
        ofPopView();        
    }
    else{
        ofPushStyle();
        ofSetColor(100, 0, 0);
        ofRect(rightHalf);
        ofSetColor(255);
        ofDrawBitmapString( "Waiting for message", ofPoint(leftHalf.width+30,30) );
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void testApp::drawScene(){
    for(int i = 0; i < nodes.size(); i++){
    	nodes[i].draw();	
    }
    ofDrawGrid();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
        generateNodes();
    }
    
    if(key == 'f'){
    	ofToggleFullscreen();
    }
    
    if(key == 'k'){
    	nodes.clear();
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