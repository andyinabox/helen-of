#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){  
  bool parsingSuccessful = data.open("../../../shared/annotations.json");
  
  if(parsingSuccessful) {
    ofLogNotice() << "Parsing successful!";
  } else {
    ofLogNotice() << "Failed parsing";
  }
  
  current = data[0];
  
  img.load(getImagePath(current));
  img.update();

  for(auto p : current[1]) {
    facePoints.push_back(ofVec2f(p[0].asFloat(), p[1].asFloat()));
  }

  transformFbo.allocate(ofGetWidth(), ofGetHeight());
  transformFbo.begin();
    ofClear(0);
  transformFbo.end();
  

}

//------------------------------------------------facePoints--------------
void ofApp::update(){
  transformFbo.begin();
    ofPushMatrix();
      float scale = ofGetHeight() / current[0][2].asFloat();
      ofScale(scale, scale);
      ofRotate(5);
      img.draw(0, 0);
    
      // draw face points
      for(ofVec2f p : facePoints) {
        ofDrawCircle(p.x, p.y, 3);
      }
    ofPopMatrix();
  transformFbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofClear(0);
  transformFbo.draw(0, 0);
}


string ofApp::getImagePath(ofxJSONElement item) {
  return getSharedPath(ofFilePath::join("images/", item[0][0].asString()));
}

string ofApp::getSharedPath(string path) {
  return ofFilePath::join("../../../shared/", path);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
