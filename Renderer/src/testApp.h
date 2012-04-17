#pragma once

#include "ofMain.h"
#include "ofxRGBDRenderer.h"
#include "ScreenLabPortrait.h"
#include "ofxGameCamera.h"
#include "ofxXmlSettings.h"
#include "ofxCameraTrack.h"

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
    
    ofRectangle leftRect;
    ofRectangle rightRect;
    ofxCameraTrack track;
    
    vector<ofNode> debugNodes;
    
    bool composeMode;
  	void checkSwitchCamera(bool force = false);
    
};
