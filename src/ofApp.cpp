#include "ofApp.h"

void ofApp::setup(){
  ofSetVerticalSync(true);
  ofSetFrameRate(30);

  transition.addListener(this, &ofApp::onTransitionChange);
  toggleRotation.addListener(this, &ofApp::onToggleRotation);

  gui.setup();
  
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
  gui.add(resetBackgroundDelay.setup("Reset delay", 1000, 0, 120000));

	// video grabber setup
	grabber.setDesiredFrameRate(30);
	grabber.initGrabber(camWidth ,camHeight);
  detector.setup(ofRectangle(0, 0, camWidth, camHeight), detectThreshold, 300, 10);

  
  gui.loadFromFile("settings.xml");

  displacementDirection = ofVec2f(0, 0);
  
  
  
  if(json.open("../../../shared/annotations.json")) {
    ofLogNotice("ofApp::setup") << "Parsing successful!";
    data = parseData(json);
  } else {
    ofLogNotice("ofApp::setup") << "Failed parsing";
  }
  
  updateCanvas();
  
  // load images and allocate fbos
  for(currentIndex; currentIndex < imageCount; currentIndex++) {
    addFbo(data[currentIndex]);
  }


  // load shader
  avg.load("shaders/avg");
  
  screen.setup(ofApp::getWidth(), ofApp::getHeight());
  ofAddListener(imageLoader.ThreadedLoaderE, this, &ofApp::onImageLoaded);
  

}

void ofApp::update(){


  if(playing) {
    if(imageOffset < imageCount-1) {
      imageOffset++;
    } else {
      imageOffset = 0;
    }
  }
  
  if(useDetection) {
  
    grabber.update();
    if(grabber.isFrameNew()) {
      detector.setPresenceThreshold(detectThreshold);
      detector.update(grabber);
    }
    
    float presence = detector.getPresence();
    if(presence > detectThreshold) {
      transition = ofMap(presence, detectThreshold, detectUpperThreshold, 0.0, 1.0, true);
    }
  }
  



  for(int i = 0; i < fbos.size(); i++) {
    drawFbo( data[currentIndex-(imageCount)+i], i);
  }
  
  canvas.begin();
    ofClear(0);
  
    ofDrawRectangle(0, 0, canvas.getWidth(), canvas.getHeight());
  
    avg.begin();
  
      // set standard uniforms
      avg.setUniform1f("u_displacement", avgDisplacement);
      avg.setUniform2f("u_direction", displacementDirection);
  
      // set texture uniforms
      // this will cycle through the current
      // slice of images
      for(int i = 0; i < fbos.size(); i++) {
        int j = abs(i-imageOffset);
        string texName = "tex"+ofToString(i);
        avg.setUniformTexture(texName, fbos[j].getTexture(0), i);
      }
  
      // draw empty screen
      screen.draw();
    
    avg.end();

    if(topAnnotationsOpacity > 0) {

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
  
  canvas.end();
  
}


void ofApp::updateCanvas() {
  int newWidth, newHeight;
  
  if(rotated) {
    newWidth = ofGetHeight();
    newHeight = ofGetWidth();
  } else {
    newWidth = ofGetHeight()*0.75;
    newHeight = ofGetHeight();
  }
  ofLogNotice("updateCanvas") << newWidth << " " << newHeight;
  
  canvas.allocate(newWidth, newHeight);
  screen.update(newWidth, newHeight);
  
  for(auto fbo : fbos) {
    fbo.clear();
    fbo.allocate(newWidth, newHeight);
    ofLogNotice("updateCanvas:fbo") << fbo.getWidth() << " " << fbo.getHeight();
  }
  
}

void ofApp::draw(){
  ofClear(0);

    ofPushMatrix();
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
  
    ofDrawBitmapStringHighlight(ofToString(fbos[0].getWidth()) + " " + ofToString(fbos[0].getHeight()) , 10, 10);
  
  
  if(showGui) {
    drawGui();
  }
}

void ofApp::drawGui() {
  gui.draw();
  detector.draw(ofGetWidth()-(camWidth/2), ofGetHeight()-(camHeight/2), camWidth/2, camHeight/2);
}

void ofApp::onToggleRotation() {
  rotated  = !rotated;
  updateCanvas();
}

void ofApp::onTransitionChange(float &transition) {

  faceAlign = transition;
  annotationSize = ofLerp(maxAnnotationSize, minAnnotationSize, transition);
  avgDisplacement = ofLerp(maxDisplacement, minDisplacement, transition);
}

void ofApp::exit() {}

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
  
    ofPushStyle();
      ofSetColor(0, 255, 0);
      ofDrawRectangle(0, 0, fbos[index].getWidth(), fbos[index].getHeight());
    ofPopStyle();

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

void ofApp::onImageLoaded(ofxThreadedImageLoader::ThreadedLoaderEvent &e) {
  
  pushFbo(data[currentIndex], nextImage, false);
  
  if(currentIndex < data.size() - 1) {
    currentIndex++;
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
  return getSharedPath(ofFilePath::join("images/", item.fileName));
}


string ofApp::getSharedPath(string path) {
  return ofFilePath::join("../../../shared/", path);
}


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
  
  if(key == 'r') {
      detector.resetBackground(resetBackgroundDelay, ofRectangle(0, 0, camWidth, camHeight));
  }
  if(key == 'f') {
    ofToggleFullscreen();
    updateCanvas();
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
