#pragma once

#include "ofMain.h"
#include "ofxRGBDRenderer.h"
#include "ScreenLabPortrait.h"
#include "ofxGameCamera.h"
#include "ofxXmlSettings.h"
#include "ofxCameraTrack.h"
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
    
    int lastCameraChangeFrameLeft;
    int currentCameraFramesLeft;
    
    int lastCameraChangeFrameRight;
    int currentCameraFramesRight;
    
    int lastCameraChangeFrameMiddle;
    int currentCameraFramesMiddle;
    
    vector<ScreenLabPortrait> allPortraits;
    int currentPortrait;
    
    ofxRGBDRenderer renderer;
    
    string soundDirectory;
    string portraitMediaBin;
    string cameraTrackFile;
    string middleCameraTrackFile;
    
    PortraitType type;
    
    ofxGameCamera leftCam;
    ofxGameCamera rightCam;
    ofxGameCamera middleCam;

    ofCamera normalLeftCam;
    ofCamera normalRightCam;
    ofCamera normalMiddleCam;
    
    ofxCameraTrack track;
    ofxCameraTrack middleTrack;
    
    bool twoScreens;
    bool threeScreens;
    ofRectangle leftRect;
    ofRectangle rightRect;
    ofRectangle middleRect;
    
    float lineWidth;
    float pointSize;
    int currentLeft;
    int currentRight;
    int currentMiddle;
    
    vector<ofNode> debugNodes;
    bool loadedSuccess;
    bool composeMode;
  	void checkSwitchCamera(bool force = false);

    ofxPanel panel;
	ofxToggle renderOn;
    
//    ofxOscReceiver receiver;

    bool alignMode;
    ofFbo fbo;
    bool render;
    int totalFramesRendered;
    ofImage savePixels;
  //  bool fullCamMode;
    

};
