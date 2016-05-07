//
//  Detector.cpp
//  vespers
//
//  Created by Andrew Dayton on 11/15/15.
//
//

#include "Detector.h"

void Detector::setup(ofRectangle r, float thresh, int lTime, int tValue) {

    learningTime = lTime;
    thresholdValue = tValue;

    setRegion(r);
    setPresenceThreshold(thresh);

	background.setLearningTime(learningTime);
	background.setThresholdValue(thresholdValue);
    
    resetCycleInProgress = false;
    resetDelayInProgress = false;
    resetLearningInProgress = false;
    
}

void Detector::update(ofVideoGrabber &frame) {

    cv::Mat roi;
    cv::Mat frameMat;
    
    frameMat = ofxCv::toCv(frame);
    roi = frameMat(region);
    
    background.update(roi, thresholded);
    thresholded.update();
 
    updateTimer();
}


void Detector::draw(int x, int y, int w, int h) {

    ofPushStyle();
        ofEnableAlphaBlending();
        if(resetDelayInProgress) {
            ofSetColor(255, 0, 0);
        } else if(resetLearningInProgress) {
            ofSetColor(255, 255, 0);
        } else if(isPresent()) {
            ofSetColor(0, 255, 0);
        } else {
            ofSetColor(255);
        }
        thresholded.draw(x, y, w, h);
    
        ofDrawBitmapStringHighlight("Presence: "+ofToString(background.getPresence()), x, y+h);
    
        if(resetCycleInProgress) {
            string output = "";
            
            if(resetDelayInProgress) {
                output += "DELAY: " + ofToString(ofGetElapsedTimeMillis()-resetDelayStartTime) + "/" + ofToString(resetDelay);
            } else if(resetLearningInProgress) {
                output += "LEARN:" + ofToString(ofGetFrameNum()-resetLearningStartFrame)+ "/" + ofToString(learningTime);
            } else {
                output += "ERROR: booleans misaligned";
            }
            
            ofDrawBitmapStringHighlight(output, x, y+5);
        
        }
    
    
    ofPopStyle();

}


void Detector::updateTimer() {
    if(resetCycleInProgress) {
        if(resetDelayInProgress && (ofGetElapsedTimeMillis()-resetDelayStartTime > resetDelay)) {
        
            resetDelayInProgress = false;
            resetLearningInProgress = true;
            
            resetLearningStartFrame = ofGetFrameNum();
            
            setRegion(pendingRegion);
            resetBackground();
        } else if(resetLearningInProgress && ofGetFrameNum()-resetLearningStartFrame > learningTime) {
            
            resetLearningInProgress = false;
            resetCycleInProgress = false;
            resetDelayStartTime = 0;
            resetLearningStartFrame = 0;
        }
    }
}


void Detector::setRegion(ofRectangle r) {
    ofLogNotice("Detector::setRegion") << "set region" << r.x << "," << r.y << "," << r.getWidth() << "," << r.getHeight() << endl;
    region = cv::Rect(r.x, r.y, r.getWidth(), r.getHeight());
}


void Detector::setPresenceThreshold(float thresh) {
    pThresh = thresh;
}

void Detector::resetBackground() {
    background.reset();
}

void Detector::resetBackground(int delay, ofRectangle r) {
    ofLogNotice("Detector::resetBackground") << "Starting background reset timer for " << ofToString(delay) << "ms";
    
    pendingRegion = r;
    
    resetCycleInProgress = true;
    resetDelayInProgress = true;
    resetLearningInProgress = false;
    
    // set reset delay
    resetDelay = delay;
    resetDelayStartTime = ofGetElapsedTimeMillis();
}

bool Detector::isPresent() {
    return background.getPresence() > pThresh;
}

float Detector::getPresence() {
    return background.getPresence();
}


