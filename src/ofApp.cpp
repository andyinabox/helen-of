#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetVerticalSync(true);
  ofSetFullscreen(true);

  bool parsingSuccessful = data.open("../../../shared/annotations.json");
  
  if(parsingSuccessful) {
    ofLogNotice() << "Parsing successful!";
  } else {
    ofLogNotice() << "Failed parsing";
  }
  
  // load images and allocate fbos
  for(int i = 0; i < imageCount; i++) {
    addHelenFbo(data[i]);
    currentIndex = i;
  }


  // load shader
  avg.load("shaders/avg");
  
  // setup screen
  screen.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
  screen.addVertex(ofVec3f(0, 0, 0));
  screen.addVertex(ofVec3f(ofGetWidth(), 0, 0));
  screen.addVertex(ofVec3f(ofGetWidth(), ofGetHeight(), 0));
  screen.addVertex(ofVec3f(0, ofGetHeight(), 0));
  screen.addIndex(0);
  screen.addIndex(1);
  screen.addIndex(2);
  screen.addIndex(3);
  screen.addTexCoord(ofVec2f(0, 0));
  screen.addTexCoord(ofVec2f(ofGetWidth(), 0));
  screen.addTexCoord(ofVec2f(ofGetWidth(), ofGetHeight()));
  screen.addTexCoord(ofVec2f(0, ofGetHeight()));

}

//------------------------------------------------facePoints--------------
void ofApp::update(){
  addHelenFbo(data[currentIndex]);
  currentIndex++;
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofClear(0);
  
//  fbos[currentIndex].draw(0, 0);
  
  avg.begin();
  
    avg.setUniform1f("dMultiply", 0.3);
    avg.setUniform2f("direction",
        ofMap(mouseX, 0, ofGetWidth(), -1, 1, true),
        ofMap(mouseY, 0, ofGetHeight(), -1, 1, true)
    );
  
    for(int i = 0; i < fbos.size(); i++) {
      string texName = "tex"+ofToString(i);
      avg.setUniformTexture(texName, fbos[i].getTexture(0), i);
    }
  
    screen.draw();
  
  avg.end();

  
}

void ofApp::addHelenFbo(ofxJSONElement item) {

    ofImage img;
    img.load(getImagePath(item));
    img.update();
    images.push_back(img);
    
    ofFbo fbo;
    fbo.allocate(ofGetWidth(), ofGetHeight());
    fbos.push_back(fbo);
  
    if(images.size() > imageCount) {
      images.erase(images.begin());
      fbos.erase(fbos.begin());
    }
  
    drawHelenFbo(item, images.size()-1);
}

void ofApp::drawHelenFbo(ofxJSONElement item, int index) {

  float baseImgWidth = item[0][1].asFloat();
  float baseImgHeight = item[0][2].asFloat();
  float scale = ofGetHeight() / baseImgHeight;
  ofxJSONElement annotations = item[1];

  fbos[index].begin();
    ofClear(0);
    ofPushMatrix();
      ofTranslate((ofGetWidth()-(baseImgWidth*scale))/2, 0);
      ofScale(scale, scale);
      images[index].draw(0, 0);
    
      // draw face points
      for(auto p : annotations) {
        ofDrawCircle(p[0].asFloat(), p[1].asFloat(), 2/scale);
      }
    ofPopMatrix();
  fbos[index].end();
}


string ofApp::getImagePath(ofxJSONElement item) {
  return getSharedPath(ofFilePath::join("images/", item[0][0].asString()));
}

string ofApp::getSharedPath(string path) {
  return ofFilePath::join("../../../shared/", path);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if(key == ' ') {
    currentIndex++;
    if(currentIndex >= imageCount) {
      currentIndex = 0;
    }
  }
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
