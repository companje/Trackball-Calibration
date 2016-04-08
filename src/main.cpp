#include "ofMain.h"
#include "ofxManyMouse.h"

class ofApp : public ofBaseApp, public ofxManyMouse {
public:
  
  vector<ofPoint> history[3];
  float trackballOrientation = 0; //45 //90;
  ofImage image;
  float globeRadius = .44;
  
  struct {
    int id;
    ofPoint value; //x,y
  } devices[3];
  
  void setup() {
    ofBackground(0);
    ofEnableNormalizedTexCoords();
    image.load("moon.jpg");
    
    //configure or detect me
    devices[0].id = 0;
    devices[1].id = 0; //1
    devices[2].id = 0; //2
  }
  
  void update() {
    for (int i=0; i<3; i++) {
      devices[i].value.rotate(trackballOrientation, ofVec3f(0,0,1));
      history[i].push_back(devices[i].value);
      if (history[i].size()>40) history[i].erase(history[i].begin());
      devices[i].value *= 0;
    }
  }
  
  void draw() {
    ofBackgroundGradient(100, 0, OF_GRADIENT_CIRCULAR);
    ofTranslate(ofGetWidth()/2,ofGetHeight()/2);
    ofNoFill();
    
    //globe
    ofFill();
    ofSetColor(255);
    image.bind();
    ofEnableDepthTest();
    ofDrawSphere(scaler(globeRadius));
    ofDisableDepthTest();
    image.unbind();

    //trackballs
    for (int i=0; i<3; i++) {
      ofPushMatrix();
      ofPushStyle();
      ofRotateZ(i/3.*360);
      ofTranslate(0,-scaler(globeRadius));
//      ofRotateZ(90);
      ofSetColor(0);
      ofDrawEllipse(0,0,scaler(.3),scaler(.2));
      ofSetColor(255);
      ofDrawSphere(scaler(.1/2));
      
      //trackball motion
      for (int j=0; j<history[i].size(); j++) {
        float jj = float(j)/history[i].size();
        ofPoint &p = history[i].at(j);
        float angle = atan2(p.y,p.x);
        float d = p.length();
        float x = cos(angle)*d;
        float y = sin(angle)*d;
        ofSetColor(ofColor::fromHsb(i/3.*255,255,255,jj*255));
        ofSetLineWidth(3);
        ofDrawLine(0,0,x,y);
      }
      
      ofPopStyle();
      ofPopMatrix();
    }
    
  }
  
  float scaler(float s) {
    return s * ofGetHeight()/2;
  }
  
  void keyPressed(int key) {
    ofSaveFrame();
  }
  
  virtual void mouseMoved(int device, int axis, int delta) {
    if (devices[0].id==device) devices[0].value[axis] += delta;
    if (devices[1].id==device) devices[1].value[axis] += delta;
    if (devices[2].id==device) devices[2].value[axis] += delta;
  }
  
};


//========================================================================
int main( ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context
	ofRunApp(new ofApp());
}
