#include "GUI.h"
#include "Parameter.h"
#include "Controller.h"

GUI::GUI(Controller* cont): 
   controller(cont),
   allSliders(new vector<ofxUIWidget*>())   
{
   setup();
}

GUI::~GUI(){
   delete allSliders;
   delete canvas1;
   delete canvas2;
   delete canvas3;
}

void GUI::addEvent(Event e){
   // Iterate all widgets and set the value to the one that matches id
   for (std::vector<ofxUIWidget*>::iterator i = allSliders->begin(); i!=allSliders->end(); ++i) {
      ofxUISlider* widget = (ofxUISlider*) *i;
      if(widget->getID() == e.parameter){
         widget->setValue(e.value1);
         break;
      }
   }
}

void GUI::setup(){
   // We're using the first digit in the id to determine which dsp module the parameter corresponds to.
   const int width = 250;
   const int padding = 15;
   const int itemHeight = 18;

   //================//
   //  NOTE TRIGGER  //
   //================//
   canvas1 = new ofxUICanvas(0, 0, width+2*padding, ofGetHeight());
   canvas1->addWidgetDown(new ofxUILabel("NOTE ON/OFF", OFX_UI_FONT_MEDIUM));
   canvas1->addWidgetDown(new ofxUILabelToggle(false, "NOTE ON", OFX_UI_FONT_SMALL)) -> setID(NOTE_ON);
   canvas1->addWidgetDown(new ofxUISpacer(width,1));

   //=========//
   //  NOISE  //
   //=========//
   canvas1->addWidgetDown(new ofxUILabel("NOISE", OFX_UI_FONT_MEDIUM));
	canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.0, "LEVEL")) -> setID(NOISE_LEVEL);
   canvas1->addWidgetDown(new ofxUISpacer(width,1));

   //==============//
   //  OSCILLATOR  //
   //==============//
   canvas1->addWidgetDown(new ofxUILabel("OSCILLATOR", OFX_UI_FONT_MEDIUM));
	canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 27.0, 4200.0, 440.0, "FREQUENCY")) -> setID(OSCILLATOR_FREQUENCY);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(OSCILLATOR_AMPLITUDE);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(OSCILLATOR_WAVEFORM);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "PHASE")) -> setID(OSCILLATOR_PHASE);

   //============//
   //  LOW PASS  //
   //============//
   canvas2 = new ofxUICanvas(width+2*padding, 0, width+2*padding, ofGetHeight());
   canvas2->addWidgetDown(new ofxUILabel("LOW PASS FILTER", OFX_UI_FONT_MEDIUM));
	canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 00.0, 22000.0, 22000.0, "FREQUENCY")) -> setID(LOW_PASS_FREQUENCY); 
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.8, "RESONANCE")) -> setID(LOW_PASS_RESONANCE);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 2.0, 1.0, "GAIN")) -> setID(LOW_PASS_GAIN);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 1.0, "KEYFOLLOW")) -> setID(LOW_PASS_KEYFOLLOW);
   canvas2->addWidgetDown(new ofxUISpacer(width,1));

   //===========//
   //  HI PASS  //
   //===========//
   canvas2->addWidgetDown(new ofxUILabel("HI PASS FILTER", OFX_UI_FONT_MEDIUM));
	canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 00.0, 22000.0, 0.0, "FREQUENCY")) -> setID(HI_PASS_FREQUENCY);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.0, "RESONANCE")) -> setID(HI_PASS_RESONANCE);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 2.0, 1.0, "GAIN")) -> setID(HI_PASS_GAIN);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 1.0, "KEYFOLLOW")) -> setID(HI_PASS_KEYFOLLOW);
   
   //============//
   //  ENVELOPE  //
   //============//
   canvas3 = new ofxUICanvas(2*(width+2*padding), 0, width+2*padding, ofGetHeight());
   canvas3->addWidgetDown(new ofxUILabel("ENVELOPE", OFX_UI_FONT_MEDIUM));
	canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "ATTACK TIME")) -> setID(ENVELOPE_ATTACK); 
   canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "DECAY TIME")) -> setID(ENVELOPE_DECAY);
   canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.8, "SUSTAIN LEVEL")) -> setID(ENVELOPE_SUSTAIN);
   canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "RELEASE TIME")) -> setID(ENVELOPE_RELEASE);

   // Store a vector with all widgets for later use
   vector<ofxUIWidget*> widgets1 = canvas1->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   vector<ofxUIWidget*> widgets2 = canvas2->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   vector<ofxUIWidget*> widgets3 = canvas3->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   allSliders->reserve(widgets1.size() + widgets2.size() + widgets3.size());
   allSliders->insert(allSliders->end(), widgets1.begin(), widgets1.end());
   allSliders->insert(allSliders->end(), widgets2.begin(), widgets2.end());
   allSliders->insert(allSliders->end(), widgets3.begin(), widgets3.end());

   // Add event listener
	ofAddListener(canvas1->newGUIEvent, this, &GUI::guiEvent);
	ofAddListener(canvas2->newGUIEvent, this, &GUI::guiEvent);
	ofAddListener(canvas3->newGUIEvent, this, &GUI::guiEvent);
}

void GUI::guiEvent(ofxUIEventArgs &e){
   sendEventToController(e.widget);
}

void GUI::sendEventToController(ofxUIWidget* w){
   // Get value according to widget type
   float value = 0.0;
   if(w->getKind() == OFX_UI_WIDGET_SLIDER_H){
      value = ((ofxUISlider *)w)->getScaledValue();
   }
   else{ //if(w.getKind() == OFX_UI_WIDGET_LABELTOGGLE){
      value = ((ofxUILabelToggle *)w)->getValue();
   }

   Event event;
   event.value1 = value;
   event.parameter = (Parameter)w->getID();
   controller->addGUIEvent(event);
}
