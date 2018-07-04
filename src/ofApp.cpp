#include "ofApp.h"

void ofApp::setup(){
  ofSetVerticalSync(true);
  ofSetFrameRate(30);
  
  ofSetFullscreen(true);

  // load data
  if(json.open(ofToDataPath("helen-images/annotations.json"))) {
    ofLogNotice("ofApp::setup") << "Parsing successful! " << json.size() << " entries loaded";
    data = parseData(json);
  } else {
    ofLogNotice("ofApp::setup") << "Failed parsing";
  }

  // setup gui
  transition.addListener(this, &ofApp::onTransitionChange);
  toggleRotation.addListener(this, &ofApp::onToggleRotation);
  
  gui.setup();
  gui.add(currentIndex.setup("Current index", 0, 0, data.size()-1));
  gui.add(loadOnPlay.setup("New img loading", true));
  gui.add(transition.setup("Transition", 0.0, 0.0, 1.0));
  gui.add(toggleRotation.setup("Toggle rotation"));
  gui.add(transformsLabel.setup("// TRANSFORMS", ""));
  gui.add(faceAlign.setup("Face align", 1.0, 0.0, 1.0));
  gui.add(scaleFactor.setup("Scale", 300, 1, 500));
  gui.add(maxDisplacement.setup("Begin displacement", 5.0, 0.0, 10.0));
  gui.add(minDisplacement.setup("End displacement", 0.3, 0.0, 10.0));
  gui.add(annotationsLabel.setup("// ANNOTATIONS", ""));
  gui.add(topAnnotationsOpacity.setup("Top annotation opacity", 0., 0.0, 1.0));
  gui.add(maxAnnotationSize.setup("Begin annotation size", 5.0, 0.0, 10.0));
  gui.add(minAnnotationSize.setup("End annotation size", 0.0, 0.0, 10.0));
  gui.add(detectLabel.setup("// DETECTION", ""));
  gui.add(useDetection.setup("Use detection", true));
	gui.add(detectThreshold.setup("Detect lower threshold", 0.1, 0.0, 1.0));
	gui.add(detectUpperThreshold.setup("Detect upper threshold", 0.8, 0.0, 1.0));
	gui.add(detectOutputLimit.setup("Detect output limit", 0.75, 0.0, 1.0));
  gui.add(resetBackgroundDelay.setup("Reset delay", 1000, 0, 120000));
  gui.loadFromFile("settings.xml");

	// video grabber setup
  #ifdef USE_PS3EYE
  grabber.setGrabber(std::make_shared<ofxPS3EyeGrabber>());
  #endif
	grabber.setDesiredFrameRate(30);
	grabber.setup(camWidth ,camHeight);
  detector.setup(ofRectangle(0, 0, camWidth, camHeight), detectThreshold, 300, 10);
  tracker.setup();


  // set canvas size
  screen.setup(ofGetWidth(), ofGetHeight());
  updateCanvasSize();
  
  // load shader
  avg.load("shaders/avg");
  
  // setup listener for image loader
  ofAddListener(ofxTILEvent().events, this, &ofApp::onImageLoaded);

  // load images and allocate fbos
  for(currentIndex; currentIndex < imageCount; currentIndex=currentIndex+1) {
    addFbo(data[currentIndex]);
  }

}

void ofApp::update(){

  // update image playback by incrementing
  // image offset
  if(playing) {
    if(imageOffset < imageCount-1) {
      imageOffset++;
    } else {
      imageOffset = 0;
    }
  }
  
  // if using background subtraction, update
  // detection
  if(useDetection) {
  
    grabber.update();
    
    // update detector from latest frame
    if(grabber.isFrameNew()) {
      detector.setPresenceThreshold(detectThreshold);
      detector.update(grabber);
      tracker.update(ofxCv::toCv(grabber));
      
      // update transition value based on presence
      if(tracker.getFound()) {
        transition = 1.0;
      } else if (detector.isPresent()) {
        transition = ofMap(detector.getPresence(), detectThreshold, detectUpperThreshold, 0.0, detectOutputLimit, true);
      } else {
        transition = 0.0;
      }
    }
  }
  
  // go through and redraw all of the fbos
  for(int i = 0; i < fbos.size(); i++) {
    ofLogNotice("ofApp::update") << "Draw fbo for index " << (currentIndex-(imageCount)+i);
    drawFbo( data[abs(currentIndex-(imageCount)+i)], i);
  }
  
  // update the canvas fbo
  canvas.begin();
    ofClear(0);
  
    // draw to average shader
    drawAverage();

    // overlay annotations (if enabled)
    if(topAnnotationsOpacity > 0) {
      drawAnnotations();
    }
  
  canvas.end();
  
}


void ofApp::updateCanvasSize() {
  int newWidth, newHeight;
  
  if(rotated) {
    newWidth = ofGetHeight();
    newHeight = ofGetWidth();
  } else {
    newWidth = ofGetHeight()*0.75;
    newHeight = ofGetHeight();
  }
  ofLogNotice("ofApp::updateCanvasSize") << newWidth << ", " << newHeight;
  
  canvas.allocate(newWidth, newHeight);
  screen.update(newWidth, newHeight);
  
  for(auto fbo : fbos) {
    fbo.clear();
    fbo.allocate(newWidth, newHeight);
  }
  
}

void ofApp::draw(){
  ofClear(0);
  
    ofPushMatrix();
  
      // rotate and translate canvas based
      // on current settings
      if(rotated) {
        ofTranslate((ofGetWidth()-ofApp::getWidth())/2, (ofGetHeight()-ofApp::getHeight())/2);
        ofTranslate(canvas.getWidth()/2, canvas.getHeight()/2);
        ofRotate(-90, 0, 0, 1);
        ofTranslate(-canvas.getWidth()/2, -canvas.getHeight()/2);
      } else {
        ofTranslate((ofGetWidth()-ofApp::getWidth())/2, 0);
      }
  
      canvas.draw(0, 0);
  
    ofPopMatrix();
  
  if(showGui) {
    drawGui();
  }
}

void ofApp::drawGui() {
  gui.draw();
  detector.draw(ofGetWidth()-(camWidth/2), ofGetHeight()-(camHeight/2), camWidth/2, camHeight/2);
  
  ofDrawBitmapStringHighlight(ofToString(int(ofGetFrameRate()))+" fps", 5, ofGetHeight()-10);

}

void ofApp::drawAnnotations() {
  HelenDatum latest = data[currentIndex];
  float scale = scaleFactor / latest.area;

  ofPushMatrix();
    ofTranslate(
      canvas.getWidth()/2 - latest.centroid.x*scale,
      canvas.getHeight()/2 - latest.centroid.y*scale
    );
    ofScale(scale, scale);
    ofTranslate(latest.centroid.x, latest.centroid.y);
    ofRotate(latest.eyesAngle, 0, 0, 1);
    ofTranslate(-latest.centroid.x, -latest.centroid.y);

    ofSetColor(255, 255, 255, 255*topAnnotationsOpacity);

    for(auto p : latest.points) {
      ofDrawCircle(p.x, p.y, annotationSize/scale);
    }
  ofPopMatrix();
}

void ofApp::drawAverage() {
    avg.begin();
  
      // set standard uniforms
      avg.setUniform1f("u_displacement", avgDisplacement);
      avg.setUniform2f("u_direction", displacementDirection);
  
      // set texture uniforms
      // this will cycle through the current
      // slice of images
      for(int i = 0; i < fbos.size(); i++) {
        int j = abs(i-imageOffset);
        avg.setUniformTexture("tex"+ofToString(i), fbos[j].getTexture(0), i);
      }
  
      // draw empty screen
      screen.draw();
    
    avg.end();
}

void ofApp::drawFbo(HelenDatum item, int index) {

  float interpolate = faceAlign;
  float centeredScale = scaleFactor / item.area;
  float scale = ofLerp(1.0, centeredScale, interpolate);

  displacementDirection = ofVec2f(
    ofMap((ofApp::getWidth()/2 - item.centroid.x*scale), 0, ofApp::getWidth()/2, -1, 1, true),
    ofMap((ofApp::getHeight()/2 - item.centroid.y*scale), 0, ofApp::getHeight()/2, -1, 1, true)
  );
  
  ofVec2f defaultTranslation = ofVec2f((ofApp::getWidth()-(item.imageWidth*scale))/2, (ofApp::getHeight()-(item.imageHeight*scale))/2);
  ofVec2f centeredTranslation =  ofVec2f(
    ofApp::getWidth()/2 - item.centroid.x*scale,
    ofApp::getHeight()/2 - item.centroid.y*scale
  );
  ofVec2f faceTranslation = defaultTranslation.getInterpolated(centeredTranslation, interpolate);
  
  fbos[index].begin();
    ofClear(0);

    ofPushMatrix();
      ofTranslate(faceTranslation.x, faceTranslation.y);
      ofScale(scale, scale);
      ofTranslate(item.centroid.x, item.centroid.y);
      ofRotate(ofLerp(0, item.eyesAngle, interpolate) , 0, 0, 1);
      ofTranslate(-item.centroid.x, -item.centroid.y);
      images[index].draw(0, 0);
  
      // draw face points
      if(annotationSize > 0) {
        for(auto p : item.points) {
          ofPushStyle();
            ofDrawCircle(p.x, p.y, annotationSize/scale);
          ofPopStyle();
        }
      }
  
    ofPopMatrix();
  fbos[index].end();
}

void ofApp::onToggleRotation() {
  rotated  = !rotated;
  updateCanvasSize();
}

void ofApp::onTransitionChange(float &transition) {

  faceAlign = transition;
  annotationSize = ofLerp(maxAnnotationSize, minAnnotationSize, transition);
  avgDisplacement = ofLerp(maxDisplacement, minDisplacement, transition);
}

void ofApp::exit() {
  tracker.waitForThread();
//  imageLoader.waitForThread();
}

void ofApp::next() {
  imageLoader.loadFromDisk(nextImage, getImagePath(data[currentIndex]));
}

float ofApp::getWidth() {
  return canvas.getWidth(); //rotated ? canvas.getHeight() : canvas.getWidth();
}

float ofApp::getHeight() {
  return canvas.getHeight(); //rotated ? canvas.getWidth() : canvas.getHeight();
}

void ofApp::addFbo(ofApp::HelenDatum item, bool draw) {

    ofImage img;
    img.load(getImagePath(item));
    img.update();
    images.push_back(img);
    
    ofFbo fbo;
    fbo.allocate(ofApp::getWidth(), ofApp::getHeight());
    fbos.push_back(fbo);
  
    if(images.size() > imageCount) {
      ofLogNotice("ofApp::addFbo") << "remove last image";
      images.erase(images.begin());
      fbos.erase(fbos.begin());
    }
  
    if(draw) {
      drawFbo(item, images.size()-1);
    }
}

void ofApp::pushFbo(HelenDatum item, ofImage &img, bool draw) {
  int end = images.size() - 1;

  images.push_back(img);
  images.erase(images.begin());

  if(draw) {
    drawFbo(item, end);
  }
}


void ofApp::onImageLoaded(ofxTILEvent &e) {
  
  pushFbo(data[currentIndex], nextImage, false);
  
  if(currentIndex < data.size() - 1) {
    currentIndex=currentIndex+1;
  } else {
    currentIndex = 0;
  }
  
  if(playing && loadOnPlay) {
    next();
  }
  
}


vector<ofApp::HelenDatum> ofApp::parseData(ofxJSONElement items) {
  vector<ofApp::HelenDatum> helenData;
  
  for(auto item : items) {
    ofApp::HelenDatum hd;

    hd.fileName = item[0][0].asString();
    hd.imageWidth = item[0][1].asInt();
    hd.imageHeight = item[0][2].asInt();
    
    vector<ofVec2f> points;
    for(auto p : item[1]) {
      points.push_back(ofVec2f(p[0].asFloat(), p[1].asFloat()));
    }
    hd.points = points;
        
    hd.leftEyeCentroid = getCentroid(hd.points, LEFT_EYE_START, LEFT_EYE_END);
    hd.rightEyeCentroid = getCentroid(hd.points, RIGHT_EYE_START, RIGHT_EYE_END);
    hd.mouthCentroid = getCentroid(hd.points, MOUTH_OUTLINE_START, MOUTH_OUTLINE_END);
    hd.centroid = ofVec2f(
      (hd.leftEyeCentroid.x + hd.rightEyeCentroid.x + hd.mouthCentroid.x)/3,
      (hd.leftEyeCentroid.y + hd.rightEyeCentroid.y + hd.mouthCentroid.y)/3
    );
    
    // interocculary vector
    hd.IOV = hd.rightEyeCentroid - hd.leftEyeCentroid;
    hd.eyesCenter = .5 * (hd.leftEyeCentroid + hd.rightEyeCentroid);
    hd.eyesAngle = 180.0 / PI * atan2(-hd.IOV.y, hd.IOV.x);
    
    hd.area = (hd.leftEyeCentroid.distance(hd.rightEyeCentroid) + hd.leftEyeCentroid.distance(hd.mouthCentroid) + hd.mouthCentroid.distance(hd.rightEyeCentroid)) / 2;
    
    helenData.push_back(hd);
  }
  
  return helenData;
}

string ofApp::getImagePath(ofApp::HelenDatum item) {
  return ofToDataPath(ofFilePath::join("helen-images/images/", item.fileName));
}


//string ofApp::getSharedPath(string path) {
//  return ofFilePath::join("../../../shared/", path);
//}


ofVec2f ofApp::getCentroid(vector<ofVec2f> annotations, int start, int end) {
    float xt = 0;
    float yt = 0;
    int count = end - start;
  
    if(annotations.size() >= end) {
      for(int i = start; i < end; i++) {
        xt += annotations[i].x;
        yt += annotations[i].y;
      }
    } else {
      ofLogNotice("ofApp::getCentroid") << "Not enough points!";
    }
  
    return ofVec2f(xt/count, yt/count);
}



void ofApp::keyPressed(int key){
  if(key == ' ') {
    playing = !playing;
    if(playing) {
      next();
    }
  }
  
  if(key == 'k') {
    next();
  }
  
  if(key == 'g') {
    showGui = !showGui;
  }
  
  if(key == 'l') {
    gui.loadFromFile("settings.xml");
  }
  
  if(key == 's') {
    gui.saveToFile("settings.xml");
  }
  
  if(key == 'b') {
      detector.resetBackground(resetBackgroundDelay, ofRectangle(0, 0, camWidth, camHeight));
  }
  
  if(key == 'r') {
    onToggleRotation();
  }
  
  if(key == 'f') {
    ofToggleFullscreen();
    updateCanvasSize();
  }
}

void ofApp::keyReleased(int key){}
void ofApp::mouseMoved(int x, int y ){}
void ofApp::mouseDragged(int x, int y, int button){}
void ofApp::mousePressed(int x, int y, int button){}
void ofApp::mouseReleased(int x, int y, int button){}
void ofApp::mouseEntered(int x, int y){}
void ofApp::mouseExited(int x, int y){}
void ofApp::windowResized(int w, int h){}
void ofApp::gotMessage(ofMessage msg){}
void ofApp::dragEvent(ofDragInfo dragInfo){}
