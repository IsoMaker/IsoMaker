#include "raylib.h"
#include "rlgl.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <iostream>
#include <fstream>
#include <filesystem>


void saveFile(Model& previewModel, std::string selectedFilePath, float modelScale)
{
    BoundingBox bounds = GetModelBoundingBox(previewModel);
    Vector3 modelDimensions = {
        bounds.max.x - bounds.min.x,
        bounds.max.y - bounds.min.y,
        bounds.max.z - bounds.min.z
    };

    std::string fileName = std::filesystem::path(selectedFilePath).filename().string();
    std::string savePath = "ressources/loadedAssets/" + fileName + ".txt";
    std::ofstream outFile(savePath);

    if (outFile.is_open()) {
        outFile << "File: " << selectedFilePath << "\n";
        outFile << "Scale: " << modelScale << "\n";
        outFile << "Scaled Size: "
                << modelDimensions.x * modelScale << " x "
                << modelDimensions.y * modelScale << " x "
                << modelDimensions.z * modelScale << "\n";

        outFile.close();
        std::cout << "Saved asset info to: " << savePath << std::endl;
    }
    else {
        std::cerr << "Failed to write file: " << savePath << std::endl;
    }
}

void DrawModelPreview(Model& model, Rectangle viewport, int screenHeight, float scale)
{
    BeginScissorMode((int)viewport.x, (int)viewport.y, (int)viewport.width, (int)viewport.height);
    rlViewport(viewport.x, screenHeight - (viewport.y + viewport.height), viewport.width, viewport.height);

    Camera cam = { 0 };
    cam.position = { 2.0f, 2.0f, 2.0f };
    cam.target = { 0.0f, 0.5f, 0.0f };
    cam.up = { 0.0f, 1.0f, 0.0f };
    cam.fovy = 45.0f;
    cam.projection = CAMERA_PERSPECTIVE;

    BeginMode3D(cam);

    Vector3 pos = { 0.0f, 0.0f, 0.0f };
    Vector3 rotAxis = { 0.0f, 1.0f, 0.0f };
    float angle = GetTime() * 45.0f;

    DrawModelEx(model, pos, rotAxis, angle, {scale, scale, scale}, WHITE);

    EndMode3D();

    rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());
    rlLoadIdentity();
    EndScissorMode();
}

int main()
{
    Rectangle previewRect = { 100, 100, 300, 300 };
    Model previewModel = {0};
    bool modelLoaded = false;
    std::string selectedFilePath;
    float modelScale = 0.5f;
    char scaleInput[32] = "0.5";
    bool editMode = false;

    InitWindow(800, 600, "GLB Model Preview");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (GuiButton((Rectangle){ 50, 50, 120, 30 }, "Open GLB File")) {
            FILE* f = popen("zenity --file-selection --file-filter='GLB files | *.glb'", "r");
            if (f)
            {
                char path[1024];
                if (fgets(path, sizeof(path), f))
                {
                    size_t len = strlen(path);
                    if (len > 0 && path[len-1] == '\n') path[len-1] = '\0';

                    if (modelLoaded) UnloadModel(previewModel);

                    previewModel = LoadModel(path);
                    modelLoaded = true;
                    selectedFilePath = path;

                    BoundingBox bounds = GetModelBoundingBox(previewModel);
                    Vector3 dims = {
                        bounds.max.x - bounds.min.x,
                        bounds.max.y - bounds.min.y,
                        bounds.max.z - bounds.min.z
                    };
                    float maxDim = fmaxf(dims.x, fmaxf(dims.y, dims.z));
                    modelScale = (maxDim > 0.0f) ? 1.0f / maxDim : 1.0f;
                    snprintf(scaleInput, sizeof(scaleInput), "%.3f", modelScale); // Optional: update string value if used in a text box
                    std::cout << "Auto-scaled to: " << modelScale << " to normalize model size\n";
                }
                pclose(f);
            }
        }

        if (modelLoaded) {
            DrawModelPreview(previewModel, previewRect, GetScreenHeight(), modelScale);
            DrawRectangleLines((int)previewRect.x, (int)previewRect.y, (int)previewRect.width, (int)previewRect.height, DARKGRAY);

            if (GuiButton((Rectangle){ 450, 160, 200, 30 }, "Save Asset Info")) {
                FILE* f = popen("zenity --entry --title='Save Asset' --text='Enter filename to save asset info:'", "r");
                if (f) {
                    char inputFileName[256] = { 0 };
                    if (fgets(inputFileName, sizeof(inputFileName), f)) {
                        size_t len = strlen(inputFileName);
                        if (len > 0 && inputFileName[len - 1] == '\n') inputFileName[len - 1] = '\0';
                        saveFile(previewModel, inputFileName, modelScale);
                    }
                    pclose(f);
                }
            }
        }
        else {
            DrawText("No model loaded", 120, 220, 20, GRAY);
        }

        EndDrawing();
    }

    if (modelLoaded) UnloadModel(previewModel);
    CloseWindow();

    return 0;
}
