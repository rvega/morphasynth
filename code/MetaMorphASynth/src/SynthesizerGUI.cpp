#include "SynthesizerGUI.h"

SynthesizerGUI::SynthesizerGUI(Synthesizer* synthesizer): 
   synth(synthesizer),
   canvas(new ofxUICanvas())
{
      // Constructor...
}

SynthesizerGUI::~SynthesizerGUI(){
   delete canvas;
}

void SynthesizerGUI::setup(){
   // We're using the first digit in the id to determine which dsp module the parameter corresponds to.
   const int width = 262;
   const int itemHeight = 18;

   //================//
   //  NOTE TRIGGER  //
   //================//
   canvas->addWidgetDown(new ofxUILabel("NOTE ON/OFF", OFX_UI_FONT_MEDIUM));
   canvas->addWidgetDown(new ofxUILabelToggle(false, "NOTE ON", OFX_UI_FONT_SMALL)) -> setID(00);

   //==============//
   //  OSCILLATOR  //
   //==============//
   canvas->addWidgetDown(new ofxUISpacer(width,2));
   canvas->addWidgetDown(new ofxUILabel("OSCILLATOR", OFX_UI_FONT_MEDIUM));

	canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 20.0, 4200.0, 440.0, "FREQUENCY")) -> setID(10);
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(11);
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(12);

   //============//
   //  LOW PASS  //
   //============//
   canvas->addWidgetDown(new ofxUISpacer(width,2));
   canvas->addWidgetDown(new ofxUILabel("LOW PASS FILTER", OFX_UI_FONT_MEDIUM));

	canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 00.0, 22000.0, 22000.0, "FREQUENCY")) -> setID(20); 
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.8, "RESONANCE")) -> setID(21);
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 2.0, 1.0, "GAIN")) -> setID(22);

   //============//
   //  ENVELOPE  //
   //============//
   canvas->addWidgetDown(new ofxUISpacer(width,2));
   canvas->addWidgetDown(new ofxUILabel("ENVELOPE", OFX_UI_FONT_MEDIUM));

	canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "ATTACK TIME")) -> setID(30); 
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "DECAY TIME")) -> setID(31);
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.8, "SUSTAIN LEVEL")) -> setID(32);
   canvas->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "RELEASE TIME")) -> setID(33);

   //==================//
   //  INITIAL VALUES  //
   //==================//
   vector<ofxUIWidget*> sliders = canvas->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   for(vector<ofxUIWidget*>::size_type i=0; i<sliders.size(); i++){
      ofxUISlider* w = (ofxUISlider*) sliders[i];
      sendEventToAudio(w);
   }

   // Add event listener
	ofAddListener(canvas->newGUIEvent, this, &SynthesizerGUI::guiEvent);
}

void SynthesizerGUI::guiEvent(ofxUIEventArgs &e){
   sendEventToAudio(e.widget);
}

void SynthesizerGUI::sendEventToAudio(ofxUIWidget* w){
   // Get value according to widget type
   float value = 0.0;
   if(w->getKind() == OFX_UI_WIDGET_SLIDER_H){
      value = ((ofxUISlider *)w)->getScaledValue();
   }
   else{ //if(w.getKind() == OFX_UI_WIDGET_LABELTOGGLE){
      value = ((ofxUILabelToggle *)w)->getValue();
   }

   // Which parameter are we changing?
   std::string* parameter = new std::string();   // WTF??? 
   int id = w->getID();
   if(id==0) *parameter = "NOTE ON";
   if(id>=10 && id<20) *parameter = "OSCILLATOR_1_" + w->getName();
   if(id>=20 && id<30) *parameter = "LOPASS_" + w->getName();
   if(id>=30 && id<40) *parameter = "ENVELOPE_" + w->getName();

   // Send event to Audio thread
   Event event;
   event.value = value;
   event.parameter = (char*)parameter->c_str();  // How to force a copy here?
   synth->pushEvent(&event);
}
