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

#include <vector>
#include <string>

#include "GUIEvent.h"

class PresetManager{
	public:
      static std::vector<std::string> getAllCategories();
      static std::vector<std::string> getPresetNamesForCategory(std::string categoryName);
      static std::vector<GuiEvent> getParametersForPreset(std::string presetName);
      static void savePreset(std::string name, std::string category, std::vector<GuiEvent> parameters);
};
