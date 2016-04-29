#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetVerticalSync(true);
//  ofSetFullscreen(true);

  canvas.allocate(ofGetHeight()*0.75, ofGetHeight());

  gui.setup();
  gui.add(faceAlign.setup("Face align", 1.0, 0.0, 1.0));
  gui.add(annotationSize.setup("Annotation size", 2.0, 0.0, 10.0));
  gui.add(avgDisplacement.setup("Displacement", 0.3, 0.0, 10.0));
  displacementDirection = ofVec2f(0, 0);

  bool parsingSuccessful = data.open("../../../shared/annotations.json");
  
  if(parsingSuccessful) {
    ofLogNotice("ofApp::setup") << "Parsing successful!";
  } else {
    ofLogNotice("ofApp::setup") << "Failed parsing";
  }
  
  // load images and allocate fbos
  for(currentIndex; currentIndex < imageCount; currentIndex++) {
    addHelenFbo(data[currentIndex]);
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

  for(int i = 0; i < fbos.size(); i++) {
    drawHelenFbo( data[currentIndex-(imageCount)+i], i);
  }
  
  canvas.begin();
    avg.begin();
  
      // set standard uniforms
      avg.setUniform1f("dMultiply", avgDisplacement);
      avg.setUniform2f("direction", displacementDirection);
  
      // set texture uniforms
      for(int i = 0; i < fbos.size(); i++) {
        string texName = "tex"+ofToString(i);
        avg.setUniformTexture(texName, fbos[i].getTexture(0), i);
      }
  
      // draw empty screen
      screen.draw();
    
    avg.end();

//    fbos[0].draw(0, 0);

  canvas.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofClear(0);
  
    ofPushMatrix();
      ofTranslate((ofGetWidth()-ofApp::getWidth())/2, 0);
      canvas.draw(0, 0);
    ofPopMatrix();
  
  gui.draw();
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
  ofxJSONElement annotations = item[1];
  ofVec2f leftEyeCentroid = getCentroid(annotations, LEFT_EYE_START, LEFT_EYE_END);
  ofVec2f rightEyeCentroid = getCentroid(annotations, RIGHT_EYE_START, RIGHT_EYE_END);
  ofVec2f mouthCentroid = getCentroid(annotations, MOUTH_OUTLINE_START, MOUTH_OUTLINE_END);
  ofVec2f faceCentroid = ofVec2f(
    (leftEyeCentroid.x + rightEyeCentroid.x + mouthCentroid.x)/3,
    (leftEyeCentroid.y + rightEyeCentroid.y + mouthCentroid.y)/3
  );
  

  
  float area = (leftEyeCentroid.distance(rightEyeCentroid) + leftEyeCentroid.distance(mouthCentroid) + mouthCentroid.distance(rightEyeCentroid)) / 2;

  float interpolate = faceAlign;
  float centeredScale = 200 / area;
  float scale = ofLerp(1.0, centeredScale, interpolate);

  displacementDirection = ofVec2f(
    ofMap((ofApp::getWidth()/2 - faceCentroid.x*scale), 0, ofApp::getWidth()/2, -1, 1, true),
    ofMap((ofApp::getHeight()/2 - faceCentroid.y*scale), 0, ofApp::getHeight()/2, -1, 1, true)
  );
  
  ofVec2f defaultTranslation = ofVec2f((ofApp::getWidth()-(baseImgWidth*scale))/2, (ofApp::getHeight()-(baseImgHeight*scale))/2);
  ofVec2f centeredTranslation =  ofVec2f(
    ofApp::getWidth()/2 - faceCentroid.x*scale,
    ofApp::getHeight()/3 - faceCentroid.y*scale
  );
  ofVec2f faceTranslation = defaultTranslation.getInterpolated(centeredTranslation, interpolate);

  fbos[index].begin();
    ofClear(0);
    ofPushMatrix();
      ofTranslate(faceTranslation.x, faceTranslation.y);
      ofScale(scale, scale);
      images[index].draw(0, 0);
    
      // draw face points
      if(annotationSize > 0) {
        for(auto p : annotations) {
          ofPushStyle();
            ofDrawCircle(p[0].asFloat(), p[1].asFloat(), annotationSize/scale);
          ofPopStyle();
        }
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

ofVec2f ofApp::getCentroid(ofxJSONElement annotations, int start, int end) {
    float xt = 0;
    float yt = 0;
    int count = end - start;
  
    for(int i = start; i < end; i++) {
      xt += annotations[i][0].asFloat();
      yt += annotations[i][1].asFloat();
    }
  
    return ofVec2f(xt/count, yt/count);
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
