#include "SynthesizerGUI.h"

SynthesizerGUI::SynthesizerGUI(Synthesizer* synthesizer): 
   canvas(new ofxUICanvas()),
   synth(synthesizer){
      // Constructor...
}

SynthesizerGUI::~SynthesizerGUI(){
   delete canvas;
}

void SynthesizerGUI::setup(){
   const int width = 262;
   const int itemHeight = 18;

   canvas->addWidgetDown(new ofxUILabel("OSCILLATOR", OFX_UI_FONT_MEDIUM));
   canvas->addWidgetDown(new ofxUISpacer(width,2));

	canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 20.0, 4200.0, 440.0, "FREQUENCY")); 
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE"));
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM"));

   // Send initial values to synth
   vector<ofxUIWidget*> sliders = canvas->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   for(vector<ofxUIWidget*>::size_type i=0; i<sliders.size(); i++){
      ofxUISlider* w = (ofxUISlider*) sliders[i];

      Event event;
      event.value = w->getScaledValue();
      event.parameter = (char*) w->getName().c_str();
      synth->pushEvent(&event);
   }

   // Add event listener
	ofAddListener(canvas->newGUIEvent, this, &SynthesizerGUI::guiEvent);
}

void SynthesizerGUI::guiEvent(ofxUIEventArgs &e){
   std::string name = e.widget->getName(); 
   ofxUISlider *slider = (ofxUISlider *) e.widget; 
   float value = slider->getScaledValue();

   Event event;
   event.value = value;
   event.parameter = (char*)name.c_str();
   synth->pushEvent(&event);
}
