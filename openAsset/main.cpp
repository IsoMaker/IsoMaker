#include "raylib.h"
#include "rlgl.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include "Entities/MapElement.hpp"
#include "AnimateImage2D.hpp"

enum class AssetType {
    ASSET2D,
    ASSET3D
};

bool saveAssetFile(Model& Asset3D, std::string inputFileName, float scale, AssetType type, std::string filePath, std::shared_ptr<objects::Character> character)
{
    std::string fileName = std::filesystem::path(inputFileName).filename().string();
    std::string savePath = "ressources/loadedAssets/" + fileName + ".txt";
    std::ofstream outFile(savePath);

    if (outFile.is_open()) {
        if (type == AssetType::ASSET3D) {
            BoundingBox bounds = GetModelBoundingBox(Asset3D);
            Vector3 modelDimensions = {
                bounds.max.x - bounds.min.x,
                bounds.max.y - bounds.min.y,
                bounds.max.z - bounds.min.z
            };
            outFile << "Type: 3D\n";
            outFile << "Name: " << inputFileName << "\n";
            outFile << "File: " << filePath << "\n";
            outFile << "Scale: " << scale << "\n";
        } else if (type == AssetType::ASSET2D) {
            outFile << "Type: 2D\n";
            outFile << "Name: " << inputFileName << "\n";
            outFile << "File: " << filePath << "\n";
            outFile << "Scale: " << scale << "\n";
            outFile << "Size: "
                    << character->getBox2D().getRectangle().width << " x "
                    << character->getBox2D().getRectangle().height << "\n";
            outFile << "Frames: " << character->getTotalFrames() << "\n";
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

bool buttonSave(Model& Asset3D, float scale, AssetType type, std::string filePath, std::shared_ptr<objects::Character> character)
{
    if (GuiButton((Rectangle){ 450, 100, 200, 30 }, "Save Asset Info")) {
        FILE* f = popen("zenity --entry --title='Save Asset' --text='Enter filename to save asset info:'", "r");
        if (f) {
            char inputFileName[256] = { 0 };
            if (fgets(inputFileName, sizeof(inputFileName), f)) {
                size_t len = strlen(inputFileName);
                if (len > 0 && inputFileName[len - 1] == '\n') inputFileName[len - 1] = '\0';
                if (saveAssetFile(Asset3D, inputFileName, scale, type, filePath, character)) {
                    pclose(f);
                    return true;
                }
            }
            pclose(f);
        }
    }
    return false;
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

void unloadData(AssetType type, std::shared_ptr<objects::Character> character, Model &model)
{
    if (type == AssetType::ASSET3D) {
        UnloadModel(model);
    } else if (type == AssetType::ASSET2D) {
        UnloadTexture((character->getAsset2D().getTexture()));
    }
}

void drawImagePreview(std::shared_ptr<objects::Character> character, Rectangle viewport, float scale)
{
    character->updateAnimation();
    character->draw(viewport, 0);
}

void drawRectanglePreview(bool dataLoaded, Model& model, Rectangle previewRect, std::shared_ptr<objects::Character> character, float scale, std::string selectedFilePath, AssetType type)
{
    DrawRectangleLines((int)previewRect.x, (int)previewRect.y, (int)previewRect.width, (int)previewRect.height, DARKGRAY);
    if (dataLoaded) {
        if (type == AssetType::ASSET3D) {
            drawModelPreview(model, previewRect, GetScreenHeight(), scale);
        } else if (type == AssetType::ASSET2D) {
            int width = character->getBox2D().getRectangle().width;
            int height = character->getBox2D().getRectangle().height;
            int frames = character->getTotalFrames();

            GuiSpinner((Rectangle){ 450, 130, 120, 30 }, "Width", &(width), 1, 120, 0);
            GuiSpinner((Rectangle){ 450, 160, 120, 30 }, "Height", &(height), 1, 120, 0);
            GuiSpinner((Rectangle){ 450, 190, 120, 30 }, "Frames", &(frames), 1, 16, 0);
            Asset2D tmpAsset = character->getAsset2D();
            if (width != tmpAsset.getWidth() || height != tmpAsset.getHeight() || frames != character->getTotalFrames()) {
                character->setTotalFrames(frames);
                character->setMoving(true);
                character->setBox2DSize(Vector2D(width, height));
            }
            drawImagePreview(character, previewRect, scale);
        }

        buttonSave(model, scale, type, selectedFilePath, character);
    }
    else {
        DrawText("No model loaded", 160, 150, 20, GRAY);
    }
}

int main()
{
    Rectangle previewRect = { 100, 100, 300, 300 };
    Model model = {0};
    bool dataLoaded = false;
    std::string selectedFilePath;
    float scale = 0.5f;
    bool editMode = false;
    AssetType type;
    std::shared_ptr<objects::Character> character;

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

                    unloadData(type, character, model);
                    if (extension == ".glb") {
                        model = LoadModel(path);

                        BoundingBox bounds = GetModelBoundingBox(model);
                        Vector3 dims = {
                            bounds.max.x - bounds.min.x,
                            bounds.max.y - bounds.min.y,
                            bounds.max.z - bounds.min.z
                        };
                        char scaleInput[32];
                        float maxDim = fmaxf(dims.x, fmaxf(dims.y, dims.z));

                        scale = (maxDim > 0.0f) ? 1.0f / maxDim : 1.0f;
                        snprintf(scaleInput, sizeof(scaleInput), "%.3f", scale);
                        std::cout << "Auto-scaled to: " << scale << " to normalize model size\n";
                        type = AssetType::ASSET3D;
                    } else {
                        character = std::make_shared<objects::Character>(Asset2D(path), Vector3D(0,0,0), Vector2D(0, 0), Vector2D(32, 40));
                        character->setMoving(true);
                        type = AssetType::ASSET2D;
                    }

                    dataLoaded = true;
                    selectedFilePath = path;
                }
                pclose(f);
            }
        }
        drawRectanglePreview(dataLoaded, model, previewRect, character, scale, selectedFilePath, type);
        EndDrawing();
    }

    unloadData(type, character, model);
    CloseWindow();
    return 0;
}
