#include "raylib.h"
#include "rlgl.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <iostream>
#include <fstream>
#include <filesystem>

#include "AnimateImage2D.hpp"

bool saveAssetFile(Model& previewModel, AnimatedSprite sprite, std::string inputFileName, float modelScale, AssetType currentAsset, std::string filePath)
{
    std::string fileName = std::filesystem::path(inputFileName).filename().string();
    std::string savePath = "ressources/loadedAssets/" + fileName + ".txt";
    std::ofstream outFile(savePath);

    if (outFile.is_open()) {
        if (currentAsset == AssetType::ASSET3D) {
            BoundingBox bounds = GetModelBoundingBox(previewModel);
            Vector3 modelDimensions = {
                bounds.max.x - bounds.min.x,
                bounds.max.y - bounds.min.y,
                bounds.max.z - bounds.min.z
            };
            outFile << "Type: 3D\n";
            outFile << "Name: " << inputFileName << "\n";
            outFile << "File: " << filePath << "\n";
            outFile << "Scale: " << modelScale << "\n";
        } else if (currentAsset == AssetType::ASSET2D) {
            outFile << "Type: 2D\n";
            outFile << "Name: " << inputFileName << "\n";
            outFile << "File: " << filePath << "\n";
            outFile << "Scale: " << modelScale << "\n";
            outFile << "Size: "
                    << sprite.getFrameWidth() << " x "
                    << sprite.getFrameHeight()<< "\n";
            outFile << "Frames: " << sprite.getTotalFrames() << "\n";
        }
        outFile.close();
        std::cout << "Saved asset info to: " << savePath << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to write file: " << savePath << std::endl;
        return false;
    }
}

void drawModelPreview(Model& model, Rectangle viewport, int screenHeight, float scale)
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

void drawImagePreview(AnimatedSprite& sprite, Rectangle viewport, float modelScale)
{
    int spriteWidth = sprite.getFrameWidth() * modelScale;
    int spriteHeight = sprite.getFrameHeight() * modelScale;

    Vector2 position = {
        viewport.x + (viewport.width - spriteWidth) / 2.0f,
        viewport.y + (viewport.height - spriteHeight) / 2.0f
    };
    sprite.updateAnimation();
    sprite.draw(position, modelScale);
}

int main()
{
    Rectangle previewRect = { 100, 100, 300, 300 };
    Model previewModel = {0};
    bool dataLoaded = false;
    std::string selectedFilePath;
    float modelScale = 0.5f;
    char scaleInput[32] = "0.5";
    bool editMode = false;
    AssetType currentAsset;
    AnimatedSprite sprite;
    int frameWidth = 32;
    int frameHeight = 40;
    int totalFrames = 4;

    InitWindow(800, 600, "GLB Model Preview");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (GuiButton((Rectangle){ 50, 50, 120, 30 }, "Open Asset File")) {
            FILE* f = popen("zenity --file-selection --file-filter='GLB files & Image files | *.glb *.png *.jpg *.jpeg'", "r");
            if (f) {
                char path[1024];
                if (fgets(path, sizeof(path), f)) {
                    size_t len = strlen(path);

                    if (len > 0 && path[len-1] == '\n') path[len-1] = '\0';

                    std::filesystem::path filePath(path);
                    std::string extension = filePath.extension().string();

                    if (dataLoaded && currentAsset == AssetType::ASSET3D) {
                        UnloadModel(previewModel);
                    } else if (dataLoaded && currentAsset == AssetType::ASSET2D) {
                        sprite.unload();
                    }
                    if (extension == ".glb") {
                        previewModel = LoadModel(path);

                        BoundingBox bounds = GetModelBoundingBox(previewModel);
                        Vector3 dims = {
                            bounds.max.x - bounds.min.x,
                            bounds.max.y - bounds.min.y,
                            bounds.max.z - bounds.min.z
                        };
                        float maxDim = fmaxf(dims.x, fmaxf(dims.y, dims.z));
                        modelScale = (maxDim > 0.0f) ? 1.0f / maxDim : 1.0f;
                        snprintf(scaleInput, sizeof(scaleInput), "%.3f", modelScale);
                        std::cout << "Auto-scaled to: " << modelScale << " to normalize model size\n";
                        currentAsset = AssetType::ASSET3D;
                    } else {
                        sprite.load(path, frameWidth, frameHeight, totalFrames);

                        float maxDim = fmaxf(frameWidth, frameHeight);
                        modelScale = (maxDim > 0.0f) ? 300.0f / maxDim : 1.0f;
                        snprintf(scaleInput, sizeof(scaleInput), "%.3f", modelScale);

                        std::cout << "Loaded 2D image: " << path << " (" << frameWidth << "x" << frameHeight << ")\n";
                        currentAsset = AssetType::ASSET2D;
                    }

                    dataLoaded = true;
                    selectedFilePath = path;
                }
                pclose(f);
            }
        }
        DrawRectangleLines((int)previewRect.x, (int)previewRect.y, (int)previewRect.width, (int)previewRect.height, DARKGRAY);
        if (dataLoaded) {
            if (currentAsset == AssetType::ASSET3D) {
                drawModelPreview(previewModel, previewRect, GetScreenHeight(), modelScale);
            } else if (currentAsset == AssetType::ASSET2D) {
                GuiSpinner((Rectangle){ 450, 130, 120, 30 }, "", &frameWidth, 1, 120, 0);
                GuiSpinner((Rectangle){ 450, 160, 120, 30 }, "", &frameHeight, 1, 120, 0);
                GuiSpinner((Rectangle){ 450, 190, 120, 30 }, "", &totalFrames, 1, 16, 0);
                if (frameWidth != sprite.getFrameWidth() || frameHeight != sprite.getFrameHeight() || totalFrames != sprite.getTotalFrames())
                    sprite.load(selectedFilePath, frameWidth, frameHeight, totalFrames);
                drawImagePreview(sprite, previewRect, modelScale);
            }

            if (GuiButton((Rectangle){ 450, 100, 200, 30 }, "Save Asset Info")) {
                FILE* f = popen("zenity --entry --title='Save Asset' --text='Enter filename to save asset info:'", "r");
                if (f) {
                    char inputFileName[256] = { 0 };
                    if (fgets(inputFileName, sizeof(inputFileName), f)) {
                        size_t len = strlen(inputFileName);
                        if (len > 0 && inputFileName[len - 1] == '\n') inputFileName[len - 1] = '\0';
                        if (saveAssetFile(previewModel, sprite, inputFileName, modelScale, currentAsset, selectedFilePath)) {
                            pclose(f);
                            break;
                        }
                    }
                    pclose(f);
                }
            }
        }
        else {
            DrawText("No model loaded", 160, 150, 20, GRAY);
        }
        EndDrawing();
    }

    if (dataLoaded && currentAsset == AssetType::ASSET3D) {
        UnloadModel(previewModel);
    } else if (dataLoaded && currentAsset == AssetType::ASSET2D) {
        sprite.unload();
    }
    CloseWindow();
    return 0;
}
