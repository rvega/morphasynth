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

GUIMorph::GUIMorph(Controller* cont, GUI* superV, int x, int y, int w, int h): 
   controller(cont),
   superView(superV),
   x(x),
   y(y),
   w(w),
   h(h),
   isVisible(true)   
{
   setup();
   ofAddListener(ofEvents.draw, this, &GUIMorph::onDraw);
}

GUIMorph::~GUIMorph(){
   delete canvas1;
}

void GUIMorph::show(){
   isVisible = true;
   canvas1->setVisible(true);
}

void GUIMorph::hide(){
   isVisible = false;
   canvas1->setVisible(false);
}

void GUIMorph::onDraw(ofEventArgs &data) {
//    if(isVisible){
//       ofSetColor(241, 209, 205);
//    }
}

void GUIMorph::setup(){
   canvas1 = new ofxUICanvas(x, y, w, h);
   canvas1->addWidget(new ofxUILabel(300, 300, "AQUI VA UN ESPACIO 2D Y UN TECLADO DE PIANO", OFX_UI_FONT_LARGE));
}
