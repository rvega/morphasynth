#include "MetaMorphASynthApp.h"

//--------------------------------------------------------------
void MetaMorphASynthApp::setup(){
   synth = new Synthesizer();
   audioIO = new AudioIO(synth);
   audioIO->start();
}

void MetaMorphASynthApp::exit(){
   audioIO->stop();
   delete audioIO;
   delete synth;
}
//--------------------------------------------------------------
void MetaMorphASynthApp::update(){

}

//--------------------------------------------------------------
void MetaMorphASynthApp::draw(){

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
