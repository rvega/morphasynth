#include "ofMain.h"
#include "MetaMorphASynthApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

   // window.setGlutDisplayString("rgba double samples>=6 depth");        
   // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
   // glEnable(GL_MULTISAMPLE_ARB);
   ofAppGlutWindow window;
   ofSetupOpenGL(&window, 840,425, OF_WINDOW);			// <-------- setup the GL context

   // this kicks off the running of my app
   // can be OF_WINDOW or OF_FULLSCREEN
   // pass in width and height too:
   ofRunApp( new MetaMorphASynthApp());

}
