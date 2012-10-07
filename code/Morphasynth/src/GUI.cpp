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
   controller(cont)
{
   setup();
   ofAddListener(ofEvents.draw, this, &GUI::onDraw);
   guiTimbre = new GUITimbre(cont, this, 0, 47, ofGetWidth(), ofGetHeight()-47);
   guiMorph = new GUIMorph(cont, this, 0, 47, ofGetWidth(), ofGetHeight()-47);
   guiPresets = new GUIPresets(cont, this, 0, 47, ofGetWidth(), ofGetHeight()-47);
   guiPresets->setGuiTimbre(guiTimbre);

   guiTimbre->show(); 
   guiMorph->hide(); 
   guiPresets->hide(); 
}

GUI::~GUI(){
   delete guiTimbre;
   delete guiMorph;
   delete guiPresets;
}

void GUI::addEvent(GuiEvent e){
   guiTimbre->addEvent(e);
}

void GUI::onDraw(ofEventArgs &data) {
   // Top horizontal line
   ofSetColor(241, 209, 205);
   ofLine(9, 47, ofGetWidth()-9, 47); 
}

void GUI::setup(){
   const ofColor bgColor = ofColor(210, 47, 24);
   const int totalWidth = ofGetWidth();

   // TITLE
   topBar = new ofxUICanvas(0, 0, totalWidth, 47);
   topBar->addWidget(new ofxUILabel(9, 15, "MORPHASYNTH v0.1", OFX_UI_FONT_LARGE));
   topBar->addWidgetRight(new ofxUISpacer(635, 1)) -> setColorFill(bgColor);

   // RADIOS
   std::vector<std::string> radios;
	radios.push_back("TIMBRE");
	radios.push_back("MORPH");
	radios.push_back("PRESETS");	
   ofxUIRadio* radio = new ofxUIRadio(15, 15, "", radios, OFX_UI_ORIENTATION_HORIZONTAL);
   radio->activateToggle("TIMBRE"); 
   topBar->addWidgetRight(radio);

   // PANIC
   topBar->addWidgetRight(new ofxUISpacer(1, 1)) -> setColorFill(bgColor);
	topBar->addWidgetRight(new ofxUILabelButton(false, "PANIC !!!", OFX_UI_FONT_MEDIUM));

	ofAddListener(topBar->newGUIEvent, this, &GUI::guiEvent);
}

void GUI::guiEvent(ofxUIEventArgs &e){
	string name = e.widget->getName(); 
   if(name == "TIMBRE"){
      guiTimbre->show(); 
      guiMorph->hide(); 
      guiPresets->hide(); 
   }
   else if(name == "MORPH"){
      guiTimbre->hide(); 
      guiMorph->show(); 
      guiPresets->hide(); 
   }
   else if(name == "PRESETS"){
      guiTimbre->hide(); 
      guiMorph->hide(); 
      guiPresets->show(); 
   }
   else if(name == "PANIC"){
      // TODO;
      std::cout << "PANIC!!" << "\n";
   }
}
