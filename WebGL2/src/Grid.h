#pragma once

#include <vector>
#include <cstdint>
#include "Vertex.h"

class Grid {
public:
    Grid(int size = 10, int divisions = 10);
    ~Grid();
    
    void createGPUBuffers();
    void deleteGPUBuffers();
    void draw() const;
    
private:
    void generateGrid();
    
    std::vector<LineVertex> m_vertices;
    int m_size;
    int m_divisions;
    
    // OpenGL handles
    uint32_t m_vao;
    uint32_t m_vbo;
    bool m_gpuBuffersCreated;
};
