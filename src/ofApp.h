#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxGui.h"
#include "ofxThreadedImageLoader.h"

#include "ShaderScreen.h"
#include "Detector.h"

#define INSTALLATION_MODE

class ofApp : public ofBaseApp{

	public:
  
    struct HelenDatum {
      string fileName;
      int imageWidth;
      int imageHeight;
      
      vector<ofVec2f> points;
      ofVec2f leftEyeCentroid;
      ofVec2f rightEyeCentroid;
      ofVec2f mouthCentroid;
      ofVec2f centroid;
      
      ofVec2f IOV;
      ofVec2f eyesCenter;
      float eyesAngle;
      float area;
    };
  
  
		void setup();
		void update();
		void draw();
  
    void onTransitionChange(float &transition);
  
    void next();
  
    float getWidth();
    float getHeight();
  
    void addHelenFbo(HelenDatum item, bool draw=true);
    void pushHelenFbo(HelenDatum item, ofImage &img , bool draw=true);
//    void pushHelenFbo(ofxJSONElement item, bool draw=true);
    void drawHelenFbo(HelenDatum item, int index);
  
    void imageLoaded(ofxThreadedImageLoader::ThreadedLoaderEvent &e);
  
    vector<HelenDatum> parseData(ofxJSONElement data);
    string getImagePath(HelenDatum item);
    string getSharedPath(string path);
    ofVec2f getCentroid(vector<ofVec2f> annotations, int start, int end);
//    HelenDatum getAverageAnnotations(vector<HelenDatum> data, int start, int end);

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
  
    // general
    ofxToggle loadOnPlay;
    ofxFloatSlider transition;
  
    // transforms
		ofxLabel transformsLabel;
    ofxFloatSlider faceAlign;
    ofxFloatSlider scaleFactor;
    ofxFloatSlider maxDisplacement;
    ofxFloatSlider minDisplacement;
  
    // annotations
    ofxLabel annotationsLabel;
    ofxFloatSlider topAnnotationsOpacity;
    ofxFloatSlider maxAnnotationSize;
    ofxFloatSlider minAnnotationSize;

    // detection
		ofxLabel detectLabel;
    ofxFloatSlider detectThreshold;
    ofxFloatSlider detectUpperThreshold;
    ofxToggle useDetection;
    ofxIntSlider resetBackgroundDelay;
  
    ofVec2f displacementDirection;
  
    int camWidth = 360;
    int camHeight = 240;
    int detectRegionWidth = camWidth;
    int detectRegionHeight = camHeight;
    ofVideoGrabber grabber;
    Detector detector;
  
    vector<HelenDatum> data;
    ofxJSONElement json;
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
