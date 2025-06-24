/*
** EPITECH PROJECT, 2025
** IsoMaker
** File description:
** Grid
*/

#include "Grid.hpp"

using namespace map;

MapGrid::MapGrid(int cellAmount, int cellSize, Color gridColor, Color backgroundColor)
{
    _cellAmount = cellAmount;
    _cellSize = cellSize;
    _color = gridColor;
    _backgroundColor = backgroundColor;
    _mesh = { 0 };
    _model = { 0 };
    _scale = 1.0f;
}

MapGrid::~MapGrid()
{
    if (_mesh.vertices) {
        MemFree(_mesh.vertices);
        _mesh.vertices = nullptr;
    }
    if (_mesh.normals) {
        MemFree(_mesh.normals);
        _mesh.normals = nullptr;
    }
    if (_mesh.texcoords) {
        MemFree(_mesh.texcoords);
        _mesh.texcoords = nullptr;
    }
    if (_mesh.indices) {
        MemFree(_mesh.indices);
        _mesh.indices = nullptr;
    }
}

void MapGrid::init()
{
    int vertexAmount = _cellAmount + 1;
    int vertexCount = vertexAmount * vertexAmount;
    int indexCount = _cellAmount * _cellAmount * 6;

    Vector3 *vertices = (Vector3 *)MemAlloc(vertexCount * sizeof(Vector3));
    Vector3 *normals = (Vector3 *)MemAlloc(vertexCount * sizeof(Vector3));
    Vector2 *texcoords = (Vector2 *)MemAlloc(vertexCount * sizeof(Vector2));
    unsigned short *indices = (unsigned short *)MemAlloc(indexCount * sizeof(unsigned short));

    float gridExtent = _cellAmount * (_cellSize * 0.5f); // Half grid width

    int v = 0, i = 0;
    for (int z = 0; z < vertexAmount; z++) {
        for (int x = 0; x < vertexAmount; x++) {
            vertices[v] = (Vector3){ x * _cellSize - gridExtent, 0.0f, z * _cellSize - gridExtent };
            normals[v] = (Vector3){ 0.0f, 1.0f, 0.0f };
            texcoords[v] = (Vector2){ (float)x / _cellAmount, (float)z / _cellAmount };

            if (x < _cellAmount && z < _cellAmount) {
                int topLeft = v;
                int topRight = v + 1;
                int bottomLeft = v + vertexAmount;
                int bottomRight = v + vertexAmount + 1;

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

    _mesh = { 0 };
    _mesh.vertexCount = vertexCount;
    _mesh.triangleCount = indexCount / 3;

    _mesh.vertices = (float *)vertices;
    _mesh.normals = (float *)normals;
    _mesh.texcoords = (float *)texcoords;
    _mesh.indices = indices;

    UploadMesh(&_mesh, true);
    _model = LoadModelFromMesh(_mesh);
}

void MapGrid::draw()
{
    drawMesh();
    drawWireframe();
}

std::optional<Vector3D> MapGrid::getCellFromRay(const Ray &ray) const
{
    if (ray.direction.y == 0.0f)
        return std::nullopt;

    float t = -ray.position.y / ray.direction.y;
    if (t < 0.0f)
        return std::nullopt;

    Vector3 hit = Vector3Add(ray.position, Vector3Scale(ray.direction, t));

    float halfExtent = (_cellAmount * _cellSize) / 2.0f;

    if (hit.x < -halfExtent || hit.x >= halfExtent || hit.z < -halfExtent || hit.z >= halfExtent)
        return std::nullopt;

    int gridX = static_cast<int>(floor((hit.x + halfExtent) / _cellSize));
    int gridZ = static_cast<int>(floor((hit.z + halfExtent) / _cellSize));

    float worldX = gridX * _cellSize - halfExtent + _cellSize / 2.0f;
    float worldZ = gridZ * _cellSize - halfExtent + _cellSize / 2.0f;

    return std::optional<Vector3D>(Vector3D(worldX, 0.5f, worldZ));
}

void MapGrid::drawMesh()
{
    DrawModel(_model, (Vector3){ 0, 0, 0 }, _scale, _color);
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
    rlColor4ub(_backgroundColor.r, _backgroundColor.g, _backgroundColor.b, _backgroundColor.a);

    float gridExtent = _cellAmount * (_cellSize * 0.5f); // Center the grid

    for (int i = 0; i < _cellAmount + 1; i++) {
        float offset = i * _cellSize - gridExtent;
        drawLines(gridExtent, 0.01f, offset);
    }

    rlEnd();
    rlPopMatrix();
}
