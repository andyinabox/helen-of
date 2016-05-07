//
//  Detector.h
//  vespers
//
//  Created by Andrew Dayton on 11/15/15.
//
//
#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class Detector {

    public:
        void setup(ofRectangle r, float thresh, int lTime, int tValue);
        void update(ofVideoGrabber &frame);
        void draw(int x, int y, int w, int h);
    
        void updateTimer();
    
        void resetBackground();
        void resetBackground(int delay, ofRectangle r);
    
        void setRegion(ofRectangle roi);
        void setPresenceThreshold(float thresh);
    
        bool isPresent();
        float getPresence();
  
        int learningTime;
        int thresholdValue;
    
        float pThresh;
        ofImage thresholded;
        cv::Rect region;
    
        bool resetCycleInProgress;
        bool resetDelayInProgress;
        bool resetLearningInProgress;
        int resetDelay;
        long resetDelayStartTime;
        long resetLearningStartFrame;
        ofRectangle pendingRegion;
        
        ofxCv::RunningBackground background;    


};
