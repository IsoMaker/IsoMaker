/**
 * @file MainUI.hpp
 * @brief Main application UI controller for the IsoMaker game engine editor
 * @author IsoMaker Team
 * @version 0.1
 */

#pragma once

// #include "../Editor/2D/2DEditor.hpp"
#include "../Editor/3DMap/3DMapEditor.hpp"
#include "Entities/BasicObject2D.hpp"
#include "Input/MouseKeyboard.hpp"
#include "../UI/UIManager.hpp"
#include <iostream>

/** @brief Default screen height for the application window */
#define SCREENHEIGHT 1200
/** @brief Default screen width for the application window */
#define SCREENWIDTH 1600

// using namespace paint;

/**
 * @brief Editor type enumeration
 * 
 * Defines the different types of editors available in the application.
 */
enum EditorType {
    PAINT, ///< 2D pixel art editor mode
    MAP,   ///< 3D map editor mode
};

/**
 * @brief Main application UI controller class
 * 
 * The MainUI class serves as the central controller for the IsoMaker game engine editor.
 * It manages the main application window, camera, and orchestrates communication between
 * the UI system and various editor components. This class is responsible for the main
 * application loop and high-level coordination of all editor subsystems.
 * 
 * Key responsibilities:
 * - Main application lifecycle management
 * - Window and camera management
 * - Coordination between UI and editor components
 * - Input handling coordination
 * - Main rendering loop
 * 
 * @see UIManager For UI-specific functionality
 * @see MapEditor For 3D map editing capabilities
 */
class MainUI {
    public:
        /**
         * @brief Construct a new MainUI object
         * 
         * Initializes the main UI controller, setting up the window, camera,
         * editors, and UI manager components.
         */
        MainUI();
        
        /**
         * @brief Destroy the MainUI object
         * 
         * Cleanup is handled automatically by member destructors.
         */
        ~MainUI() {};

        /**
         * @brief Update the main application state
         * 
         * Updates all subsystems including UI manager and editors based on
         * current input state. This method is called once per frame.
         * 
         * @param inputHandler Reference to the input handler for processing user input
         */
        void update(input::IHandlerBase &inputHandler);
        
        /**
         * @brief Render the main application
         * 
         * Renders all visual components including the 3D scene, UI elements,
         * and editor-specific visualizations.
         */
        void draw();
        
        /**
         * @brief Main application loop
         * 
         * Runs the primary application loop, handling events, updates, and rendering
         * until the application is closed. This is the entry point for the main
         * application execution.
         * 
         * @param inputHandler Reference to the input handler for processing user input
         */
        void loop(input::IHandlerBase &inputHandler);

    protected:
        Render::Camera _camera;              ///< Main 3D camera for scene rendering
        Render::Window _window;              ///< Application window manager
        MapEditor _3DMapEditor;              ///< 3D map editor instance
        std::string _gameProjectName;        ///< Name of the current game project
        UI::UIManager _uiManager;            ///< UI manager for all interface elements
};
