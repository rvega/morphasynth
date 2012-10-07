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
#include "PresetManager.h"

GUIPresets::GUIPresets(Controller* cont, GUI* superV, int x, int y, int w, int h): 
   controller(cont),
   superView(superV),
   x(x),
   y(y),
   w(w),
   h(h),
   isVisible(true)   
{
   setup();
   ofAddListener(ofEvents.draw, this, &GUIPresets::onDraw);
}

GUIPresets::~GUIPresets(){
   delete canvas1;
}

void GUIPresets::setGuiTimbre(GUITimbre* t){
   this->guiTimbre = t;
}

void GUIPresets::show(){
   isVisible = true;
   canvas1->setVisible(true);
   canvas2->setVisible(true);
}

void GUIPresets::hide(){
   isVisible = false;
   canvas1->setVisible(false);
   canvas2->setVisible(false);
}

void GUIPresets::onDraw(ofEventArgs &data) {
//    if(isVisible){
//       ofSetColor(241, 209, 205);
//    }
}

void GUIPresets::setup(){
   canvas1 = new ofxUICanvas(x, y, w, 150);
   canvas1->addWidgetDown(new ofxUILabel("CREATE A NEW PRESET", OFX_UI_FONT_MEDIUM));
   canvas1->addWidgetDown(new ofxUITextInput(300, "CATEGORY_INPUT", "CATEGORY", OFX_UI_FONT_SMALL));
   canvas1->addWidgetDown(new ofxUITextInput(300, "NAME_INPUT", "NAME", OFX_UI_FONT_SMALL));
	canvas1->addWidgetDown(new ofxUILabelButton(false, "SAVE", OFX_UI_FONT_SMALL));

   canvas1->addWidgetDown(new ofxUISpacer(300, 1));
	ofAddListener(canvas1->newGUIEvent, this, &GUIPresets::guiEvent);

   canvas2 = new ofxUIScrollableCanvas(x, y+150, w, h-150);
   canvas2->setScrollableDirections(false, true);
   canvas2->setScrollArea(x, y+150, w, h-150);
   
   // Draw a section for each preset category 
   std::vector<std::string> categories = PresetManager::getAllCategories();
   for(std::vector<std::string>::size_type i = 0; i < categories.size(); i++){
      std::string category = categories[i];
      canvas2->addWidgetDown(new ofxUILabel(ofToUpper(category), OFX_UI_FONT_MEDIUM));

      // Draw a label and a button for each preset
      std::vector<std::string> presetNames = PresetManager::getPresetNamesForCategory(category);
      for(std::vector<std::string>::size_type j = 0; j < presetNames.size(); j++){
         std::string presetName = presetNames[j];
         canvas2->addWidgetDown(new ofxUILabelButton(false, ofToUpper(presetName), OFX_UI_FONT_SMALL));
      }
      canvas2->addWidgetDown(new ofxUISpacer(300, 1));
   }

   canvas2->autoSizeToFitWidgets();
   canvas2->getRect()->setWidth(w);
	ofAddListener(canvas2->newGUIEvent, this, &GUIPresets::guiEvent);
}

void GUIPresets::guiEvent(ofxUIEventArgs &e){
   ofxUILabelButton* button = (ofxUILabelButton*)e.widget;
   if(!button->getValue()) return; // Prevent triggering the event twice
   if(button->getName() == "SAVE") savePreset();
   else loadPreset(button);
}

void GUIPresets::loadPreset(ofxUILabelButton* button){
   if(!button->getValue()) return;
   std::string parameterName = ofToUpper(button->getName());
   std::vector<GuiEvent> parameters = PresetManager::getParametersForPreset(parameterName);
   for(std::vector<GuiEvent>::size_type i = 0; i < parameters.size(); i++){
      GuiEvent event = parameters[i];
      controller->sendEventToSynth(event);
      controller->sendEventToGui(event);
   }
}

void GUIPresets::savePreset(){
   std::vector<GuiEvent> parameters = guiTimbre->getAllParameters();
   std::string category = ((ofxUITextInput*)canvas1->getWidget("CATEGORY_INPUT"))->getTextString();
   std::string name = ((ofxUITextInput*)canvas1->getWidget("NAME_INPUT"))->getTextString();
   PresetManager::savePreset(name, category, parameters);
}
