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

#pragma once

#include "ofxUI.h"
#include "GUIEvent.h"

class GUI;
class Controller;

class GUIPresets{
   public:
      GUIPresets(Controller* cont, GUI* superV, int x, int y, int w, int h);
      ~GUIPresets();

      void show();
      void hide();

   private:
      Controller* controller;
      GUI* superView;
      ofxUICanvas* canvas1;
      ofxUIScrollableCanvas* canvas2;
      int x,y,w,h;

      bool isVisible;

      void setup();
      void onDraw(ofEventArgs &data);
      void guiEvent(ofxUIEventArgs &e);
};
