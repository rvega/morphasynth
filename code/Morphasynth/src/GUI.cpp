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

#include "GUI.h"
#include "Parameter.h"
#include "Controller.h"

GUI::GUI(Controller* cont): 
   controller(cont),
   allSliders(new vector<ofxUIWidget*>())   
{
   setup();
   ofAddListener(ofEvents.draw, this, &GUI::onDraw);
}

GUI::~GUI(){
   delete allSliders;
   delete canvas1;
   delete canvas2;
   delete canvas3;
   delete canvas4;
   delete canvas5;
}

void GUI::addEvent(GuiEvent e){
   // Iterate all widgets and set the value to the one that matches id
   for (std::vector<ofxUIWidget*>::iterator i = allSliders->begin(); i!=allSliders->end(); ++i) {
      ofxUISlider* widget = (ofxUISlider*) *i;
      if(widget->getID() == e.parameter){
         widget->setValue(e.value);
         break;
      }
   }
}

void GUI::onDraw(ofEventArgs &data) {
   ofSetColor(241, 209, 205);
   ofLine(9, 40, ofGetWidth()-9, 40); // Top horizontal
   ofLine(409, 81, 409, 650); // Left vertical
   ofLine(817, 81, 817, 610); // Right vertical
}

void GUI::setup(){
   // We're using the first digit in the id to determine which dsp module the parameter corresponds to.
   const int totalWidth = ofGetWidth();
   const int totalHeight = ofGetHeight();
   const int padding = 9;
   const int topPadding = 35;
   const int width = totalWidth/5 - 2*padding;
   const int itemHeight = 15;
   const int spacerHeight = 15;
   const int largeSpacerHeight = 56;
   const ofColor bgColor = ofColor(210, 47, 24);

   //=========//
   //  TITLE  //
   //=========//
   canvas1 = new ofxUICanvas(0, 0, width + 2*padding, totalHeight);
   canvas1->addWidgetDown(new ofxUILabel("MORPHASYNTH v0.1", OFX_UI_FONT_LARGE));
   canvas1->addWidgetDown(new ofxUISpacer(1,6)) -> setColorFill(bgColor);

   //===============//
   //  OSCILLATORS  //
   //===============//
   canvas1->addWidgetDown(new ofxUILabel("OSCILLATOR 1", OFX_UI_FONT_MEDIUM));
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(OSCILLATOR1_WAVEFORM);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(OSCILLATOR1_AMPLITUDE);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, -1.0, 1.0, 0.0, "FINETUNE")) -> setID(OSCILLATOR1_FINETUNE);

   canvas1->addWidgetDown(new ofxUISpacer(width, largeSpacerHeight)) -> setColorFill(bgColor);

   canvas1->addWidgetDown(new ofxUILabel("OSCILLATOR 2", OFX_UI_FONT_MEDIUM));
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(OSCILLATOR2_WAVEFORM);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(OSCILLATOR2_AMPLITUDE);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, -1.0, 1.0, 0.0, "FINETUNE")) -> setID(OSCILLATOR2_FINETUNE);

   canvas1->addWidgetDown(new ofxUISpacer(width, largeSpacerHeight)) -> setColorFill(bgColor);

   canvas1->addWidgetDown(new ofxUILabel("SUB OSCILLATOR", OFX_UI_FONT_MEDIUM));
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(OSCILLATOR3_WAVEFORM);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(OSCILLATOR3_AMPLITUDE);
   canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, -1.0, 1.0, 0.0, "FINETUNE")) -> setID(OSCILLATOR3_FINETUNE);

   canvas1->addWidgetDown(new ofxUISpacer(width, 1.5*spacerHeight)) -> setColorFill(bgColor);

   canvas1->addWidgetDown(new ofxUILabel("NOISE", OFX_UI_FONT_MEDIUM));
	canvas1->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.0, "AMPLITUDE")) -> setID(NOISE_LEVEL);

   //========================//
   //  LFOS FOR OSCILLATORS  //
   //========================//
   canvas2 = new ofxUICanvas(width+2*padding, 0, width+2*padding, totalHeight);
   canvas2->addWidgetDown(new ofxUISpacer(width, topPadding)) -> setColorFill(bgColor);

   canvas2->addWidgetDown(new ofxUILabel("LFO FOR OSC 1", OFX_UI_FONT_MEDIUM));
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(LFO_OSC1_WAVEFORM);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 200.0, 0.0, "FREQUENCY")) -> setID(LFO_OSC1_FREQUENCY);  //TODO: logarithmic
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.0, "TO AMPLITUDE")) -> setID(LFO_OSC1_TO_AMPLITUDE);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 20000.0, 0.0, "TO FREQUENCY")) -> setID(LFO_OSC1_TO_FREQUENCY); //TODO: logarithmic

   canvas2->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);

   canvas2->addWidgetDown(new ofxUILabel("LFO FOR OSC 2", OFX_UI_FONT_MEDIUM));
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(LFO_OSC2_WAVEFORM);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 200.0, 0.0, "FREQUENCY")) -> setID(LFO_OSC2_FREQUENCY);  //TODO: logarithmic
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.0, "TO AMPLITUDE")) -> setID(LFO_OSC2_TO_AMPLITUDE);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 20000.0, 0.0, "TO FREQUENCY")) -> setID(LFO_OSC2_TO_FREQUENCY); //TODO: logarithmic

   canvas2->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);

   canvas2->addWidgetDown(new ofxUILabel("LFO FOR SUB OSC", OFX_UI_FONT_MEDIUM));
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(LFO_OSC3_WAVEFORM);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 200.0, 0.0, "FREQUENCY")) -> setID(LFO_OSC3_FREQUENCY);  //TODO: logarithmic
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.0, "TO AMPLITUDE")) -> setID(LFO_OSC3_TO_AMPLITUDE);
   canvas2->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 20000.0, 0.0, "TO FREQUENCY")) -> setID(LFO_OSC3_TO_FREQUENCY); //TODO: logarithmic

   canvas2->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);

   //=============//
   //  HIGH PASS  //
   //=============//
   // canvas3 = new ofxUICanvas(2*width+4*padding, 0, width+2*padding, totalHeight);
   // canvas3->addWidgetDown(new ofxUISpacer(width, topPadding)) -> setColorFill(bgColor);

   // canvas3->addWidgetDown(new ofxUILabel("HIGH PASS FILTER", OFX_UI_FONT_MEDIUM));
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(OSCILLATOR_WAVEFORM);
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(OSCILLATOR_AMPLITUDE);
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "FINETUNE")) -> setID(OSCILLATOR_PHASE);
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "FINETUNE")) -> setID(OSCILLATOR_PHASE);

   // canvas3->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);

   // canvas3->addWidgetDown(new ofxUILabel("HIGH PASS ENVELOPE", OFX_UI_FONT_MEDIUM));
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(OSCILLATOR_WAVEFORM);
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(OSCILLATOR_AMPLITUDE);
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "FINETUNE")) -> setID(OSCILLATOR_PHASE);
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "FINETUNE")) -> setID(OSCILLATOR_PHASE);

   // canvas3->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);
   // 
   // canvas3->addWidgetDown(new ofxUILabel("LFO FOR HIGH PASS", OFX_UI_FONT_MEDIUM));
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(OSCILLATOR_WAVEFORM);
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(OSCILLATOR_AMPLITUDE);
   // canvas3->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "FINETUNE")) -> setID(OSCILLATOR_PHASE);

   // //=============//
   // //  LOW  PASS  //
   // //=============//
   // canvas4 = new ofxUICanvas(3*width+6*padding, 0, width+2*padding, totalHeight);
   // canvas4->addWidgetDown(new ofxUISpacer(width, topPadding)) -> setColorFill(bgColor);

   // canvas4->addWidgetDown(new ofxUILabel("LOW PASS FILTER", OFX_UI_FONT_MEDIUM));
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(OSCILLATOR_WAVEFORM);
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(OSCILLATOR_AMPLITUDE);
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "FINETUNE")) -> setID(OSCILLATOR_PHASE);
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "FINETUNE")) -> setID(OSCILLATOR_PHASE);

   // canvas4->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);

   // canvas4->addWidgetDown(new ofxUILabel("LOW PASS ENVELOPE", OFX_UI_FONT_MEDIUM));
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(OSCILLATOR_WAVEFORM);
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(OSCILLATOR_AMPLITUDE);
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "FINETUNE")) -> setID(OSCILLATOR_PHASE);
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "FINETUNE")) -> setID(OSCILLATOR_PHASE);

   // canvas4->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);
   // 
   // canvas4->addWidgetDown(new ofxUILabel("LFO FOR LOW PASS", OFX_UI_FONT_MEDIUM));
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "WAVEFORM")) -> setID(OSCILLATOR_WAVEFORM);
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.5, "AMPLITUDE")) -> setID(OSCILLATOR_AMPLITUDE);
   // canvas4->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 359.99, 0.0, "FINETUNE")) -> setID(OSCILLATOR_PHASE);

   // canvas4->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);
   // 
   // //============//
   // //  ENVELOPE  //
   // //============//
   // canvas5 = new ofxUICanvas(4*width+8*padding, 0, width+2*padding, totalHeight);
   // canvas5->addWidgetDown(new ofxUISpacer(width, topPadding)) -> setColorFill(bgColor);

   // canvas5->addWidgetDown(new ofxUILabel("ENVELOPE", OFX_UI_FONT_MEDIUM));
	// canvas5->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "ATTACK TIME")) -> setID(ENVELOPE_ATTACK); 
   // canvas5->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "DECAY TIME")) -> setID(ENVELOPE_DECAY);
   // canvas5->addWidgetDown(new ofxUISlider(width, itemHeight, 0.0, 1.0, 0.8, "SUSTAIN LEVEL")) -> setID(ENVELOPE_SUSTAIN);
   // canvas5->addWidgetDown(new ofxUISlider(width, itemHeight, 0.001, 10.0, 0.001, "RELEASE TIME")) -> setID(ENVELOPE_RELEASE);

   // // Store a vector with all widgets for later use
   // vector<ofxUIWidget*> widgets1 = canvas1->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   // vector<ofxUIWidget*> widgets2 = canvas2->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   // vector<ofxUIWidget*> widgets3 = canvas3->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   // vector<ofxUIWidget*> widgets4 = canvas4->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   // vector<ofxUIWidget*> widgets5 = canvas5->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   // allSliders->reserve(widgets1.size() + widgets2.size() + widgets3.size() + widgets4.size() + widgets5.size());
   // allSliders->insert(allSliders->end(), widgets1.begin(), widgets1.end());
   // allSliders->insert(allSliders->end(), widgets2.begin(), widgets2.end());
   // allSliders->insert(allSliders->end(), widgets3.begin(), widgets3.end());
   // allSliders->insert(allSliders->end(), widgets4.begin(), widgets4.end());
   // allSliders->insert(allSliders->end(), widgets5.begin(), widgets5.end());

   // // Add event listener
	// ofAddListener(canvas1->newGUIEvent, this, &GUI::guiEvent);
	// ofAddListener(canvas2->newGUIEvent, this, &GUI::guiEvent);
	// ofAddListener(canvas3->newGUIEvent, this, &GUI::guiEvent);
	// ofAddListener(canvas4->newGUIEvent, this, &GUI::guiEvent);
	// ofAddListener(canvas5->newGUIEvent, this, &GUI::guiEvent);
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

   GuiEvent event;
   event.value = value;
   event.parameter = (Parameter)w->getID();
   controller->addGUIEvent(event);
}
