#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxGui.h"
#include "ofxThreadedImageLoader.h"

#include "ShaderScreen.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
  
    void onTransitionChange(float &transition);
  
    void next();
  
    float getWidth();
    float getHeight();
  
    void addHelenFbo(ofxJSONElement item, bool draw=true);
    void pushHelenFbo(ofxJSONElement item, ofImage &img , bool draw=true);
//    void pushHelenFbo(ofxJSONElement item, bool draw=true);
    void drawHelenFbo(ofxJSONElement item, int index);
  
    void imageLoaded(ofxThreadedImageLoader::ThreadedLoaderEvent &e);
  
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
		
    bool showGui = true;
    bool playing = false;
    int currentIndex = 0;
    int imageOffset = 0;
  
    float annotationSize;
    float avgDisplacement;
  
    ofxPanel gui;
    ofxFloatSlider faceAlign;
    ofxFloatSlider maxAnnotationSize;
    ofxFloatSlider minAnnotationSize;
    ofxFloatSlider maxDisplacement;
    ofxFloatSlider minDisplacement;
    ofxFloatSlider transition;
    ofxToggle loadOnPlay;
    ofVec2f displacementDirection;
  
    ofxJSONElement data;
    ofxJSONElement current;
    ofImage nextImage;
    vector<ofImage> images;
    vector<ofFbo> fbos;
    ofShader avg;
    ShaderScreen screen;
    ofFbo canvas;
    ofxThreadedImageLoader imageLoader;
  
    const int imageCount = 10;
    const int RIGHT_EYE_START = 114;
    const int RIGHT_EYE_END = 133;
    const int LEFT_EYE_START = 134;
    const int LEFT_EYE_END = 153;
    const int MOUTH_OUTLINE_START = 58;
    const int MOUTH_OUTLINE_END = 85;
};
