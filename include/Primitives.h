#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

const float PI = 3.14159265359f;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    {
        this->vertices = vertices;
        this->indices = indices;
        setupMesh();
    }

    void Draw()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        // TexCoords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
};

class PrimitiveFactory {
public:
    // -------- CUBE --------
    static Mesh CreateCube()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Each face: position, normal, uv (CCW from outside)
        // Back (-Z)
        vertices.push_back({glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3( 0, 0,-1), glm::vec2(0,0)});
        vertices.push_back({glm::vec3( 0.5f,-0.5f,-0.5f), glm::vec3( 0, 0,-1), glm::vec2(1,0)});
        vertices.push_back({glm::vec3( 0.5f, 0.5f,-0.5f), glm::vec3( 0, 0,-1), glm::vec2(1,1)});
        vertices.push_back({glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec3( 0, 0,-1), glm::vec2(0,1)});
        // Front (+Z)
        vertices.push_back({glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec3( 0, 0, 1), glm::vec2(0,0)});
        vertices.push_back({glm::vec3( 0.5f,-0.5f, 0.5f), glm::vec3( 0, 0, 1), glm::vec2(1,0)});
        vertices.push_back({glm::vec3( 0.5f, 0.5f, 0.5f), glm::vec3( 0, 0, 1), glm::vec2(1,1)});
        vertices.push_back({glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3( 0, 0, 1), glm::vec2(0,1)});
        // Left (-X)
        vertices.push_back({glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1,0)});
        vertices.push_back({glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec3(-1, 0, 0), glm::vec2(0,0)});
        vertices.push_back({glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(-1, 0, 0), glm::vec2(0,1)});
        vertices.push_back({glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1,1)});
        // Right (+X)
        vertices.push_back({glm::vec3( 0.5f, 0.5f, 0.5f), glm::vec3( 1, 0, 0), glm::vec2(0,0)});
        vertices.push_back({glm::vec3( 0.5f, 0.5f,-0.5f), glm::vec3( 1, 0, 0), glm::vec2(1,0)});
        vertices.push_back({glm::vec3( 0.5f,-0.5f,-0.5f), glm::vec3( 1, 0, 0), glm::vec2(1,1)});
        vertices.push_back({glm::vec3( 0.5f,-0.5f, 0.5f), glm::vec3( 1, 0, 0), glm::vec2(0,1)});
        // Bottom (-Y)
        vertices.push_back({glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3( 0,-1, 0), glm::vec2(0,1)});
        vertices.push_back({glm::vec3( 0.5f,-0.5f,-0.5f), glm::vec3( 0,-1, 0), glm::vec2(1,1)});
        vertices.push_back({glm::vec3( 0.5f,-0.5f, 0.5f), glm::vec3( 0,-1, 0), glm::vec2(1,0)});
        vertices.push_back({glm::vec3(-0.5f,-0.5f, 0.5f), glm::vec3( 0,-1, 0), glm::vec2(0,0)});
        // Top (+Y)
        vertices.push_back({glm::vec3(-0.5f, 0.5f,-0.5f), glm::vec3( 0, 1, 0), glm::vec2(0,0)});
        vertices.push_back({glm::vec3( 0.5f, 0.5f,-0.5f), glm::vec3( 0, 1, 0), glm::vec2(1,0)});
        vertices.push_back({glm::vec3( 0.5f, 0.5f, 0.5f), glm::vec3( 0, 1, 0), glm::vec2(1,1)});
        vertices.push_back({glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3( 0, 1, 0), glm::vec2(0,1)});

        unsigned int idx[] = {
            0, 1, 2,  2, 3, 0,
            4, 5, 6,  6, 7, 4,
            8, 9,10, 10,11, 8,
           12,13,14, 14,15,12,
           16,17,18, 18,19,16,
           20,21,22, 22,23,20
        };
        indices.assign(idx, idx + 36);
        return Mesh(vertices, indices);
    }

    // -------- PLANE (single quad) --------
    static Mesh CreatePlane()
    {
        std::vector<Vertex> v(4);
        v[0] = {glm::vec3(-0.5f, 0.0f,-0.5f), glm::vec3(0,1,0), glm::vec2(0,0)};
        v[1] = {glm::vec3( 0.5f, 0.0f,-0.5f), glm::vec3(0,1,0), glm::vec2(1,0)};
        v[2] = {glm::vec3( 0.5f, 0.0f, 0.5f), glm::vec3(0,1,0), glm::vec2(1,1)};
        v[3] = {glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec3(0,1,0), glm::vec2(0,1)};
        std::vector<unsigned int> i = {0, 2, 1, 0, 3, 2};
        return Mesh(v, i);
    }

    // -------- SPHERE --------
    static Mesh CreateSphere(int sectorCount = 36, int stackCount = 24)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        float x, y, z, xy;
        float nx, ny, nz, lengthInv = 1.0f;
        float radius = 0.5f;

        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;

        for (int i = 0; i <= stackCount; ++i)
        {
            float stackAngle = PI / 2 - i * stackStep;
            xy = radius * cosf(stackAngle);
            z = radius * sinf(stackAngle);

            for (int j = 0; j <= sectorCount; ++j)
            {
                float sectorAngle = j * sectorStep;
                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);

                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;

                Vertex vtx;
                // Swap to align with cube Y-up
                vtx.Position = glm::vec3(x, z, y);
                vtx.Normal = glm::vec3(nx, nz, ny);
                vtx.TexCoords = glm::vec2((float)j / sectorCount, (float)i / stackCount);
                vertices.push_back(vtx);
            }
        }

        for (int i = 0; i < stackCount; ++i)
        {
            int k1 = i * (sectorCount + 1);
            int k2 = k1 + sectorCount + 1;
            for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                if (i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                if (i != (stackCount - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
        return Mesh(vertices, indices);
    }

    // -------- CYLINDER --------
    // Cylinder with side + top + bottom caps, UVs wrapping around.
    // Default is unit cylinder centered at origin, height 1, radius 0.5, axis = Y.
    static Mesh CreateCylinder(int sectorCount = 36)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        float radius = 0.5f;
        float height = 1.0f;
        float halfHeight = height / 2.0f;

        // Center vertices for caps
        vertices.push_back({glm::vec3(0, halfHeight, 0), glm::vec3(0, 1, 0), glm::vec2(0.5f, 0.5f)});
        vertices.push_back({glm::vec3(0,-halfHeight, 0), glm::vec3(0,-1, 0), glm::vec2(0.5f, 0.5f)});
        int topCenterIdx = 0;
        int botCenterIdx = 1;

        int sideStart = 2;
        float sectorStep = 2 * PI / sectorCount;
        for (int i = 0; i <= sectorCount; ++i)
        {
            float ang = i * sectorStep;
            float x = radius * cosf(ang);
            float z = radius * sinf(ang);
            glm::vec3 n = glm::normalize(glm::vec3(x, 0, z));
            float u = (float)i / sectorCount;

            // side top
            vertices.push_back({glm::vec3(x, halfHeight, z), n, glm::vec2(u, 1.0f)});
            // side bottom
            vertices.push_back({glm::vec3(x,-halfHeight, z), n, glm::vec2(u, 0.0f)});
            // top cap (normal +Y)
            vertices.push_back({glm::vec3(x, halfHeight, z), glm::vec3(0,1,0), glm::vec2(0.5f + 0.5f*x/radius, 0.5f + 0.5f*z/radius)});
            // bottom cap (normal -Y)
            vertices.push_back({glm::vec3(x,-halfHeight, z), glm::vec3(0,-1,0), glm::vec2(0.5f + 0.5f*x/radius, 0.5f - 0.5f*z/radius)});
        }

        for (int i = 0; i < sectorCount; ++i)
        {
            int b = sideStart + i * 4;
            // side (CCW outside)
            indices.push_back(b);
            indices.push_back(b + 1);
            indices.push_back(b + 4);
            indices.push_back(b + 4);
            indices.push_back(b + 1);
            indices.push_back(b + 5);
            // top cap
            indices.push_back(topCenterIdx);
            indices.push_back(b + 6);
            indices.push_back(b + 2);
            // bottom cap
            indices.push_back(botCenterIdx);
            indices.push_back(b + 3);
            indices.push_back(b + 7);
        }

        return Mesh(vertices, indices);
    }
};

#endif
