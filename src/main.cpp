#include "ofMain.h"
#include "ofxManyMouse.h"

ofVec3f ofxMouseToSphere(float x, float y) {  //-0.5 ... +0.5
  ofVec3f v(x,y);
  float mag = v.x*v.x + v.y*v.y;
  if (mag>1.0f) v.normalize();
  else v.z = sqrt(1.0f - mag);
  return v;
}

ofVec3f ofxMouseToSphere(ofVec2f v) {  //-0.5 ... +0.5
  return ofxMouseToSphere(v.x,v.y);
}

int localOrientation = -45; //45 //90 //same for all trackballs
int globalOrientation = 0;
float speed = .0005;

class ofApp : public ofBaseApp, public ofxManyMouse {
public:
  
  vector<ofPoint> history[3];
  ofImage image;
  ofPoint mouseFrom,mouseTo;
  
  
  struct {
    float radius = .75;
    float smoothing = 1.0;
    ofQuaternion rotation;
  } globe;
  
  struct Trackball {
    int deviceId; //hardware id
    ofVec2f value; //x,y
    ofVec2f offset,target;
    ofVec3f from,to;
    ofQuaternion q;
    int order; //0,1,2 counter clockwise?
    
    void update() {
      
      value = value.getRotated(localOrientation);
      ofVec2f offset = ofVec2f(.7, 0).getRotated(-120 * order);
      ofVec2f target = offset + value.getRotated(-120 * order) * speed;

      offset.rotate(globalOrientation);
      target.rotate(globalOrientation);

      from = ofxMouseToSphere(offset);
      //cout << "order: " << order << " " << "from: "<< from << endl;
      to = ofxMouseToSphere(target);
      ofVec3f crossed = from.getCrossed(to);
      float dot = from.dot(to);
      q = ofQuaternion(crossed.x, crossed.y, crossed.z, dot);

    }
    
  } trackballs[3];
  
  
  void setup() {
    ofBackground(0);
    ofSetFrameRate(60);
    ofEnableNormalizedTexCoords();
    image.load("moon.jpg");
  }
  
  void update() {
    for (int i=0; i<3; i++) {
      trackballs[i].update();
      history[i].push_back(trackballs[i].value); //undo speed correction for draw
      if (history[i].size()>40) history[i].erase(history[i].begin());
      trackballs[i].value *= 0;
      globe.rotation *= trackballs[i].q;
    }
    
    if (mouseFrom.distance(mouseTo)>.001) {
      ofPoint axis = mouseFrom.getCrossed(mouseTo);
      globe.rotation = ofQuaternion(axis.x,axis.y,axis.z,mouseFrom.dot(mouseTo)); //absolute rotation
    }
 
  }
  
  void draw() {
    ofBackgroundGradient(100, 0, OF_GRADIENT_CIRCULAR);
    ofTranslate(ofGetWidth()/2,ofGetHeight()/2);
    ofNoFill();
    
    drawGlobe();
    drawTrackballs();
    drawArrowAnimation();
    
    ofSetColor(255);
    ofSetupScreen();
    ofDrawBitmapString("globalOrientation: " + ofToString(globalOrientation),50,50);
    ofDrawBitmapString("localOrientation: " + ofToString(localOrientation),50,75);
  }
  
  void keyPressed(int key) {
    if (key=='x') swap(trackballs[0].order, trackballs[1].order);
    if (key=='r') { //shift right
      swap(trackballs[0].order, trackballs[2].order);
      swap(trackballs[1].order, trackballs[2].order);
      cout << trackballs[0].order << trackballs[1].order << trackballs[2].order << endl;
    }
    if (key=='[') globalOrientation--;
    if (key==']') globalOrientation++;
    
    if (key==',') localOrientation--;
    if (key=='.') localOrientation++;
    
  }
  
//  ofQuaternion axesToQuaternion(ofPoint items[]) { //vector<string> items) { //input 6 items
//    //ofxAssert(items.size()==6,"axesToQuaternion: should always receive 6 items, got "+ofToString(items.size()));
//    ofQuaternion q;
//    
//    for (int i=0; i<3; i++) {
//      float x = items[i].x; // .at(i)*2); // ofToFloat(items.at(i*2));
//      float y = items[i].y; //.at(i*2+1); // ofToFloat(items.at(i*2+1));
//      
//      ofVec2f input(x,y);
//      input = input.rotated(trackballs.orientation) * trackballs.speed;
//      ofVec2f offset = ofVec2f(trackballs.offset, 0).rotated(-120 * i);
//      ofVec2f target = offset + input.rotated(-120 * i);
//      
//      offset.rotate(screen.rotation); //globalOrientation);
//      target.rotate(screen.rotation); //globalOrientation);
//      
//      ofVec3f from = ofxMouseToSphere(offset);
//      ofVec3f to = ofxMouseToSphere(target);
//      ofVec3f crossed = from.crossed(to);
//      float dot = from.dot(to);
//      q *= ofQuaternion(crossed.x, crossed.y, crossed.z, dot);
//    }
//    
//    return q;
//  }
  
  void applyRotation() {
    static ofVec3f fromAxis,toAxis;
    static float fromAngle=0,toAngle;
    globe.rotation.getRotate(toAngle,toAxis);
    fromAxis += (toAxis-fromAxis) * globe.smoothing;
    fromAngle += (toAngle-fromAngle) * globe.smoothing;
    ofRotate(fromAngle, fromAxis.x, fromAxis.y, fromAxis.z);
  }
  
  void drawGlobe() {
    ofPushMatrix();
//    ofRotateZ(90); //tricky?
    applyRotation();
    ofFill();
    ofSetColor(255);
    image.bind();
    ofEnableDepthTest();
    
    ofDrawSphere(scaler(globe.radius));
    ofDisableDepthTest();
    image.unbind();
    ofPopMatrix();
    
    for (int i=0; i<3; i++) {
      ofSetColor(255,255,0);
      ofPushMatrix();
      ofSetColor(ofColor::fromHsb(i/3.*255,255,255));
      ofDrawSphere(trackballs[i].from * scaler(globe.radius+.1),5);
      ofDrawLine(trackballs[i].from * scaler(globe.radius+.1), trackballs[i].to * scaler(globe.radius+.1));
      ofPopMatrix();
    }
    
    ofSetColor(255,255,0);
    ofDrawSphere(mouseFrom * scaler(globe.radius),5);
    ofDrawLine(mouseFrom * scaler(globe.radius), mouseTo * scaler(globe.radius));
    
  }
  
  void drawTrackballs() {
    //trackballs
    for (int i=0; i<3; i++) {
      ofPushMatrix();
      ofPushStyle();
      ofRotateZ(-i*120);
      ofTranslate(0,-scaler(globe.radius));
      ofSetColor(0);
      ofDrawEllipse(0,0,scaler(.3),scaler(.2));
      ofSetColor(255);
      ofFill();
      ofDrawCircle(0,0,scaler(.1/2));
      ofSetColor(0);
      ofDrawBitmapString(ofToString(trackballs[i].order),0,0);
      
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
        ofPushMatrix();
        ofRotateZ(-90);
        ofDrawLine(0,0,x,y);
        ofPopMatrix();
      }
      
      ofPopStyle();
      ofPopMatrix();
    }
  }
  
  void drawArrowAnimation() {
    int h = 175;
    for (int i=0, n=4; i<n; i++) {
      float ii=float(i)/n;
      ofPushMatrix();
      float y = int(ofGetFrameNum() + ii*h) % h;
      ofTranslate(0, y);
      ofSetColor(255, (1-abs(2*(y/h)-1)) * 255);
      ofNoFill();
      ofSetLineWidth(10);
      ofBeginShape();
      ofVertex(-30,-30);
      ofVertex(0,0);
      ofVertex(30,-30);
      ofEndShape();
      ofPopMatrix();
    }
  }
  
  float scaler(float s) {
    return s * ofGetHeight()/2;
  }
  
  virtual void mouseMoved(int device, int axis, int delta) {
    //cout << device << " " << axis << " " << delta << endl;
    
    //detect deviceIds
    static vector<int> ids;
    if (ids.size()<3 && !ofContains(ids, device)) {
      trackballs[ids.size()].deviceId = device;
      trackballs[ids.size()].order = ids.size();
      ids.push_back(device);
    }
    
    //cout << ids.size() << " " << ofFind(ids,device) << endl;
    
    if (trackballs[0].deviceId==device) trackballs[0].value[axis] += delta;
    if (trackballs[1].deviceId==device) trackballs[1].value[axis] += delta;
    if (trackballs[2].deviceId==device) trackballs[2].value[axis] += delta;
  }
  
  void mousePressed(int x, int y, int button) {
    mouseFrom = ofPoint(ofGetMouseX()/float(ofGetWidth()/2)-1,ofGetMouseY()/float(ofGetHeight()/2)-1);
    mouseFrom = ofxMouseToSphere(mouseFrom);
  }
  
  void mouseDragged(int x, int y, int button) {
    mouseTo = ofPoint(ofGetMouseX()/float(ofGetWidth()/2)-1,ofGetMouseY()/float(ofGetHeight()/2)-1);
    mouseTo = ofxMouseToSphere(mouseTo);
  }
};


//========================================================================
int main( ){
	ofSetupOpenGL(768,768,OF_WINDOW);			// <-------- setup the GL context
	ofRunApp(new ofApp());
}
