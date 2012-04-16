#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

    ofSetFrameRate(60);
    
    receiver.setup(1200);
    
    viewReceived = false;
    projectionReceived = false;
    anyMessageReceived = false;
    
    cam.setup();
    cam.usemouse = true;
    cam.speed = 2;
    cam.autosavePosition = true;
//    cam.loadCameraPosition();
    
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
    
	while(receiver.hasWaitingMessages()){
        anyMessageReceived = true;
        
        ofxOscMessage m;
        receiver.getNextMessage(&m);
//        cout << "message " << m.getAddress() << " num args " << m.getNumArgs() << endl;
//        for(int i = 0; i < m.getNumArgs(); i++){
//        	cout << "	arg " << i << m.getArgAsFloat(i) << endl;
//        }

        if(m.getAddress() == "/view" && m.getNumArgs() == 16){
            ofMatrix4x4 mat;
        	for(int i = 0; i < 16; i++){
                mat.getPtr()[i] = m.getArgAsFloat(i);
            }
            view.setTransformMatrix(mat);
            viewReceived = true;

        }
        else if(m.getAddress() == "/projection" && m.getNumArgs() == 16){
        	for(int i = 0; i < 16; i++){
                viewMatrix.getPtr()[i] = m.getArgAsFloat(i);
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
    cam.usemouse = leftHalf.inside(mouseX, mouseY);
   
    
    cam.begin(leftHalf);
    drawScene();
    cam.end();
    
    if(projectionReceived && viewReceived){
        ofPushView();
        ofViewport(rightHalf);
        glPushMatrix();

//        glMultMatrixf(viewMatrix.getPtr());

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
        if(!projectionReceived){
            ofDrawBitmapString( "Waiting for projection", ofPoint(leftHalf.width+30,60) );        
        }
        if(!viewReceived){
            ofDrawBitmapString( "Waiting for view", ofPoint(leftHalf.width+30,90) );                	
        }
        if(anyMessageReceived){
        	ofDrawBitmapString( "We got something at least...", ofPoint(leftHalf.width+30,120) );                	
        }
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void testApp::drawScene(){
    glEnable(GL_DEPTH);
    for(int i = 0; i < nodes.size(); i++){
    	nodes[i].draw();	
    }
    view.draw();
    ofDrawGrid();
    glDisable(GL_DEPTH);
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