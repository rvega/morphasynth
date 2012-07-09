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

#include "ofMain.h"
#include "MorphasynthApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

   // window.setGlutDisplayString("rgba double samples>=6 depth");        
   // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
   // glEnable(GL_MULTISAMPLE_ARB);
   ofAppGlutWindow window;
   ofSetupOpenGL(&window, 1024, 748, OF_WINDOW);			// <-------- setup the GL context

   // this kicks off the running of my app
   // can be OF_WINDOW or OF_FULLSCREEN
   // pass in width and height too:
   ofRunApp( new MorphasynthApp());

}
