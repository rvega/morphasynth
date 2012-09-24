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
#include "GUITimbre.h"


GUITimbre::GUITimbre(Controller* cont, GUI* superV, int x, int y, int w, int h): 
   controller(cont),
   superView(superV),
   x(x),
   y(y),
   w(w),
   h(h),
   allSliders(new vector<ofxUIWidget*>()),
   isVisible(true)   
{
   setup();
   ofAddListener(ofEvents.draw, this, &GUITimbre::onDraw);
}

GUITimbre::~GUITimbre(){
   delete allSliders;
   delete canvas1;
   delete canvas2;
   delete canvas3;
   delete canvas4;
   delete canvas5;
}

void GUITimbre::show(){
   isVisible = true;
   canvas1->setVisible(true);
   canvas2->setVisible(true);
   canvas3->setVisible(true);
   canvas4->setVisible(true);
   canvas5->setVisible(true);
}

void GUITimbre::hide(){
   isVisible = false;
   canvas1->setVisible(false);
   canvas2->setVisible(false);
   canvas3->setVisible(false);
   canvas4->setVisible(false);
   canvas5->setVisible(false);
}

void GUITimbre::addEvent(GuiEvent e){
   // Iterate all widgets and set the value to the one that matches id
   for (std::vector<ofxUIWidget*>::iterator i = allSliders->begin(); i!=allSliders->end(); ++i) {

      SliderPot* widget = (SliderPot*) *i;
      if(widget->getID() == e.parameter){
         widget->setValue(e.value);
         break;
      }
   }


}

void GUITimbre::onDraw(ofEventArgs &data) {
   if(isVisible){
      ofSetColor(241, 209, 205);
      ofLine(409, 81, 409, 650); // Left vertical
      ofLine(817, 81, 817, 610); // Right vertical
   }
}

void GUITimbre::setup(){
   // We're using the first digit in the id to determine which dsp module the parameter corresponds to.
   const int totalWidth = w;
   const int totalHeight = h;
   const int padding = 40; //nuevo=4 viejo=9
   const int topPadding = -5; //n=-2 v=-5
   const int width = totalWidth/5 - 2*padding;
   const int itemHeight = 10; //6 nuevo=6 viejo=15
   const int spacerHeight = 15; //igual q arriba
   const int largeSpacerHeight = 56;//n=23 v=56
   const ofColor bgColor = ofColor(210, 47, 24);

   //===============//
   //  OSCILLATORS  //
   //===============//
   canvas1 = new ofxUICanvas(x, y, width + 2*padding, totalHeight);
   canvas1->addWidgetDown(new ofxUISpacer(width, topPadding)) -> setColorFill(bgColor);

   canvas1->addWidgetDown(new ofxUILabel("OSCILLATOR 1", OFX_UI_FONT_MEDIUM));
   canvas1->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "WAVEFORMMM")) -> setID(OSCILLATOR1_WAVEFORM);//0 a 1, n=1 (lineal)
   canvas1->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "AMPLITUDE")) -> setID(OSCILLATOR1_AMPLITUDE);//0 a 1, n=1
   canvas1->addWidgetDown(new SliderPot(width, itemHeight, -1.0, 1.0, 0.5, 1.0, "FINETUNE")) -> setID(OSCILLATOR1_FINETUNE);//-1 a 1, n=1

   canvas1->addWidgetDown(new ofxUISpacer(width, largeSpacerHeight)) -> setColorFill(bgColor);

   canvas1->addWidgetDown(new ofxUILabel("OSCILLATOR 2", OFX_UI_FONT_MEDIUM));
   canvas1->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "WAVEFORM")) -> setID(OSCILLATOR2_WAVEFORM);
   canvas1->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "AMPLITUDE")) -> setID(OSCILLATOR2_AMPLITUDE);
   canvas1->addWidgetDown(new SliderPot(width, itemHeight, -1.0, 1.0, 0.5, 1.0, "FINETUNE")) -> setID(OSCILLATOR2_FINETUNE);

   canvas1->addWidgetDown(new ofxUISpacer(width, largeSpacerHeight)) -> setColorFill(bgColor);

   canvas1->addWidgetDown(new ofxUILabel("SUB OSCILLATOR", OFX_UI_FONT_MEDIUM));
   canvas1->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "WAVEFORM")) -> setID(OSCILLATOR3_WAVEFORM);
   canvas1->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "AMPLITUDE")) -> setID(OSCILLATOR3_AMPLITUDE);
   canvas1->addWidgetDown(new SliderPot(width, itemHeight, -1.0, 1.0, 0.5, 1.0, "FINETUNE")) -> setID(OSCILLATOR3_FINETUNE);

   canvas1->addWidgetDown(new ofxUISpacer(width, 1.5*spacerHeight)) -> setColorFill(bgColor);

   canvas1->addWidgetDown(new ofxUILabel("NOISE", OFX_UI_FONT_MEDIUM));
	canvas1->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 3.0, "AMPLITUDE")) -> setID(NOISE_LEVEL);


   //========================//
   //  LFOS FOR OSCILLATORS  //
   //========================//
   canvas2 = new ofxUICanvas(x+width+2*padding, y, width+2*padding, totalHeight);
   canvas2->addWidgetDown(new ofxUISpacer(width, topPadding)) -> setColorFill(bgColor);

   canvas2->addWidgetDown(new ofxUILabel("LFO FOR OSC 1", OFX_UI_FONT_MEDIUM)); 
   
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "WAVEFORM")) -> setID(LFO_OSC1_WAVEFORM);//0 a 1, n=1
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 200.0, 0.5, 2.0, "FREQUENCY")) -> setID(LFO_OSC1_FREQUENCY);  //TODO: logarithmic. 0 a 200, n=2
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "TO AMPLITUDE")) -> setID(LFO_OSC1_TO_AMPLITUDE);//0 a 1, n=1
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 3.0, "TO FREQUENCY")) -> setID(LFO_OSC1_TO_FREQUENCY); //TODO: logarithmic. 0 a 1, n=3

   canvas2->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);

   canvas2->addWidgetDown(new ofxUILabel("LFO FOR OSC 2", OFX_UI_FONT_MEDIUM));
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "WAVEFORM")) -> setID(LFO_OSC2_WAVEFORM);
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 200.0, 0.5, 2.0, "FREQUENCY")) -> setID(LFO_OSC2_FREQUENCY);  //TODO: logarithmic
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "TO AMPLITUDE")) -> setID(LFO_OSC2_TO_AMPLITUDE);
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 3.0, "TO FREQUENCY")) -> setID(LFO_OSC2_TO_FREQUENCY); //TODO: logarithmic

   canvas2->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);

   canvas2->addWidgetDown(new ofxUILabel("LFO FOR SUB OSC", OFX_UI_FONT_MEDIUM));
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "WAVEFORM")) -> setID(LFO_OSC3_WAVEFORM);
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 200.0, 0.5, 2.0, "FREQUENCY")) -> setID(LFO_OSC3_FREQUENCY);  //TODO: logarithmic
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "TO AMPLITUDE")) -> setID(LFO_OSC3_TO_AMPLITUDE);
   canvas2->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 3.0, "TO FREQUENCY")) -> setID(LFO_OSC3_TO_FREQUENCY); //TODO: logarithmic

   canvas2->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);

   //=============//
   //  HIGH PASS  //
   //=============//
   canvas3 = new ofxUICanvas(x+2*width+4*padding, y, width+2*padding, totalHeight);
   canvas3->addWidgetDown(new ofxUISpacer(width, topPadding)) -> setColorFill(bgColor);

   canvas3->addWidgetDown(new ofxUILabel("HIGH PASS FILTER", OFX_UI_FONT_MEDIUM)); 
  
   canvas3->addWidgetDown(new SliderPot(width, itemHeight, 20.0, 20000.0, 0.5, 3.0, "FREQUENCY")) -> setID(HI_PASS_FREQUENCY);//20 a 20000, n=3
   canvas3->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "RESONANCE")) -> setID(HI_PASS_RESONANCE);//0 a 1, n=1
   canvas3->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "KEYFOLLOW")) -> setID(HI_PASS_KEYFOLLOW);//0 a 1, n=1
   canvas3->addWidgetDown(new SliderPot(width, itemHeight, -5.0, 5.0, 0.5, 1.0, "CONTOUR")) -> setID(HI_PASS_CONTOUR);//-5 a 5, n=1

   canvas3->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);

   canvas3->addWidgetDown(new ofxUILabel("HIGH PASS ENVELOPE", OFX_UI_FONT_MEDIUM));
	canvas3->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 10000.0, 0.5, 3.0, "ATTACK TIME")) -> setID(HI_PASS_ATTACK); // TODO: logarithmic. 0 a 10000, n=3
   canvas3->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 10000.0, 0.5, 3.0, "DECAY TIME")) -> setID(HI_PASS_DECAY);// TODO: logarithmic. 0 a 10000, n=3
   canvas3->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "SUSTAIN LEVEL")) -> setID(HI_PASS_SUSTAIN);//0 a 1, n=1
   canvas3->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 10000.0, 0.5, 3.0, "RELEASE TIME")) -> setID(HI_PASS_RELEASE);// TODO: logarithmic. 0 a 10000, n=3

   canvas3->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);
   
   canvas3->addWidgetDown(new ofxUILabel("LFO FOR HIGH PASS", OFX_UI_FONT_MEDIUM));
   canvas3->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 3.0, "WAVEFORM")) -> setID(LFO_HI_PASS_WAVEFORM);//0 a 1, n=3
   canvas3->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 200.0, 0.5, 2.0, "FREQUENCY")) -> setID(LFO_HI_PASS_FREQUENCY);// TODO: logarithmic. 0 a 200, n=2
   canvas3->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "AMPLITUDE")) -> setID(LFO_HI_PASS_AMPLITUDE);//0 a 1, n=1
  
   //=============//
   //  LOW PASS  //
   //=============//
   canvas4 = new ofxUICanvas(x+3*width+6*padding, y, width+2*padding, totalHeight);
   canvas4->addWidgetDown(new ofxUISpacer(width, topPadding)) -> setColorFill(bgColor);

   canvas4->addWidgetDown(new ofxUILabel("LOW PASS FILTER", OFX_UI_FONT_MEDIUM));
   
   canvas4->addWidgetDown(new SliderPot(width, itemHeight, 20.0, 20000.0, 0.5, 3.0, "FREQUENCY")) -> setID(LO_PASS_FREQUENCY);
   canvas4->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "RESONANCE")) -> setID(LO_PASS_RESONANCE);
   canvas4->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "KEYFOLLOW")) -> setID(LO_PASS_KEYFOLLOW);
   canvas4->addWidgetDown(new SliderPot(width, itemHeight, -5.0, 5.0, 0.5, 1.0, "CONTOUR")) -> setID(LO_PASS_CONTOUR); 

   canvas4->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);

   canvas4->addWidgetDown(new ofxUILabel("LOW PASS ENVELOPE", OFX_UI_FONT_MEDIUM));
	canvas4->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 10000.0, 0.5, 3.0, "ATTACK TIME")) -> setID(LO_PASS_ATTACK); // TODO: logarithmic
   canvas4->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 10000.0, 0.5, 3.0, "DECAY TIME")) -> setID(LO_PASS_DECAY);// TODO: logarithmic
   canvas4->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "SUSTAIN LEVEL")) -> setID(LO_PASS_SUSTAIN);
   canvas4->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 10000.0, 0.5, 3.0, "RELEASE TIME")) -> setID(LO_PASS_RELEASE);// TODO: logarithmic

   canvas4->addWidgetDown(new ofxUISpacer(width, spacerHeight)) -> setColorFill(bgColor);
   
   canvas4->addWidgetDown(new ofxUILabel("LFO FOR LOW PASS", OFX_UI_FONT_MEDIUM));
   canvas4->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 3.0, "WAVEFORM")) -> setID(LFO_LO_PASS_WAVEFORM);
   canvas4->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 200.0, 0.5, 2.0, "FREQUENCY")) -> setID(LFO_LO_PASS_FREQUENCY);// TODO: logarithmic
   canvas4->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "AMPLITUDE")) -> setID(LFO_LO_PASS_AMPLITUDE);

   
   //============//
   //  ENVELOPE  //
   //============//
   canvas5 = new ofxUICanvas(x+4*width+8*padding, y, width+3*padding, totalHeight);
   canvas5->addWidgetDown(new ofxUISpacer(width, topPadding)) -> setColorFill(bgColor);

   canvas5->addWidgetDown(new ofxUILabel("ENVELOPE", OFX_UI_FONT_MEDIUM));
	canvas5->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 10000.0, 0.5, 6.0, "ATTACK TIME")) -> setID(ENVELOPE_ATTACK); 
   canvas5->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 10000.0, 0.5, 3.0, "DECAY TIME")) -> setID(ENVELOPE_DECAY);
   canvas5->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 1.0, 0.5, 1.0, "SUSTAIN LEVEL")) -> setID(ENVELOPE_SUSTAIN);
   canvas5->addWidgetDown(new SliderPot(width, itemHeight, 0.0, 10000.0, 0.5, 3.0, "RELEASE TIME")) -> setID(ENVELOPE_RELEASE);




   //============//
   //  PLUMBING  //
   //============//

   // Store a vector with all widgets for later use
   vector<ofxUIWidget*> widgets1 = canvas1->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   vector<ofxUIWidget*> widgets2 = canvas2->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   vector<ofxUIWidget*> widgets3 = canvas3->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   vector<ofxUIWidget*> widgets4 = canvas4->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);
   vector<ofxUIWidget*> widgets5 = canvas5->getWidgetsOfType(OFX_UI_WIDGET_SLIDER_H);

   allSliders->reserve(widgets1.size() + widgets2.size() + widgets3.size() + widgets4.size() + widgets5.size());

   allSliders->insert(allSliders->end(), widgets1.begin(), widgets1.end());
   allSliders->insert(allSliders->end(), widgets2.begin(), widgets2.end());
   allSliders->insert(allSliders->end(), widgets3.begin(), widgets3.end());
   allSliders->insert(allSliders->end(), widgets4.begin(), widgets4.end());
   allSliders->insert(allSliders->end(), widgets5.begin(), widgets5.end());


   // Add event listener
	ofAddListener(canvas1->newGUIEvent, this, &GUITimbre::guiEvent);
	ofAddListener(canvas2->newGUIEvent, this, &GUITimbre::guiEvent);
	ofAddListener(canvas3->newGUIEvent, this, &GUITimbre::guiEvent);
	ofAddListener(canvas4->newGUIEvent, this, &GUITimbre::guiEvent);
	ofAddListener(canvas5->newGUIEvent, this, &GUITimbre::guiEvent);

}

void GUITimbre::guiEvent(ofxUIEventArgs &e){
   sendEventToController(e.widget);
}

void GUITimbre::sendEventToController(ofxUIWidget* w){
   float value = ((SliderPot *)w)->getMapValue();
	
   // Debug:
	// std::cout << "valor mapeado--> " << value << "\n" ;

   GuiEvent event;
   event.value = value;
   event.parameter = (Parameter)w->getID();
   controller->addGUIEvent(event);
}
