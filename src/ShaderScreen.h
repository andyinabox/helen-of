#pragma once

//
//  ShaderScreen.h
//  helen1
//
//  Created by Andrew Dayton on 4/29/16.
//
//

#include "ofMain.h"

class ShaderScreen {

  public:
    ofMesh mesh;
    
    void setup(int w, int h, bool arb= true) {
    
      // set gl mode
      mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
      
      // add indexes
      mesh.addIndex(0);
      mesh.addIndex(1);
      mesh.addIndex(2);
      mesh.addIndex(3);
      
      // set vertices and tex coordinates
      update(w, h, arb);

    };
  
    void update(int w, int h, bool arb=true) {
      // set texture coordinates for arbitrary
      // or non-arbitrary texture coords
      int tw = arb ? w : 1;
      int th = arb ? h : 1;

      // reset
      mesh.clearVertices();
      mesh.clearTexCoords();

      // add vertices
      mesh.addVertex(ofVec3f(0, 0, 0));
      mesh.addVertex(ofVec3f(w, 0, 0));
      mesh.addVertex(ofVec3f(w, h, 0));
      mesh.addVertex(ofVec3f(0, h, 0));
      
      // add texture coordinates
      mesh.addTexCoord(ofVec2f(0, 0));
      mesh.addTexCoord(ofVec2f(tw, 0));
      mesh.addTexCoord(ofVec2f(tw, th));
      mesh.addTexCoord(ofVec2f(0, th));
    }
  
    void draw() {
      mesh.draw();
    };

};