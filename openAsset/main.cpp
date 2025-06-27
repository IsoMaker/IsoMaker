#include "raylib.h"
#include "rlgl.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <iostream>
#include <fstream>
#include <filesystem>

enum class AssetType { None, Model3D, Image2D };

void saveAssetFile(Model& previewModel, Image& previewImage, std::string selectedFilePath, float modelScale, AssetType currentAsset)
{
    std::string fileName = std::filesystem::path(selectedFilePath).filename().string();
    std::string savePath = "ressources/loadedAssets/" + fileName + ".txt";
    std::ofstream outFile(savePath);

    if (outFile.is_open()) {
        if (currentAsset == AssetType::Model3D) {
            BoundingBox bounds = GetModelBoundingBox(previewModel);
            Vector3 modelDimensions = {
                bounds.max.x - bounds.min.x,
                bounds.max.y - bounds.min.y,
                bounds.max.z - bounds.min.z
            };
            outFile << "Type: 3D\n";
            outFile << "File: " << selectedFilePath << "\n";
            outFile << "Scale: " << modelScale << "\n";
            outFile << "Scaled Size: "
                    << modelDimensions.x * modelScale << " x "
                    << modelDimensions.y * modelScale << " x "
                    << modelDimensions.z * modelScale << "\n";
        } else if (currentAsset == AssetType::Image2D) {
            outFile << "Type: 2D\n";
            outFile << "File: " << selectedFilePath << "\n";
            outFile << "Scale: " << modelScale << "\n";
            outFile << "Scaled Size: "
                    << previewImage.width * modelScale << " x "
                    << previewImage.height * modelScale << "\n";
        }

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
    bool dataLoaded = false;
    std::string selectedFilePath;
    float modelScale = 0.5f;
    char scaleInput[32] = "0.5";
    bool editMode = false;
    Image previewImage = {0};
    Texture2D previewTexture = {0};
    AssetType currentAsset = AssetType::None;

    InitWindow(800, 600, "GLB Model Preview");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (GuiButton((Rectangle){ 50, 50, 120, 30 }, "Open GLB File")) {
            FILE* f = popen("zenity --file-selection --file-filter='GLB files & Image files | *.glb *.png *.jpg *.jpeg'", "r");
            if (f) {
                char path[1024];
                if (fgets(path, sizeof(path), f)) {
                    size_t len = strlen(path);

                    if (len > 0 && path[len-1] == '\n') path[len-1] = '\0';

                    std::filesystem::path filePath(path);
                    std::string extension = filePath.extension().string();

                    if (dataLoaded && currentAsset == AssetType::Model3D) {
                        UnloadModel(previewModel);
                    } else if (dataLoaded && currentAsset == AssetType::Image2D) {
                        UnloadTexture(previewTexture);
                        UnloadImage(previewImage);
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
                        currentAsset = AssetType::Model3D;
                    } else {
                        previewImage = LoadImage(path);
                        previewTexture = LoadTextureFromImage(previewImage);

                        int imgWidth = previewImage.width;
                        int imgHeight = previewImage.height;
                        float maxDim = fmaxf(imgWidth, imgHeight);
                        modelScale = (maxDim > 0.0f) ? 300.0f / maxDim : 1.0f;  // Fit into previewRect
                        snprintf(scaleInput, sizeof(scaleInput), "%.3f", modelScale);

                        std::cout << "Loaded 2D image: " << path << " (" << imgWidth << "x" << imgHeight << ")\n";
                        currentAsset = AssetType::Image2D;
                    }

                    dataLoaded = true;
                    selectedFilePath = path;
                }
                pclose(f);
            }
        }

        if (dataLoaded) {
            if (currentAsset == AssetType::Model3D) {
                DrawModelPreview(previewModel, previewRect, GetScreenHeight(), modelScale);
            } else if (currentAsset == AssetType::Image2D) {
                float scaledWidth = previewImage.width * modelScale;
                float scaledHeight = previewImage.height * modelScale;

                Rectangle src = { 0, 0, (float)previewImage.width, (float)previewImage.height };
                Rectangle dest = {
                    previewRect.x + (previewRect.width - scaledWidth) / 2,
                    previewRect.y + (previewRect.height - scaledHeight) / 2,
                    scaledWidth,
                    scaledHeight
                };
                Vector2 origin = { 0, 0 };
                DrawTexturePro(previewTexture, src, dest, origin, 0.0f, WHITE);
            }
            DrawRectangleLines((int)previewRect.x, (int)previewRect.y, (int)previewRect.width, (int)previewRect.height, DARKGRAY);

            if (GuiButton((Rectangle){ 450, 160, 200, 30 }, "Save Asset Info")) {
                FILE* f = popen("zenity --entry --title='Save Asset' --text='Enter filename to save asset info:'", "r");
                if (f) {
                    char inputFileName[256] = { 0 };
                    if (fgets(inputFileName, sizeof(inputFileName), f)) {
                        size_t len = strlen(inputFileName);
                        if (len > 0 && inputFileName[len - 1] == '\n') inputFileName[len - 1] = '\0';
                        saveAssetFile(previewModel, previewImage, inputFileName, modelScale, currentAsset);
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

    if (dataLoaded && currentAsset == AssetType::Model3D) {
        UnloadModel(previewModel);
    } else if (dataLoaded && currentAsset == AssetType::Image2D) {
        UnloadTexture(previewTexture);
        UnloadImage(previewImage);
    }
    CloseWindow();
    return 0;
}
