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

// static std::vector<std::string> PresetManager::getAllPresetNamesForCategory(std::string categoryName){
//    //Lina: Read all xml files in data/presets directory and return vector of strings with preset names
// }
// 
// static std::vector<Parameter> PresetManager::getParametersForPreset(std::string presetName){
//    //Lina: Read xml file that contains the named preset and return vector of Parameters
// }
//       
// static void PresetManager::savePreset(std::string name, std::string category, std::vector<Parameter> parameters){
//    // Lina: Write (or overwrite) a new xml file
// }

//Lina: Other supporting methods (will probably be private and static). 
//
//
// Si la vuelta se pone lenta al leer todos los archivos varias veces, tal vez poner variables instermedias
// que se compartan entre metodos (variables de clase) y que eviten tener que abrir los archivos mas de 1 vez????
// (especulando).

