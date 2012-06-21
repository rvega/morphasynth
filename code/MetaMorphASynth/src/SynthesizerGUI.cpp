#include "SynthesizerGUI.h"

SynthesizerGUI::SynthesizerGUI(Synthesizer* synthesizer): synth(synthesizer) {
   // Constructor...
}

SynthesizerGUI::~SynthesizerGUI(){
   delete canvas1;
   delete canvas2;
   delete canvas3;
}

void SynthesizerGUI::setup(){
   // We're using the first digit in the id to determine which dsp module the parameter corresponds to.
   const int width = 250;
   const int padding = 15;
   const int itemHeight = 18;

   //================//
   //  NOTE TRIGGER  //
   //================//
   canvas1 = new ofxUICanvas(0, 0, width+2*padding, ofGetHeight());
   canvas1->addWidgetDown(new ofxUILabel("NOTE ON/OFF", OFX_UI_FONT_MEDIUM));
   canvas1->addWidgetDown(new ofxUILabelToggle(false, "NOTE ON", OFX_UI_FONT_SMALL)) -> setID(00);
   canvas1->addWidgetDown(new ofxUISpacer(width,1));

   //=========//
   //  NOISE  //
   //=========//
   canvas1->addWidgetDown(new ofxUILabel("NOISE", OFX_UI_FONT_MEDIUM));
	canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.0, "LEVEL")) -> setID(40);
   canvas1->addWidgetDown(new ofxUISpacer(width,1));

   //==============//
   //  OSCILLATOR  //
   //==============//
   canvas1->addWidgetDown(new ofxUILabel("OSCILLATOR", OFX_UI_FONT_MEDIUM));
	canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 27.0, 4200.0, 440.0, "FREQUENCY")) -> setID(10);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(11);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(12);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "PHASE")) -> setID(13);

   //============//
   //  LOW PASS  //
   //============//
   canvas2 = new ofxUICanvas(width+2*padding, 0, width+2*padding, ofGetHeight());
   canvas2->addWidgetDown(new ofxUILabel("LOW PASS FILTER", OFX_UI_FONT_MEDIUM));
	canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 00.0, 22000.0, 22000.0, "FREQUENCY")) -> setID(20); 
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.8, "RESONANCE")) -> setID(21);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 2.0, 1.0, "GAIN")) -> setID(22);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 1.0, "KEYFOLLOW")) -> setID(23);
   canvas2->addWidgetDown(new ofxUISpacer(width,1));

   //===========//
   //  HI PASS  //
   //===========//
   canvas2->addWidgetDown(new ofxUILabel("HI PASS FILTER", OFX_UI_FONT_MEDIUM));
	canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 00.0, 22000.0, 0.0, "FREQUENCY")) -> setID(50);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.0, "RESONANCE")) -> setID(51);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 2.0, 1.0, "GAIN")) -> setID(52);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 1.0, "KEYFOLLOW")) -> setID(53);
   
   //============//
   //  ENVELOPE  //
   //============//
   canvas3 = new ofxUICanvas(2*(width+2*padding), 0, width+2*padding, ofGetHeight());
   canvas3->addWidgetDown(new ofxUILabel("ENVELOPE", OFX_UI_FONT_MEDIUM));
	canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "ATTACK TIME")) -> setID(30); 
   canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "DECAY TIME")) -> setID(31);
   canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.8, "SUSTAIN LEVEL")) -> setID(32);
   canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "RELEASE TIME")) -> setID(33);

   //==================//
   //  INITIAL VALUES  //
   //==================//
   vector<ofxUIWidget*> sliders1 = canvas1->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   vector<ofxUIWidget*> sliders2 = canvas2->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   vector<ofxUIWidget*> sliders3 = canvas3->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   // Merge and iterate 3 vectors
   vector<ofxUIWidget*> allSliders;
   allSliders.reserve(sliders1.size() + sliders2.size() + sliders3.size());
   allSliders.insert(allSliders.end(), sliders1.begin(), sliders1.end());
   allSliders.insert(allSliders.end(), sliders2.begin(), sliders2.end());
   allSliders.insert(allSliders.end(), sliders3.begin(), sliders3.end());
   for(vector<ofxUIWidget*>::size_type i=0; i<allSliders.size(); i++){
      ofxUISlider* w = (ofxUISlider*) allSliders[i];
      sendEventToAudio(w);
   }

   // Add event listener
	ofAddListener(canvas1->newGUIEvent, this, &SynthesizerGUI::guiEvent);
	ofAddListener(canvas2->newGUIEvent, this, &SynthesizerGUI::guiEvent);
	ofAddListener(canvas3->newGUIEvent, this, &SynthesizerGUI::guiEvent);
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

   // TODO: This assignment of ids is not trivial and needs more discussion
   // 
   // TODO: This string is not being released, I'm affraid deleting at the end of this
   //       function will cause a bad memory access at the other end of the ring buffer so
   //       I'm leaving the memory leak for now. 
   //       Initializing as a std::string (not pointer) causes the value not to arrive at 
   //       the other end of the ring buffer.
           
   // Which parameter are we changing?
   std::string* parameter = new std::string();   // WTF??? 
   int id = w->getID();
   if(id==0) *parameter = "NOTE ON";
   if(id>=10 && id<20) *parameter = "OSCILLATOR_1_" + w->getName();
   if(id>=40 && id<50) *parameter = "NOISE_" + w->getName();
   if(id>=20 && id<30) *parameter = "LOPASS_" + w->getName();
   if(id>=50 && id<60) *parameter = "HIPASS_" + w->getName();
   if(id>=30 && id<40) *parameter = "ENVELOPE_" + w->getName();

   // Send event to Audio thread
   Event event;
   event.value = value;
   event.parameter = (char*)parameter->c_str();  // WTF?? Maybe force a copy here?
   synth->pushEvent(&event);
}
