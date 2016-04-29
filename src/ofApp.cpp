#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){  
  bool parsingSuccessful = data.open("../../../shared/annotations.json");
  
  if(parsingSuccessful) {
    ofLogNotice() << "Parsing successful!";
  } else {
    ofLogNotice() << "Failed parsing";
  }
  
  
  img.load(getImagePath(data[0]));
}

//--------------------------------------------------------------
void ofApp::update(){
  img.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  img.draw(0, 0);
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
