#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
  
    void next();
  
    float getWidth();
    float getHeight();
  
    void addHelenFbo(ofxJSONElement item, bool draw=true);
  
    void pushHelenFbo(ofxJSONElement item, bool draw=true);
    void drawHelenFbo(ofxJSONElement item, int index);
  
    string getImagePath(ofxJSONElement item);
    string getSharedPath(string path);
    ofVec2f getCentroid(ofxJSONElement annotations, int start, int end);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    bool playing = false;
    int currentIndex = 0;
  
    ofxPanel gui;
    ofxFloatSlider faceAlign;
    ofxFloatSlider annotationSize;
    ofxFloatSlider avgDisplacement;
    ofVec2f displacementDirection;
  
    ofxJSONElement data;
    ofxJSONElement current;
    vector<ofImage> images;
    vector<ofFbo> fbos;
    ofShader avg;
    ofMesh screen;
    ofFbo canvas;
  
    const int imageCount = 10;
    const int RIGHT_EYE_START = 114;
    const int RIGHT_EYE_END = 133;
    const int LEFT_EYE_START = 134;
    const int LEFT_EYE_END = 153;
    const int MOUTH_OUTLINE_START = 58;
    const int MOUTH_OUTLINE_END = 85;
};
