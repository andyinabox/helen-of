#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxGui.h"
#include "ofxThreadedImageLoader.h"

#include "ShaderScreen.h"
#include "Detector.h"

//#define INSTALLATION_MODE

class ofApp : public ofBaseApp{

	public:

    // our data structure for helen images
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

  
    // standard app methods
		void setup();
		void update();
		void draw();
    void drawGui();
    void exit();

    // loading new images
    void next();
//    void prev();
  
    // fbo management functions
    void addFbo(HelenDatum item, bool draw=true);
    void pushFbo(HelenDatum item, ofImage &img , bool draw=true);
    void drawFbo(HelenDatum item, int index);
  
    // misc utility methods
    float getWidth();
    float getHeight();
    string getImagePath(HelenDatum item);
    string getSharedPath(string path);
    ofVec2f getCentroid(vector<ofVec2f> annotations, int start, int end);
    vector<HelenDatum> parseData(ofxJSONElement data);

    // event callbacks
    void onTransitionChange(float &transition);
    void onImageLoaded(ofxThreadedImageLoader::ThreadedLoaderEvent &e);
  

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

    // constants
    const int imageCount = 10;
    const int RIGHT_EYE_START = 114;
    const int RIGHT_EYE_END = 133;
    const int LEFT_EYE_START = 134;
    const int LEFT_EYE_END = 153;
    const int MOUTH_OUTLINE_START = 58;
    const int MOUTH_OUTLINE_END = 85;
		
    bool showGui = true;
    bool playing = false;
    int currentIndex = 0;
    int imageOffset = 0;
    int camWidth = 360;
    int camHeight = 240;
    int detectRegionWidth = camWidth;
    int detectRegionHeight = camHeight;
    float annotationSize;
    float avgDisplacement;
    ofVec2f displacementDirection;
  
    // our dataset
    ofxJSONElement json;
    vector<HelenDatum> data;
    vector<ofImage> images;
    vector<ofFbo> fbos;
  
    // misc
    ofVideoGrabber grabber;
    Detector detector;
    ofImage nextImage;
    ofShader avg;
    ShaderScreen screen;
    ofFbo canvas;
    ofxThreadedImageLoader imageLoader;

    // gui
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
  



};
