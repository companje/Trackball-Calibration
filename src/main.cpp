#include "ofMain.h"
#include "ofxManyMouse.h"

int hardwareDeviceIDs[] = {0,1,2};
int trackballOrder[] = {0,1,2};

class ofApp : public ofBaseApp, public ofxManyMouse {
public:
  
  vector<ofPoint> history[3];
  float trackballOrientation = 45;
  
  struct {
    int id;
    ofPoint value; //x,y
  } devices[3];
  
  void setup() {
    ofBackground(0);
    
    //configure or detect me
    devices[0].id = 0;
    devices[1].id = 1;
    devices[2].id = 2;
  }
  
  void update() {
    for (int i=0; i<3; i++) {
      devices[i].value.rotate(trackballOrientation, ofVec3f(0,0,1));
      history[i].push_back(devices[i].value);
      if (history[i].size()>ofGetWidth()) history[i].erase(history[i].begin());
      devices[i].value *= 0;
    }
  }
  
  void draw() {
    ofNoFill();
    
    for (int j=0; j<3; j++) {
      ofPoint &p = history[j].back();
      ofPushMatrix();
      ofTranslate(ofGetWidth()/2, j*200+100);
      ofDrawCircle(0,0, 100);
      float angle = atan2(p.y,p.x);
      float d = p.length();
      float x = cos(angle)*d;
      float y = sin(angle)*d;
      ofDrawLine(0,0,x,y);
      ofPopMatrix();
      
//      for (int i=0; i<history[j].size(); i++) {
        //ofDrawRectangle(i,j*100+50,2,2); //history[j][i]
//      }
    }
  }
  
  virtual void mouseMoved(int device, int axis, int delta) {
    if (devices[0].id==device) devices[0].value[axis] += delta;
    else if (devices[1].id==device) devices[1].value[axis] += delta;
    else if (devices[2].id==device) devices[2].value[axis] += delta;
  }
  
};


//========================================================================
int main( ){
	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context
	ofRunApp(new ofApp());
}
