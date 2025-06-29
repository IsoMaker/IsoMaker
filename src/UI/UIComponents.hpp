#pragma once

#include "raylib.h"
#include "raygui.h"
#include "UITheme.hpp"
#include <string>
#include <functional>
#include "Entities/MapElement.hpp"

namespace UI {

// Custom button with hover effect
bool CustomButton(Rectangle bounds, const char* text, Color normalColor, Color hoverColor, Color pressedColor);

// Collapsible panel component
bool CollapsiblePanel(Rectangle bounds, const char* title, bool* isOpen, Color headerColor, Color bodyColor);

// Custom slider with label
float CustomSlider(Rectangle bounds, const char* label, float value, float min, float max, Color sliderColor);

// Custom color picker
Color CustomColorPicker(Rectangle bounds, const char* label, Color color);

// Custom dropdown
int CustomDropdown(Rectangle bounds, const char *text, int active, const char **items, int count, bool *editMode);

// Property editor for vector3 values
Vector3 PropertyEditor(Rectangle bounds, const char* label, Vector3 value);

// Tab control
int TabBar(Rectangle bounds, const char** names, int count, int* active);

// Asset tile component
bool AssetTile(Rectangle bounds, Asset2D asset, bool isSelected, Vector2 position);
bool AssetTile(Rectangle bounds, Model texture, const char* name, bool isSelected);

// Tool button with icon
bool ToolButton(Rectangle bounds, Texture2D icon, const char* tooltip, bool isSelected);

// Menu item with submenu support
bool MenuItem(Rectangle bounds, const char* text, bool* showSubmenu);

// Submenu
int Submenu(Rectangle bounds, const char** items, int count);

// Search bar
bool SearchBar(Rectangle bounds, char* text, int textSize, bool* active);

} // namespace UI