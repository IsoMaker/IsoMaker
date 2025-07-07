#pragma once

#include "raylib.h"

// Define UI colors with a safer naming to avoid conflicts with raygui enums
namespace UI {
    // Dark theme with proper visibility
    const Color UI_PRIMARY = {32, 35, 42, 255};        // #20232A - Dark primary
    const Color UI_SECONDARY = {44, 47, 56, 255};      // #2C2F38 - Secondary dark
    const Color UI_TERTIARY = {56, 60, 70, 255};       // #383C46 - Lighter dark
    const Color BACKGROUND = {24, 26, 31, 255};        // #181A1F - Main background
    const Color CANVAS_BACKGROUND = {40, 44, 52, 255}; // #282C34 - Canvas background
    
    // Text colors for dark theme
    const Color UI_TEXT_PRIMARY = {255, 255, 255, 255};   // #FFFFFF - White text
    const Color UI_TEXT_SECONDARY = {235, 235, 235, 255}; // #C8CDD2 - Light gray text
    const Color UI_TEXT_TERTIARY = {215, 215, 215, 255};  // #A0A5AA - Dimmed light text
    
    // Accent colors
    const Color ACCENT_PRIMARY = {88, 101, 242, 255};     // #5865F2 - Discord blurple
    const Color ACCENT_SECONDARY = {87, 242, 135, 255};   // #57F287 - Green accent
    const Color ACCENT_TERTIARY = {254, 231, 92, 255};    // #FEE75C - Yellow accent
    const Color ACCENT_DANGER = {237, 66, 69, 255};       // #ED4245 - Red accent
    
    // Status colors
    const Color SUCCESS = {87, 242, 135, 255};     // #57F287 - Success green
    const Color ERROR = {237, 66, 69, 255};        // #ED4245 - Error red
    const Color WARNING = {254, 231, 92, 255};     // #FEE75C - Warning yellow
    const Color INFO = {88, 101, 242, 255};        // #5865F2 - Info blue
    
    // Dark theme panel colors
    const Color PANEL_BACKGROUND = {44, 47, 56, 255};     // #2C2F38 - Dark panel background
    const Color PANEL_HEADER = {32, 35, 42, 255};         // #20232A - Darker header
    const Color PANEL_BORDER = {64, 68, 75, 255};         // #40444B - Dark border
    const Color SELECTED_BACKGROUND = {88, 101, 242, 60}; // Semi-transparent accent
    const Color HOVER_BACKGROUND = {56, 60, 70, 180};     // Dark gray hover
    
    // Shadow and depth
    const Color SHADOW_LIGHT = {0, 0, 0, 40};       // Light shadow
    const Color SHADOW_MEDIUM = {0, 0, 0, 80};      // Medium shadow  
    const Color SHADOW_HEAVY = {0, 0, 0, 120};      // Heavy shadow
    const Color GLOW_ACCENT = {88, 101, 242, 100};  // Accent glow
    
    // Scripting block colors - harmonized with main theme
    const Color BLOCK_EVENT     = {168, 38, 41, 255};   // Darker red
    const Color BLOCK_ACTION    = {55, 63, 153, 255};   // Darker blue
    const Color BLOCK_CONDITION = {36, 160, 81, 255};   // Darker green
    const Color BLOCK_VALUE     = {180, 160, 40, 255};  // Darker yellow
    const Color BLOCK_MISC      = {110, 115, 122, 255}; // Darker gray
    
    // Block secondary colors (lighter versions for body sections)
    const Color BLOCK_EVENT_SECONDARY     = {188, 48, 51, 180};   // Slightly brighter red
    const Color BLOCK_ACTION_SECONDARY    = {65, 75, 175, 180};   // Slightly brighter blue
    const Color BLOCK_CONDITION_SECONDARY = {46, 180, 96, 180};   // Slightly brighter green
    const Color BLOCK_VALUE_SECONDARY     = {200, 180, 55, 180};  // Slightly brighter yellow
    const Color BLOCK_MISC_SECONDARY      = {125, 130, 138, 180}; // Slightly brighter gray

    
    // UI dimensions and spacing
    const float UI_PADDING_SMALL = 4.0f;
    const float UI_PADDING_MEDIUM = 8.0f;
    const float UI_PADDING_LARGE = 16.0f;
    const float UI_PADDING_XLARGE = 24.0f;
    
    const float UI_BORDER_RADIUS_SMALL = 4.0f;
    const float UI_BORDER_RADIUS_MEDIUM = 6.0f;
    const float UI_BORDER_RADIUS_LARGE = 8.0f;
    
    const float UI_SHADOW_OFFSET_SMALL = 2.0f;
    const float UI_SHADOW_OFFSET_MEDIUM = 4.0f;
    const float UI_SHADOW_OFFSET_LARGE = 6.0f;
    
    const int UI_FONT_SIZE_SMALL = 10;
    const int UI_FONT_SIZE_MEDIUM = 12;
    const int UI_FONT_SIZE_LARGE = 14;
    const int UI_FONT_SIZE_XLARGE = 16;
} // namespace UI
