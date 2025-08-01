#include "UIComponents.hpp"
#include <cmath>
#include <cstring>


namespace UI {

bool CustomButton(Rectangle bounds, const char* text, Color normalColor, Color hoverColor, Color pressedColor) {
    Vector2 mousePoint = GetMousePosition();
    bool mouseHover = CheckCollisionPointRec(mousePoint, bounds);
    bool pressed = false;
    bool isPressed = mouseHover && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    
    if (mouseHover && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        pressed = true;
    }
    
    // Use enhanced button drawing
    DrawButton(bounds, text, normalColor, mouseHover, isPressed);
    
    return pressed;
}

bool CollapsiblePanel(Rectangle bounds, const char* title, bool* isOpen, Color headerColor, Color bodyColor) {
    bool clicked = false;
    
    // Draw panel with enhanced styling
    DrawPanel(bounds, "", bodyColor, PANEL_BACKGROUND, true);
    
    // Draw header section
    Rectangle headerBounds = {bounds.x, bounds.y, bounds.width, UI_PADDING_XLARGE + UI_PADDING_MEDIUM};
    DrawRectangleRounded(headerBounds, UI_BORDER_RADIUS_MEDIUM, headerColor);
    
    // Draw panel title with better typography
    int textX = bounds.x + UI_PADDING_MEDIUM;
    int textY = bounds.y + UI_PADDING_MEDIUM;
    DrawText(title, textX, textY, UI_FONT_SIZE_MEDIUM, UI_TEXT_PRIMARY);
    
    // Draw enhanced toggle button
    Rectangle toggleBounds = {
        bounds.x + bounds.width - UI_PADDING_XLARGE - UI_PADDING_MEDIUM, 
        bounds.y + UI_PADDING_SMALL, 
        UI_PADDING_XLARGE, 
        UI_PADDING_XLARGE
    };
    
    bool toggleHovered = CheckCollisionPointRec(GetMousePosition(), toggleBounds);
    DrawButton(toggleBounds, *isOpen ? "−" : "+", ACCENT_PRIMARY, toggleHovered, false);
    
    // Check for toggle button click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && toggleHovered) {
        *isOpen = !(*isOpen);
        clicked = true;
    }
    
    // Draw panel body if open with rounded corners
    if (*isOpen) {
        Rectangle bodyBounds = {
            bounds.x + 1, 
            bounds.y + headerBounds.height, 
            bounds.width - 2, 
            bounds.height - headerBounds.height - 1
        };
        DrawRectangleRounded(bodyBounds, UI_BORDER_RADIUS_MEDIUM, bodyColor);
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

bool AssetTile(Rectangle bounds, Asset2D asset, bool isSelected, Vector2 position) {
    bool clicked = false;
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), bounds);

    // Draw shadow for depth
    Rectangle shadowBounds = {bounds.x + 2, bounds.y + 2, bounds.width, bounds.height};
    DrawRectangleRounded(shadowBounds, 0.12f, 8, SHADOW_LIGHT);

    // Draw main card background with modern styling
    Color bgColor = isSelected ? ACCENT_PRIMARY : (isHovered ? HOVER_BACKGROUND : PANEL_BACKGROUND);
    Color bgColorTop = isSelected ? ACCENT_SECONDARY : (isHovered ? UI_TERTIARY : UI_SECONDARY);
    
    DrawRectangleRounded(bounds, 0.12f, 8, bgColor);
    
    // Add subtle gradient effect at the top
    Rectangle gradientRect = {bounds.x, bounds.y, bounds.width, bounds.height * 0.25f};
    DrawRectangleGradientV(gradientRect.x, gradientRect.y, gradientRect.width, gradientRect.height, bgColorTop, bgColor);

    // Draw border with hover/selection states
    Color borderColor = isSelected ? ACCENT_TERTIARY : (isHovered ? ACCENT_PRIMARY : PANEL_BORDER);
    float borderWidth = isSelected ? 3.0f : (isHovered ? 2.0f : 1.0f);
    DrawRectangleLinesEx(bounds, borderWidth, borderColor);

    // Content area for texture (with padding)
    Rectangle contentArea = {bounds.x + 6, bounds.y + 6, bounds.width - 12, bounds.height - 28};
    
    if (asset.isLoaded()) {
        Texture2D texture = asset.getTexture();
        int texW = asset.getWidth();
        int texH = asset.getHeight();
        
        // Calculate centered and properly scaled texture positioning
        float aspectRatio = (float)texW / (float)texH;
        float contentAspect = contentArea.width / contentArea.height;
        
        Rectangle textureRect;
        if (aspectRatio > contentAspect) {
            // Fit to width
            textureRect.width = contentArea.width;
            textureRect.height = contentArea.width / aspectRatio;
            textureRect.x = contentArea.x;
            textureRect.y = contentArea.y + (contentArea.height - textureRect.height) / 2;
        } else {
            // Fit to height
            textureRect.height = contentArea.height;
            textureRect.width = contentArea.height * aspectRatio;
            textureRect.x = contentArea.x + (contentArea.width - textureRect.width) / 2;
            textureRect.y = contentArea.y;
        }
        
        Rectangle source = {0, 0, (float)texW, (float)texH};
        DrawTexturePro(texture, source, textureRect, {0, 0}, 0.0f, WHITE);
        
        // Add glow effect when selected
        if (isSelected) {
            DrawRectangleLinesEx(textureRect, 2, GLOW_ACCENT);
        }
    } else {
        // Enhanced loading placeholder
        DrawRectangleRounded(contentArea, 0.1f, 6, UI_TERTIARY);
        DrawRectangleLinesEx(contentArea, 1, UI_PRIMARY);
        
        // Loading indicator with better styling
        const char* loadingText = "Loading...";
        int loadingWidth = MeasureText(loadingText, 8);
        DrawText(loadingText, contentArea.x + (contentArea.width - loadingWidth) / 2, 
                contentArea.y + contentArea.height / 2 - 4, 8, UI_TEXT_SECONDARY);
    }

    // Enhanced asset name display
    const char* name = asset.getDisplayName().c_str();
    int textWidth = MeasureText(name, 10);
    Color textColor = isSelected ? WHITE : UI_TEXT_PRIMARY;
    
    // Semi-transparent background for text readability
    Rectangle textBg = {bounds.x + 2, bounds.y + bounds.height - 20, bounds.width - 4, 16};
    Color textBgColor = {0, 0, 0, 120};
    DrawRectangleRounded(textBg, 0.3f, 4, textBgColor);
    
    DrawText(name, bounds.x + (bounds.width - textWidth) / 2, bounds.y + bounds.height - 16, 10, textColor);

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), bounds)) {
        clicked = true;
    }

    return clicked;
}

bool AssetTile(Rectangle bounds, Model model, const char* name, bool isSelected) {
    bool clicked = false;
    bool isHovered = CheckCollisionPointRec(GetMousePosition(), bounds);

    // Draw shadow for depth
    Rectangle shadowBounds = {bounds.x + 2, bounds.y + 2, bounds.width, bounds.height};
    DrawRectangleRounded(shadowBounds, 0.12f, 8, SHADOW_LIGHT);

    // Draw main card background with modern styling
    Color bgColor = isSelected ? ACCENT_PRIMARY : (isHovered ? HOVER_BACKGROUND : PANEL_BACKGROUND);
    Color bgColorTop = isSelected ? ACCENT_SECONDARY : (isHovered ? UI_TERTIARY : UI_SECONDARY);
    
    DrawRectangleRounded(bounds, 0.12f, 8, bgColor);
    
    // Add subtle gradient effect at the top
    Rectangle gradientRect = {bounds.x, bounds.y, bounds.width, bounds.height * 0.25f};
    DrawRectangleGradientV(gradientRect.x, gradientRect.y, gradientRect.width, gradientRect.height, bgColorTop, bgColor);

    // Draw border with hover/selection states
    Color borderColor = isSelected ? ACCENT_TERTIARY : (isHovered ? ACCENT_PRIMARY : PANEL_BORDER);
    float borderWidth = isSelected ? 3.0f : (isHovered ? 2.0f : 1.0f);
    DrawRectangleLinesEx(bounds, borderWidth, borderColor);

    // 3D preview area with enhanced styling
    Rectangle previewArea = {bounds.x + 6, bounds.y + 6, bounds.width - 12, bounds.height - 28};
    
    // Draw inner preview frame
    Color previewBgColor = isSelected ? (Color){20, 25, 35, 255} : (Color){25, 30, 40, 255};
    DrawRectangleRounded(previewArea, 0.1f, 6, previewBgColor);
    
    // Subtle inner border for 3D preview area
    Color previewBorderColor = isSelected ? GLOW_ACCENT : UI_PRIMARY;
    DrawRectangleLinesEx(previewArea, 1, previewBorderColor);

    // Enhanced asset name display
    int textWidth = MeasureText(name, 10);
    Color textColor = isSelected ? WHITE : UI_TEXT_PRIMARY;
    
    // Semi-transparent background for text readability
    Rectangle textBg = {bounds.x + 2, bounds.y + bounds.height - 20, bounds.width - 4, 16};
    Color textBgColor = {0, 0, 0, 120};
    DrawRectangleRounded(textBg, 0.3f, 4, textBgColor);
    
    DrawText(name, bounds.x + (bounds.width - textWidth) / 2, bounds.y + bounds.height - 16, 10, textColor);

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

// Enhanced drawing helper functions for consistent styling
void DrawRectangleRounded(Rectangle rec, float radius, Color color) {
    // Simple rounded rectangle using multiple rectangles for corners
    // This is a basic implementation - could be enhanced with actual rounded corners
    
    if (radius <= 0) {
        DrawRectangleRec(rec, color);
        return;
    }
    
    // Clamp radius to reasonable bounds
    float maxRadius = fmin(rec.width, rec.height) / 2.0f;
    radius = fmin(radius, maxRadius);
    
    // Draw main rectangle
    DrawRectangle(
        rec.x + radius, rec.y,
        rec.width - 2 * radius, rec.height,
        color
    );
    
    // Draw left and right rectangles
    DrawRectangle(
        rec.x, rec.y + radius,
        radius, rec.height - 2 * radius,
        color
    );
    DrawRectangle(
        rec.x + rec.width - radius, rec.y + radius,
        radius, rec.height - 2 * radius,
        color
    );
    
    // Draw corner circles
    DrawCircle(rec.x + radius, rec.y + radius, radius, color);
    DrawCircle(rec.x + rec.width - radius, rec.y + radius, radius, color);
    DrawCircle(rec.x + radius, rec.y + rec.height - radius, radius, color);
    DrawCircle(rec.x + rec.width - radius, rec.y + rec.height - radius, radius, color);
}

void DrawRectangleRoundedWithShadow(Rectangle rec, float radius, Color color, float shadowOffset, Color shadowColor) {
    // Draw shadow first
    Rectangle shadowRec = {
        rec.x + shadowOffset,
        rec.y + shadowOffset,
        rec.width,
        rec.height
    };
    DrawRectangleRounded(shadowRec, radius, shadowColor);
    
    // Draw main rectangle
    DrawRectangleRounded(rec, radius, color);
}

void DrawPanel(Rectangle bounds, const char* title, Color headerColor, Color bodyColor, bool drawShadow) {
    if (drawShadow) {
        DrawRectangleRoundedWithShadow(bounds, UI_BORDER_RADIUS_MEDIUM, bodyColor, UI_SHADOW_OFFSET_SMALL, SHADOW_LIGHT);
    } else {
        DrawRectangleRounded(bounds, UI_BORDER_RADIUS_MEDIUM, bodyColor);
    }
    
    // Draw header if title is provided
    if (title && strlen(title) > 0) {
        Rectangle headerBounds = {bounds.x, bounds.y, bounds.width, UI_PADDING_XLARGE};
        DrawRectangleRounded(headerBounds, UI_BORDER_RADIUS_MEDIUM, headerColor);
        
        // Draw title text
        int textX = bounds.x + UI_PADDING_MEDIUM;
        int textY = bounds.y + (headerBounds.height - UI_FONT_SIZE_MEDIUM) / 2;
        DrawText(title, textX, textY, UI_FONT_SIZE_MEDIUM, UI_TEXT_PRIMARY);
    }
    
    // Draw border
    DrawRectangleLinesEx(bounds, 1, PANEL_BORDER);
}

void DrawButton(Rectangle bounds, const char* text, Color baseColor, bool isHovered, bool isPressed) {
    Color buttonColor = baseColor;
    float shadowOffset = UI_SHADOW_OFFSET_SMALL;
    
    if (isPressed) {
        buttonColor = ColorBrightness(baseColor, -0.2f);
        shadowOffset = 1.0f;
    } else if (isHovered) {
        buttonColor = ColorBrightness(baseColor, 0.1f);
    }
    
    DrawRectangleRoundedWithShadow(bounds, UI_BORDER_RADIUS_SMALL, buttonColor, shadowOffset, SHADOW_MEDIUM);
    
    // Draw text centered
    int textWidth = MeasureText(text, UI_FONT_SIZE_MEDIUM);
    int textX = bounds.x + (bounds.width - textWidth) / 2;
    int textY = bounds.y + (bounds.height - UI_FONT_SIZE_MEDIUM) / 2;
    DrawText(text, textX, textY, UI_FONT_SIZE_MEDIUM, UI_TEXT_PRIMARY);
}

void DrawInputField(Rectangle bounds, const char* text, bool isActive, bool isHovered) {
    Color backgroundColor = isActive ? UI_SECONDARY : UI_TERTIARY;
    Color borderColor = isActive ? ACCENT_PRIMARY : (isHovered ? HOVER_BACKGROUND : PANEL_BORDER);
    
    DrawRectangleRounded(bounds, UI_BORDER_RADIUS_SMALL, backgroundColor);
    DrawRectangleLinesEx(bounds, isActive ? 2 : 1, borderColor);
    
    // Draw text
    int textX = bounds.x + UI_PADDING_MEDIUM;
    int textY = bounds.y + (bounds.height - UI_FONT_SIZE_MEDIUM) / 2;
    DrawText(text, textX, textY, UI_FONT_SIZE_MEDIUM, UI_TEXT_PRIMARY);
}

} // namespace UI
