#pragma once

#include "ofMain.h"
#include "ofxRGBDRenderer.h"
#include "ScreenLabPortrait.h"
#include "ofxGameCamera.h"
#include "ofxXmlSettings.h"
#include "ofxCameraTrack.h"
//#include "ofxOsc.h"
#include "ofxGui.h"

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
    
    void gotoNextPortrait();
    void gotoPortrait(string name);
    void drawFunc();
    
    float lastCameraChangeTimeLeft;
    float currentCameraDurationLeft;

    float lastCameraChangeTimeRight;
    float currentCameraDurationRight;

    vector<ScreenLabPortrait> allPortraits;
    int currentPortrait;
    
    ofxRGBDRenderer renderer;
    
    string soundDirectory;
    string portraitMediaBin;
    string cameraTrackFile;
    
    PortraitType type;
    
    ofxGameCamera leftCam;
    ofxGameCamera rightCam;
    
    ofCamera normalLeftCam;
    ofCamera normalRightCam;
    
    bool twoScreens;
    ofRectangle leftRect;
    ofRectangle rightRect;
    float lineWidth;
    float pointSize;
    int currentLeft;
    int currentRight;
    
    ofxCameraTrack track;
    
    vector<ofNode> debugNodes;
    bool loadedSuccess;
    bool composeMode;
  	void checkSwitchCamera(bool force = false);

    ofxPanel panel;
	ofxToggle renderOn;
    
//    ofxOscReceiver receiver;

    bool alignMode;
    ofFbo leftFbo;
    ofFbo rightFbo;
    

};
