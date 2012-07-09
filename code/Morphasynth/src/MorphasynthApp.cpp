/**
 * Morphasynth
 * 
 * Copyright (C) 2012 Rafael Vega González
 * Copyright (C) 2012 Daniel Gómez Marín
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MorphasynthApp.h"

//--------------------------------------------------------------
void MorphasynthApp::setup(){
   // ofSetVerticalSync(true); 
   // ofEnableSmoothing(); 
	// ofEnableBlendMode(OF_BLENDMODE_ALPHA);     

   controller = new Controller();
}

void MorphasynthApp::exit(){
   delete controller;
}
//--------------------------------------------------------------
void MorphasynthApp::update(){

}

//--------------------------------------------------------------
void MorphasynthApp::draw(){
	ofBackground(ofColor(233, 52, 27));     
}

//--------------------------------------------------------------
void MorphasynthApp::keyPressed(int key){

}

//--------------------------------------------------------------
void MorphasynthApp::keyReleased(int key){

}

//--------------------------------------------------------------
void MorphasynthApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void MorphasynthApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void MorphasynthApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void MorphasynthApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void MorphasynthApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void MorphasynthApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void MorphasynthApp::dragEvent(ofDragInfo dragInfo){ 

}
