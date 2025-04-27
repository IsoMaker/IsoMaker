#include "UIComponents.hpp"
#include <cmath>
#include <cstring>


namespace UI {

bool CustomButton(Rectangle bounds, const char* text, Color normalColor, Color hoverColor, Color pressedColor) {
    Vector2 mousePoint = GetMousePosition();
    bool mouseHover = CheckCollisionPointRec(mousePoint, bounds);
    bool pressed = false;
    
    // Determine button color based on state
    Color buttonColor = normalColor;
    if (mouseHover) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            buttonColor = pressedColor;
        } else {
            buttonColor = hoverColor;
        }
        
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            pressed = true;
        }
    }
    
    // Draw button
    DrawRectangleRec(bounds, buttonColor);
    
    // Calculate text position
    int textWidth = MeasureText(text, 10);
    int textX = bounds.x + (bounds.width - textWidth) / 2;
    int textY = bounds.y + (bounds.height - 10) / 2;
    
    // Draw text
    DrawText(text, textX, textY, 10, UI_TEXT_PRIMARY);
    
    return pressed;
}

bool CollapsiblePanel(Rectangle bounds, const char* title, bool* isOpen, Color headerColor, Color bodyColor) {
    bool clicked = false;
    
    // Draw panel header
    DrawRectangle(bounds.x, bounds.y, bounds.width, 30, headerColor);
    
    // Draw panel title
    DrawText(title, bounds.x + 10, bounds.y + 10, 10, UI_TEXT_PRIMARY);
    
    // Draw toggle button
    Rectangle toggleBounds = {bounds.x + bounds.width - 25, bounds.y + 5, 20, 20};
    DrawRectangleRec(toggleBounds, UI_SECONDARY);
    
    // Draw +/- symbol based on panel state
    DrawText(*isOpen ? "-" : "+", toggleBounds.x + 7, toggleBounds.y + 5, 10, UI_TEXT_PRIMARY);
    
    // Check for toggle button click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), toggleBounds)) {
        *isOpen = !(*isOpen);
        clicked = true;
    }
    
    // Draw panel body if open
    if (*isOpen) {
        DrawRectangle(bounds.x, bounds.y + 30, bounds.width, bounds.height - 30, bodyColor);
    }
    
    return clicked;
}

float CustomSlider(Rectangle bounds, const char* label, float value, float min, float max, Color sliderColor) {
    // Draw label
    DrawText(label, bounds.x, bounds.y - 15, 10, UI_TEXT_PRIMARY);
    
    // Calculate slider position
    float percentage = (value - min) / (max - min);
    float handlePos = bounds.x + percentage * bounds.width;
    
    // Draw slider track
    DrawRectangle(bounds.x, bounds.y + (bounds.height / 2) - 1, bounds.width, 2, UI_TEXT_SECONDARY);
    
    // Draw slider handle
    DrawRectangle(handlePos - 5, bounds.y, 10, bounds.height, sliderColor);
    
    // Handle interaction
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), bounds)) {
        float newPercentage = (GetMouseX() - bounds.x) / bounds.width;
        newPercentage = newPercentage < 0.0f ? 0.0f : (newPercentage > 1.0f ? 1.0f : newPercentage);
        value = min + (max - min) * newPercentage;
    }
    
    // Draw value text
    char valueText[32];
    sprintf(valueText, "%.2f", value);
    DrawText(valueText, bounds.x + bounds.width + 10, bounds.y + (bounds.height / 2) - 5, 10, UI_TEXT_SECONDARY);
    
    return value;
}

Vector3 PropertyEditor(Rectangle bounds, const char* label, Vector3 value) {
    Vector3 result = value;
    
    // Draw property label
    DrawText(label, bounds.x, bounds.y, 10, UI_TEXT_PRIMARY);
    
    int componentWidth = (bounds.width - 30) / 3;
    int yOffset = 20;
    
    // X component
    DrawText("X:", bounds.x, bounds.y + yOffset, 10, ACCENT_PRIMARY);
    Rectangle xBounds = {bounds.x + 20, bounds.y + yOffset - 2, componentWidth, 15};
    
    // Y component
    DrawText("Y:", bounds.x + componentWidth + 30, bounds.y + yOffset, 10, ACCENT_SECONDARY);
    Rectangle yBounds = {bounds.x + componentWidth + 50, bounds.y + yOffset - 2, componentWidth, 15};
    
    // Z component
    DrawText("Z:", bounds.x + 2 * componentWidth + 60, bounds.y + yOffset, 10, ACCENT_TERTIARY);
    Rectangle zBounds = {bounds.x + 2 * componentWidth + 80, bounds.y + yOffset - 2, componentWidth, 15};
    
    // Draw text boxes for values (simplified - would use GuiTextBox in actual implementation)
    DrawRectangleRec(xBounds, UI_SECONDARY);
    DrawRectangleRec(yBounds, UI_SECONDARY);
    DrawRectangleRec(zBounds, UI_SECONDARY);
    
    // Format and draw current values
    char xText[32], yText[32], zText[32];
    sprintf(xText, "%.2f", result.x);
    sprintf(yText, "%.2f", result.y);
    sprintf(zText, "%.2f", result.z);
    
    DrawText(xText, xBounds.x + 5, xBounds.y + 2, 10, UI_TEXT_PRIMARY);
    DrawText(yText, yBounds.x + 5, yBounds.y + 2, 10, UI_TEXT_PRIMARY);
    DrawText(zText, zBounds.x + 5, zBounds.y + 2, 10, UI_TEXT_PRIMARY);
    
    // In a real implementation, would handle input and parse values
    
    return result;
}

int TabBar(Rectangle bounds, const char** names, int count, int* active) {
    int tabWidth = bounds.width / count;
    
    for (int i = 0; i < count; i++) {
        Rectangle tabBounds = {bounds.x + i * tabWidth, bounds.y, tabWidth, bounds.height};
        
        // Draw tab background
        Color tabColor = (i == *active) ? ACCENT_PRIMARY : UI_SECONDARY;
        DrawRectangleRec(tabBounds, tabColor);
        
        // Draw tab name
        int textWidth = MeasureText(names[i], 10);
        DrawText(names[i], tabBounds.x + (tabWidth - textWidth) / 2, tabBounds.y + 10, 10, UI_TEXT_PRIMARY);
        
        // Check for click
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), tabBounds)) {
            *active = i;
        }
    }
    
    return *active;
}

bool ToolButton(Rectangle bounds, Texture2D icon, const char* tooltip, bool isSelected) {
    bool pressed = false;
    Color bgColor = isSelected ? ACCENT_PRIMARY : UI_SECONDARY;
    
    // Draw background
    DrawRectangleRec(bounds, bgColor);
    
    // Draw icon
    // If icon is valid, draw it centered
    if (icon.id > 0) {
        DrawTexture(
            icon, 
            bounds.x + (bounds.width - icon.width) / 2, 
            bounds.y + (bounds.height - icon.height) / 2, 
            WHITE
        );
    }
    
    // Check for hover and tooltip
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, bounds)) {
        // Show tooltip
        int tooltipWidth = MeasureText(tooltip, 10) + 10;
        DrawRectangle(bounds.x + bounds.width, bounds.y + bounds.height/2 - 10, tooltipWidth, 20, UI_SECONDARY);
        DrawText(tooltip, bounds.x + bounds.width + 5, bounds.y + bounds.height/2 - 5, 10, UI_TEXT_PRIMARY);
        
        // Check for click
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            pressed = true;
        }
    }
    
    return pressed;
}

bool AssetTile(Rectangle bounds, Texture2D texture, const char* name, bool isSelected) {
    bool clicked = false;
    
    // Draw tile background
    Color bgColor = isSelected ? ACCENT_TERTIARY : UI_SECONDARY;
    DrawRectangleRec(bounds, bgColor);
    
    // Draw asset preview (texture)
    if (texture.id > 0) {
        // Scale texture to fit within bounds while maintaining aspect ratio
        float scale = fmin(bounds.width / texture.width, bounds.height / texture.height);
        float scaledWidth = texture.width * scale;
        float scaledHeight = texture.height * scale;
        
        DrawTexturePro(
            texture,
            {0, 0, (float)texture.width, (float)texture.height},
            {bounds.x + (bounds.width - scaledWidth) / 2, bounds.y + (bounds.height - scaledHeight) / 2 - 10, scaledWidth, scaledHeight},
            {0, 0},
            0,
            WHITE
        );
    } else {
        // Placeholder when no texture
        DrawRectangle(bounds.x + 5, bounds.y + 5, bounds.width - 10, bounds.height - 25, UI_TEXT_SECONDARY);
    }
    
    // Draw asset name at bottom
    int textWidth = MeasureText(name, 9);
    DrawText(name, bounds.x + (bounds.width - textWidth) / 2, bounds.y + bounds.height - 15, 9, UI_TEXT_PRIMARY);
    
    // Check for click
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), bounds)) {
        clicked = true;
    }
    
    return clicked;
}

bool MenuItem(Rectangle bounds, const char* text, bool* showSubmenu) {
    bool clicked = false;
    Vector2 mousePos = GetMousePosition();
    bool mouseHover = CheckCollisionPointRec(mousePos, bounds);
    
    // Draw menu item background
    Color bgColor = mouseHover ? ACCENT_PRIMARY : UI_PRIMARY;
    DrawRectangleRec(bounds, bgColor);
    
    // Draw menu item text
    DrawText(text, bounds.x + 10, bounds.y + (bounds.height - 10) / 2, 10, UI_TEXT_PRIMARY);
    
    // Check for click
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && mouseHover) {
        clicked = true;
        *showSubmenu = !(*showSubmenu);
    }
    
    return clicked;
}

int Submenu(Rectangle bounds, const char** items, int count) {
    int selected = -1;
    
    // Draw submenu background
    DrawRectangleRec(bounds, UI_SECONDARY);
    DrawRectangleLinesEx(bounds, 1, UI_PRIMARY);
    
    // Draw items
    for (int i = 0; i < count; i++) {
        Rectangle itemBounds = {bounds.x, bounds.y + i * 25, bounds.width, 25};
        
        // Check for hover
        bool hover = CheckCollisionPointRec(GetMousePosition(), itemBounds);
        
        // Draw item background
        if (hover) {
            DrawRectangleRec(itemBounds, ACCENT_TERTIARY);
        }
        
        // Draw item text
        DrawText(items[i], itemBounds.x + 10, itemBounds.y + 7, 10, UI_TEXT_PRIMARY);
        
        // Check for click
        if (hover && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            selected = i;
        }
    }
    
    return selected;
}

bool SearchBar(Rectangle bounds, char* text, int textSize, bool* active) {
    bool textChanged = false;
    
    // Draw search bar background
    DrawRectangleRec(bounds, *active ? UI_SECONDARY : UI_PRIMARY);
    DrawRectangleLinesEx(bounds, 1, *active ? ACCENT_TERTIARY : UI_SECONDARY);
    
    // Draw current text
    DrawText(text, bounds.x + 25, bounds.y + (bounds.height - 10) / 2, 10, UI_TEXT_PRIMARY);
    
    // Draw search icon
    DrawText("🔍", bounds.x + 5, bounds.y + (bounds.height - 10) / 2, 10, UI_TEXT_SECONDARY);
    
    // Check for click to activate
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        *active = CheckCollisionPointRec(GetMousePosition(), bounds);
    }
    
    // Handle text input if active
    if (*active) {
        int key = GetCharPressed();
        while (key > 0) {
            // Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (strlen(text) < textSize - 1)) {
                int len = strlen(text);
                text[len] = (char)key;
                text[len + 1] = '\0';
                textChanged = true;
            }
            key = GetCharPressed();
        }
        
        // Handle backspace
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(text);
            if (len > 0) {
                text[len - 1] = '\0';
                textChanged = true;
            }
        }
    }
    
    return textChanged;
}

int CustomDropdown(Rectangle bounds, const char *text, int active, const char **items, int count, bool *editMode) {
    // Default implementation that mimics a basic dropdown
    bool clicked = false;
    int result = active;
    
    // Draw the main dropdown box
    DrawRectangleRec(bounds, UI_SECONDARY);
    DrawRectangleLinesEx(bounds, 1, UI_PRIMARY);
    
    // Draw selected text
    DrawText(text, bounds.x + 10, bounds.y + (bounds.height - 10) / 2, 10, UI_TEXT_PRIMARY);
    
    // Draw dropdown arrow
    DrawText("▼", bounds.x + bounds.width - 15, bounds.y + (bounds.height - 10) / 2, 10, UI_TEXT_PRIMARY);
    
    // Handle click to open/close dropdown
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), bounds)) {
        *editMode = !(*editMode);
        clicked = true;
    }
    
    // If dropdown is open, draw items
    if (*editMode) {
        Rectangle itemsArea = {
            bounds.x,
            bounds.y + bounds.height,
            bounds.width,
            count * 25.0f
        };
        
        // Draw dropdown list background
        DrawRectangleRec(itemsArea, UI_SECONDARY);
        DrawRectangleLinesEx(itemsArea, 1, UI_PRIMARY);
        
        // Draw each item
        for (int i = 0; i < count; i++) {
            Rectangle itemBounds = {
                itemsArea.x,
                itemsArea.y + i * 25,
                itemsArea.width,
                25
            };
            
            // Highlight selected or hovered item
            bool isHovered = CheckCollisionPointRec(GetMousePosition(), itemBounds);
            bool isSelected = (i == active);
            
            if (isSelected || isHovered) {
                DrawRectangleRec(itemBounds, isHovered ? ACCENT_TERTIARY : ACCENT_SECONDARY);
            }
            
            // Draw item text
            DrawText(items[i], itemBounds.x + 10, itemBounds.y + 8, 10, UI_TEXT_PRIMARY);
            
            // Handle item selection
            if (isHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                result = i;
                *editMode = false;
            }
        }
        
        // Close dropdown if clicked outside
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(GetMousePosition(), itemsArea) && !CheckCollisionPointRec(GetMousePosition(), bounds)) {
            *editMode = false;
        }
    }
    
    return result;
}

} // namespace UI