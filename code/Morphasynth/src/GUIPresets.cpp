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
   canvas1 = new ofxUICanvas(x, y, w, h);
   canvas1->addWidget(new ofxUILabel(80, 300, "AQUI VA UNA TABLA CON SCROLL PARA LOS PRESETS EXISTENTES (POR CATEGORIAS),", OFX_UI_FONT_LARGE));
   canvas1->addWidget(new ofxUILabel(180, 350, "CAMPOS PARA NOMBRE Y CATEGORIA Y UN BOTON PARA GUARDAR.", OFX_UI_FONT_LARGE));
}
