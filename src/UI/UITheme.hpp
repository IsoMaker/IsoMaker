#pragma once

#include "raylib.h"

// Define UI colors with a safer naming to avoid conflicts with raygui enums
namespace UI {
    // Custom color palette
    const Color UI_PRIMARY = {30, 27, 46, 255};        // #1E1B2E
    const Color UI_SECONDARY = {40, 36, 60, 255};      // #28243C
    const Color BACKGROUND = {248, 244, 255, 255};  // #F8F4FF
    const Color UI_TEXT_PRIMARY = {253, 251, 255, 255};// #FDFBFF
    const Color UI_TEXT_SECONDARY = {176, 174, 184, 255}; // #B0AEB8
    const Color ACCENT_PRIMARY = {248, 143, 112, 255};  // #F88F70
    const Color ACCENT_SECONDARY = {255, 210, 101, 255}; // #FFD265
    const Color ACCENT_TERTIARY = {154, 227, 210, 255};  // #9AE3D2
    const Color SUCCESS = {86, 194, 136, 255};     // #56C288
    const Color ERROR = {235, 107, 107, 255};      // #EB6B6B
    const Color WARNING = {249, 178, 110, 255};    // #F9B26E
} // namespace UI
