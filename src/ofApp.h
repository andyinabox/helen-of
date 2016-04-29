#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxThreadedImageLoader.h"

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
  
    ofxJSONElement data;
    ofxJSONElement current;
    vector<ofImage> images;
    vector<ofFbo> fbos;
    ofShader avg;
    ofMesh screen;
    ofFbo canvas;
    ofxThreadedImageLoader imageLoader;
  
    const int imageCount = 10;
  
};
