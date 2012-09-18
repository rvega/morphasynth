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
   delete canvas2;
}

void GUIPresets::show(){
   isVisible = true;
   canvas1->setVisible(true);
}

void GUIPresets::hide(){
   isVisible = false;
   canvas1->setVisible(false);
}

void GUIPresets::onDraw(ofEventArgs &data) {
//    if(isVisible){
//       ofSetColor(241, 209, 205);
//    }
}

void GUIPresets::setup(){
   canvas1 = new ofxUICanvas(x, y, w, 200);
   canvas1->addWidgetDown(new ofxUILabel("CREATE A NEW PRESET", OFX_UI_FONT_MEDIUM));
   canvas1->addWidgetDown(new ofxUITextInput(300, "CATEGORY_INPUT", "CATEGORY", OFX_UI_FONT_SMALL));
   canvas1->addWidgetDown(new ofxUITextInput(300, "NAME_INPUT", "NAME", OFX_UI_FONT_SMALL));
	canvas1->addWidgetDown(new ofxUILabelButton(false, "SAVE", OFX_UI_FONT_SMALL));


   canvas1->addWidgetDown(new ofxUISpacer(300, 1));
   canvas1->addWidgetDown(new ofxUILabel("AVAILABLE PRESETS:", OFX_UI_FONT_MEDIUM));
   canvas2 = new ofxUIScrollableCanvas(x, y+200, w, h-200);

   // Lina: aqui va un monton de pseudo codigo:
   /*
    categorias = PresetManager::getAllCategories();
    for (categorias as categoria){
      dibuje un label dentro de canvas2("Nombre De La Categoria")
      presetNames = PresetManager::getPresetNamesForCategory(categoria)
      for(presetNames as presetName){
         dibuje Un label dentro de canvas2("Nombre del preset")
         Dibuje un boton a la derecha del label("load preset")
      }
    }
   
    agregue un event handler para los eventos de gui (los botones de "load preset")
   */


}

/*
   Lina:
   eventHandlerBotonSave(foo, bar){
      parameters = guiTimbre->getAllParameters()
      name = canvas1->whatsTheName();
      category = canvas1->whatsTheCategory();

      filename = un nombre "safe" para el sistema de archivos basado en categoria y nombre .xml
      // Usar api de ofXML() para escribir el archivo xml. Ya tiene el nombre, la cat y los params.
   }
   

*/

/* Lina:
   eventHandlerClickBotonesLoadPreset(foo, bar, baz){
      nombrePreset = nosequecosaDelEvento.miraAVerComoRelacionasElIdDelBotonConElNombreDelPreset()
      parametros = PresetManager::getParametersForPreset(nombrePreset)
      for(parametros as parametro){
         e = new Event(parametro.foo, parametro.bar);  // maybe: Aqui hay un buque, pregunte cuando llegue aqui.
         controller->addMidiEvent(e);
         controller->addGUIEvent(e);
      }
   }
   
*/






