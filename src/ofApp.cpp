#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetVerticalSync(true);
//  ofSetFullscreen(true);

  canvas.allocate(ofGetHeight()*0.75, ofGetHeight());

  bool parsingSuccessful = data.open("../../../shared/annotations.json");
  
  if(parsingSuccessful) {
    ofLogNotice("ofApp::setup") << "Parsing successful!";
  } else {
    ofLogNotice("ofApp::setup") << "Failed parsing";
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
  screen.addVertex(ofVec3f(ofApp::getWidth(), 0, 0));
  screen.addVertex(ofVec3f(ofApp::getWidth(), ofApp::getHeight(), 0));
  screen.addVertex(ofVec3f(0, ofApp::getHeight(), 0));
  screen.addIndex(0);
  screen.addIndex(1);
  screen.addIndex(2);
  screen.addIndex(3);
  screen.addTexCoord(ofVec2f(0, 0));
  screen.addTexCoord(ofVec2f(ofApp::getWidth(), 0));
  screen.addTexCoord(ofVec2f(ofApp::getWidth(), ofApp::getHeight()));
  screen.addTexCoord(ofVec2f(0, ofApp::getHeight()));


}

//------------------------------------------------facePoints--------------
void ofApp::update(){
  if(playing) {
    next();
  }
  
  canvas.begin();
    avg.begin();
    
      avg.setUniform1f("dMultiply", 0.3);
      avg.setUniform2f("direction",
          ofMap(mouseX, 0, ofApp::getWidth(), -1, 1, true),
          ofMap(mouseY, 0, ofApp::getHeight(), -1, 1, true)
      );
    
      for(int i = 0; i < fbos.size(); i++) {
        string texName = "tex"+ofToString(i);
        avg.setUniformTexture(texName, fbos[i].getTexture(0), i);
      }
    
      screen.draw();
    
    avg.end();
  canvas.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofClear(0);
  
  ofPushMatrix();
    ofTranslate((ofGetWidth()-ofApp::getWidth())/2, 0);
    canvas.draw(0, 0);
  ofPopMatrix();
}

void ofApp::next() {
//  ofLogNotice("ofApp::next") << "get next image";
  pushHelenFbo(data[currentIndex]);
  currentIndex++;
}

float ofApp::getWidth() {
  return canvas.getWidth();
}

float ofApp::getHeight() {
  return canvas.getHeight();
}

void ofApp::addHelenFbo(ofxJSONElement item, bool draw) {

    ofImage img;
    img.load(getImagePath(item));
    img.update();
    images.push_back(img);
    
    ofFbo fbo;
    fbo.allocate(ofApp::getWidth(), ofApp::getHeight());
    fbos.push_back(fbo);
  
    if(images.size() > imageCount) {
      ofLogNotice("ofApp::addHelenFbo") << "remove last image";
      images.erase(images.begin());
      fbos.erase(fbos.begin());
    }
  
    if(draw) {
      drawHelenFbo(item, images.size()-1);
    }
}

void ofApp::pushHelenFbo(ofxJSONElement item, bool draw) {
  int end = images.size() - 1;
  
  std::rotate(fbos.begin(), fbos.begin()+1, fbos.end());
  std::rotate(images.begin(), images.begin()+1, images.end());

  images[end].load(getImagePath(item));

  if(draw) {
    drawHelenFbo(item, end);
  }
}

void ofApp::drawHelenFbo(ofxJSONElement item, int index) {

  float baseImgWidth = item[0][1].asFloat();
  float baseImgHeight = item[0][2].asFloat();
  float scale = ofApp::getHeight() / baseImgHeight;
  ofxJSONElement annotations = item[1];

  fbos[index].begin();
    ofClear(0);
    ofPushMatrix();
      ofTranslate((ofApp::getWidth()-(baseImgWidth*scale))/2, 0);
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
    playing = !playing;
  }
  
  if(key == 'k') {
    next();
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
