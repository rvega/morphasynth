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

   //================//
   //  NOTE TRIGGER  //
   //================//
   canvas->addWidgetDown(new ofxUILabel("NOTE ON/OFF", OFX_UI_FONT_MEDIUM));
   canvas->addWidgetDown(new ofxUILabelToggle(false, "NOTE ON", OFX_UI_FONT_SMALL));

   //==============//
   //  OSCILLATOR  //
   //==============//
   canvas->addWidgetDown(new ofxUISpacer(width,2));
   canvas->addWidgetDown(new ofxUILabel("OSCILLATOR", OFX_UI_FONT_MEDIUM));

	canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 20.0, 4200.0, 440.0, "FREQUENCY")); 
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE"));
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM"));

   //============//
   //  ENVELOPE  //
   //============//
   canvas->addWidgetDown(new ofxUISpacer(width,2));
   canvas->addWidgetDown(new ofxUILabel("ENVELOPE", OFX_UI_FONT_MEDIUM));

	canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "ATTACK TIME")); 
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "DECAY TIME"));
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.8, "SUSTAIN LEVEL"));
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "RELEASE TIME"));


   //==================//
   //  INITIAL VALUES  //
   //==================//
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
   float value = 0.0;

   if(e.widget->getKind() == OFX_UI_WIDGET_SLIDER_H){
      value = ((ofxUISlider *)e.widget)->getScaledValue();
   }
   else{ //if(e.widget.getKind() == OFX_UI_WIDGET_LABELTOGGLE){
      value = ((ofxUILabelToggle *)e.widget)->getValue();
   }

   Event event;
   event.value = value;
   event.parameter = (char*)name.c_str();
   synth->pushEvent(&event);
}
