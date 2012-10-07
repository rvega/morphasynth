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
#include "GUITimbre.h"
#include "GUIMorph.h"
#include "GUIPresets.h"

class Controller;

class GUI{
   public:
      GUI(Controller* cont);
      ~GUI();

      std::vector<GuiEvent> getAllParameters();
      void addEvent(GuiEvent e);

   private:
      Controller* controller;
      ofxUICanvas* topBar;
      GUITimbre* guiTimbre;
      GUIMorph* guiMorph;
      GUIPresets* guiPresets;

      void setup();
      void onDraw(ofEventArgs &data);
      void guiEvent(ofxUIEventArgs &e);
};
