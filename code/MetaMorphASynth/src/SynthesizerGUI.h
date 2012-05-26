#pragma once

#include "ofxUI.h"
#include "Synthesizer.h"
#include "Event.h"
#include "Stk.h"

class SynthesizerGUI{
   protected:
      ofxUICanvas *canvas;   	
      Synthesizer *synth;
      void guiEvent(ofxUIEventArgs &e);

   public:
      SynthesizerGUI(Synthesizer* synthesizer);
      ~SynthesizerGUI();
      void setup();
};
