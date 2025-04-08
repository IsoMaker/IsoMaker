/*
** EPITECH PROJECT, 2025
** IsoMaker
** File description:
** Grid
*/

#include "Grid.hpp"

MapGrid::MapGrid(int gridSize, int cellSize, Color gridColor, Color backgroundColor)
{
    _gridSize = gridSize;
    _cellSize = cellSize;
    _gridColor = gridColor;
    _gridBackgroundColor = backgroundColor;
    _gridMesh = { 0 };
    _gridModel = { 0 };
}

MapGrid::~MapGrid()
{
    if (_gridMesh.vertices) {
        MemFree(_gridMesh.vertices);
        _gridMesh.vertices = nullptr;
    }
    if (_gridMesh.normals) {
        MemFree(_gridMesh.normals);
        _gridMesh.normals = nullptr;
    }
    if (_gridMesh.texcoords) {
        MemFree(_gridMesh.texcoords);
        _gridMesh.texcoords = nullptr;
    }
    if (_gridMesh.indices) {
        MemFree(_gridMesh.indices);
        _gridMesh.indices = nullptr;
    }
}

void MapGrid::init()
{
    int vertexCount = _gridSize * _gridSize;
    int indexCount = (_gridSize - 1) * (_gridSize - 1) * 6;

    Vector3 *vertices = (Vector3 *)MemAlloc(vertexCount * sizeof(Vector3));
    Vector3 *normals = (Vector3 *)MemAlloc(vertexCount * sizeof(Vector3));
    Vector2 *texcoords = (Vector2 *)MemAlloc(vertexCount * sizeof(Vector2));
    unsigned short *indices = (unsigned short *)MemAlloc(indexCount * sizeof(unsigned short));

    float gridExtent = (_gridSize - 1) * (_cellSize * 0.5f); // Half grid width

    int v = 0, i = 0;
    for (int z = 0; z < _gridSize; z++) {
        for (int x = 0; x < _gridSize; x++) {
            vertices[v] = (Vector3){ x * _cellSize - gridExtent, 0.0f, z * _cellSize - gridExtent };
            normals[v] = (Vector3){ 0.0f, 1.0f, 0.0f };
            texcoords[v] = (Vector2){ (float)x / (_gridSize - 1), (float)z / (_gridSize - 1) };

            if (x < _gridSize - 1 && z < _gridSize - 1) {
                int topLeft = v;
                int topRight = v + 1;
                int bottomLeft = v + _gridSize;
                int bottomRight = v + _gridSize + 1;

                indices[i++] = topLeft;
                indices[i++] = bottomLeft;
                indices[i++] = topRight;
                indices[i++] = topRight;
                indices[i++] = bottomLeft;
                indices[i++] = bottomRight;
            }
            v++;
        }
    }

    _gridMesh = { 0 };
    _gridMesh.vertexCount = vertexCount;
    _gridMesh.triangleCount = indexCount / 3;

    _gridMesh.vertices = (float *)vertices;
    _gridMesh.normals = (float *)normals;
    _gridMesh.texcoords = (float *)texcoords;
    _gridMesh.indices = indices;

    UploadMesh(&_gridMesh, true);
    _gridModel = LoadModelFromMesh(_gridMesh);
}

void MapGrid::draw()
{
    drawMesh();
    drawWireframe();
}

void MapGrid::drawMesh()
{
    DrawModel(_gridModel, (Vector3){ 0, 0, 0 }, 1.0f, _gridColor);
}

void MapGrid::drawXAxis(float x, float y, float z)
{
    rlVertex3f(-x, y, z);
    rlVertex3f(x, y, z);
}

void MapGrid:: drawZAxis(float x, float y, float z)
{
    rlVertex3f(x, y, -z);
    rlVertex3f(x, y, z);
}

void MapGrid::drawLines(float gridExtent, float y, float offset)
{
    drawXAxis(gridExtent, y, offset);
    drawZAxis(offset, y, gridExtent);
}

void MapGrid::drawWireframe()
{
    rlPushMatrix();
    rlBegin(RL_LINES);
    rlColor4ub(_gridBackgroundColor.r, _gridBackgroundColor.g, _gridBackgroundColor.b, _gridBackgroundColor.a);

    float gridExtent = (_gridSize - 1) * (_cellSize * 0.5f); // Center the grid

    for (int i = 0; i < _gridSize; i++) {
        float offset = i * _cellSize - gridExtent;
        drawLines(gridExtent, 0.01f, offset);
    }

    rlEnd();
    rlPopMatrix();
}
