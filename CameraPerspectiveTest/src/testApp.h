#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGameCamera.h"
class testApp : public ofBaseApp{
  public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    ofxGameCamera cam;

    bool anyMessageReceived;
    bool viewReceived;
    bool projectionReceived;
    
    vector<ofNode> nodes;
    ofNode view;
    ofMatrix4x4 viewMatrix;
    ofMatrix4x4 projectionMatrix;
    ofxOscReceiver receiver;
	    
    void generateNodes();
    void drawScene();
};
