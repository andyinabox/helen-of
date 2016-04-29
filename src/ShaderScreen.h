//
//  ShaderScreen.h
//  helen1
//
//  Created by Andrew Dayton on 4/29/16.
//
//

#ifndef helen1_ShaderScreen_h
#define helen1_ShaderScreen_h


#endif

#include "ofMain.h"

class ShaderScreen {

  public:
    ofMesh mesh;
    
    void setup(int w, int h) {
      mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
      mesh.addVertex(ofVec3f(0, 0, 0));
      mesh.addVertex(ofVec3f(w, 0, 0));
      mesh.addVertex(ofVec3f(w, h, 0));
      mesh.addVertex(ofVec3f(0, h, 0));
      mesh.addIndex(0);
      mesh.addIndex(1);
      mesh.addIndex(2);
      mesh.addIndex(3);
      mesh.addTexCoord(ofVec2f(0, 0));
      mesh.addTexCoord(ofVec2f(w, 0));
      mesh.addTexCoord(ofVec2f(w, h));
      mesh.addTexCoord(ofVec2f(0, h));
    };
  
//    void update(int w, int h) {
//      // update mesh size
//    }
  
    void draw() {
      mesh.draw();
    };

};