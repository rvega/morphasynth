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

#include "MetaMorphASynthApp.h"

//--------------------------------------------------------------
void MetaMorphASynthApp::setup(){
   ofSetVerticalSync(true); 
   ofEnableSmoothing(); 
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);     

   controller = new Controller();
}

void MetaMorphASynthApp::exit(){
   delete controller;
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
