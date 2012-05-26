#include "MetaMorphASynthApp.h"

//--------------------------------------------------------------
void MetaMorphASynthApp::setup(){
   ofSetVerticalSync(true); 
   ofEnableSmoothing(); 
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);     

   synth = new Synthesizer();

   synthGUI = new SynthesizerGUI(synth);
   synthGUI->setup();

   audioIO = new AudioIO(synth);
   audioIO->start();
}

void MetaMorphASynthApp::exit(){
   audioIO->stop();
   delete audioIO;
   delete synthGUI;
   delete synth;
}
//--------------------------------------------------------------
void MetaMorphASynthApp::update(){

}

//--------------------------------------------------------------
void MetaMorphASynthApp::draw(){
	ofBackground(ofColor(233, 52, 27));     
}

//--------------------------------------------------------------
void MetaMorphASynthApp::keyPressed(int key){

}

//--------------------------------------------------------------
void MetaMorphASynthApp::keyReleased(int key){

}

//--------------------------------------------------------------
void MetaMorphASynthApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void MetaMorphASynthApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void MetaMorphASynthApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void MetaMorphASynthApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void MetaMorphASynthApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void MetaMorphASynthApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void MetaMorphASynthApp::dragEvent(ofDragInfo dragInfo){ 

}
