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

#include "PresetManager.h"
#include "Controller.h"
#include "Parameter.h"

/**
 * Read all xml files in data/presets directory and return a vector of strings with category names
 */
std::vector<std::string> PresetManager::getAllCategories(){
   std::vector<std::string> categoryNames;
   ofDirectory dir("presets/");
   dir.allowExt("xml");
   dir.listDir();

   for(int i = 0; i < dir.numFiles(); i++){
      ofxXmlSettings xmlFile;
      xmlFile.loadFile(dir.getPath(i));
      std::string categoryName = xmlFile.getValue("preset:category", "No Category");
      categoryNames.push_back(categoryName);
   }

   std::sort(categoryNames.begin(), categoryNames.end());
   categoryNames.erase( std::unique(categoryNames.begin(), categoryNames.end()), categoryNames.end());
   return categoryNames;
}

std::vector<std::string> PresetManager::getPresetNamesForCategory(std::string categoryName){
   std::vector<std::string> presetNames;
   ofDirectory dir("presets/");
   dir.allowExt("xml");
   dir.listDir();
   for(int i = 0; i < dir.numFiles(); i++){
      ofxXmlSettings xmlFile;
      xmlFile.loadFile(dir.getPath(i));
      std::string catName = xmlFile.getValue("preset:category", "No Category");
      std::string presetName = xmlFile.getValue("preset:name", "No Name");
      if(catName == categoryName){
         presetNames.push_back(presetName);
      }
   }

   std::sort(presetNames.begin(), presetNames.end());
   presetNames.erase( std::unique(presetNames.begin(), presetNames.end()), presetNames.end());
   return presetNames;
}

std::vector<GuiEvent> PresetManager::getParametersForPreset(std::string presetName){
   std::vector<GuiEvent> parameters;

   // Iterate xml files
   ofDirectory dir("presets/");
   dir.allowExt("xml");
   dir.listDir();
   for(int i = 0; i < dir.numFiles(); i++){
      
      // Iterate parameters in xml file
      ofxXmlSettings xmlFile;
      xmlFile.loadFile(dir.getPath(i));
      std::string pstName = xmlFile.getValue("preset:name", "No Name");
      if(ofToUpper(pstName) == ofToUpper(presetName)){
         xmlFile.pushTag("preset");
         xmlFile.pushTag("parameters");
         int numberOfParameters = xmlFile.getNumTags("parameter");

         for(int j = 0; j < numberOfParameters; j++){
            Parameter parameter = String2Parameter(xmlFile.getAttribute("parameter", "name", "???", j));
            float value = xmlFile.getAttribute("parameter", "value", -1.0, j);

            GuiEvent e;
            e.parameter = parameter;
            e.value = value;
            parameters.push_back(e);
         }
         xmlFile.popTag();
         xmlFile.popTag();
         break;
      }
   }

   return parameters;
}
      
void PresetManager::savePreset(std::string name, std::string category, std::vector<GuiEvent> parameters){
   ofxXmlSettings xml;
   xml.addTag("preset");
   xml.pushTag("preset");

   xml.addValue("morphasynth-version", 0.1);
   xml.addValue("name", name);
   xml.addValue("category", category);

   xml.addTag("parameters");
   xml.pushTag("parameters");

   for(std::vector<GuiEvent>::size_type i = 0; i < parameters.size(); i++){
      GuiEvent event = parameters[i];
      xml.addTag("parameter");
      xml.addAttribute("parameter", "name", Parameter2String[event.parameter], i);
      xml.addAttribute("parameter", "value", (double)event.value, i);
   }

   xml.popTag();
   xml.popTag();
   xml.saveFile("presets/" + category + "_" + name + ".xml");
}
