#pragma once

#include "ofxUI.h"
#include "GuiEvent.h"

class Controller;

class GUI{
   public:
      GUI(Controller* cont);
      ~GUI();

      void addEvent(GuiEvent e);

   private:
      Controller* controller;
      ofxUICanvas* canvas1;
      ofxUICanvas* canvas2;
      ofxUICanvas* canvas3;
      std::vector<ofxUIWidget*>* allSliders;

      void setup();
      void guiEvent(ofxUIEventArgs &e);
      void sendEventToController(ofxUIWidget* w);
};
