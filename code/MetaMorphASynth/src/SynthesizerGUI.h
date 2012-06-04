#pragma once

#include "ofxUI.h"
#include "Synthesizer.h"
#include "Event.h"
#include "Stk.h"

class SynthesizerGUI{
   protected:
      Synthesizer *synth;
      ofxUICanvas* canvas;

      void guiEvent(ofxUIEventArgs &e);
      void sendEventToAudio(ofxUIWidget* w);

   public:
      SynthesizerGUI(Synthesizer* synthesizer);
      ~SynthesizerGUI();
      void setup();
};
