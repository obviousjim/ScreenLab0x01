#pragma once

#include "ofMain.h"
#include "ofxDepthImageSequence.h"
#include "ofxGrabCam.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

typedef struct {
	ofxDepthImageSequence images;
    long intime;
    long outtime;
    string soundFile;
    int xmlIndex;
    string path;
} StreetTake;

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

    void nextTake();
    void previousTake();
    
    bool scrubMode;
    void drawPointcloud();
    
    //vector<ofxDepthImageSequence> sequences;
    vector<StreetTake> sequences;
    int currentTake;
    ofSoundPlayer player;
    ofxXmlSettings playlist;
    long startTime;

	ofxOscReceiver rx;	
	ofMatrix4x4 view;
	ofMatrix4x4 projection;

    ofxGrabCam cam;

	bool debugView;
};
