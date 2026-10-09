#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

const float PI = 3.14159265359f;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
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

        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        
        glBindVertexArray(0);
    }
};

class PrimitiveFactory {
public:
    static Mesh CreateCube()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        float vertices_data[] = {
            // positions          // normals
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f
        };

        for (int i = 0; i < 24; i++) {
            Vertex v;
            v.Position = glm::vec3(vertices_data[i * 6], vertices_data[i * 6 + 1], vertices_data[i * 6 + 2]);
            v.Normal = glm::vec3(vertices_data[i * 6 + 3], vertices_data[i * 6 + 4], vertices_data[i * 6 + 5]);
            vertices.push_back(v);
        }

        unsigned int indices_data[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9, 10, 10, 11, 8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };

        for (int i = 0; i < 36; i++) {
            indices.push_back(indices_data[i]);
        }

        return Mesh(vertices, indices);
    }

    static Mesh CreatePlane()
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        Vertex v;
        v.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        
        v.Position = glm::vec3(-0.5f, 0.0f, -0.5f); vertices.push_back(v);
        v.Position = glm::vec3( 0.5f, 0.0f, -0.5f); vertices.push_back(v);
        v.Position = glm::vec3( 0.5f, 0.0f,  0.5f); vertices.push_back(v);
        v.Position = glm::vec3(-0.5f, 0.0f,  0.5f); vertices.push_back(v);
        
        indices = { 0, 2, 1, 0, 3, 2 }; // CCW
        
        return Mesh(vertices, indices);
    }

    static Mesh CreateSphere(int sectorCount = 36, int stackCount = 18)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        float x, y, z, xy;
        float nx, ny, nz, lengthInv = 1.0f;
        float radius = 0.5f;

        float sectorStep = 2 * PI / sectorCount;
        float stackStep = PI / stackCount;
        float sectorAngle, stackAngle;

        for(int i = 0; i <= stackCount; ++i)
        {
            stackAngle = PI / 2 - i * stackStep;
            xy = radius * cosf(stackAngle);
            z = radius * sinf(stackAngle);

            for(int j = 0; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;

                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);

                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;

                Vertex v;
                v.Position = glm::vec3(x, z, y); 
                v.Normal = glm::vec3(nx, nz, ny); 
                vertices.push_back(v);
            }
        }

        int k1, k2;
        for(int i = 0; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);
            k2 = k1 + sectorCount + 1;

            for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                if(i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                if(i != (stackCount - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }

        return Mesh(vertices, indices);
    }

    static Mesh CreateCylinder(int sectorCount = 36)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        
        float radius = 0.5f;
        float height = 1.0f;
        float halfHeight = height / 2.0f;

        // Top and bottom center vertices
        Vertex topCenter, bottomCenter;
        topCenter.Position = glm::vec3(0.0f, halfHeight, 0.0f);
        topCenter.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        
        bottomCenter.Position = glm::vec3(0.0f, -halfHeight, 0.0f);
        bottomCenter.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
        
        int topCenterIdx = 0;
        vertices.push_back(topCenter);
        
        int bottomCenterIdx = 1;
        vertices.push_back(bottomCenter);

        int sideStartIdx = 2;

        float sectorStep = 2 * PI / sectorCount;
        for (int i = 0; i <= sectorCount; ++i) {
            float sectorAngle = i * sectorStep;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);

            Vertex topV, bottomV;
            
            // Side vertices (with normals facing outwards)
            glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
            
            topV.Position = glm::vec3(x, halfHeight, z);
            topV.Normal = normal;
            
            bottomV.Position = glm::vec3(x, -halfHeight, z);
            bottomV.Normal = normal;
            
            vertices.push_back(topV);
            vertices.push_back(bottomV);
            
            // Top and bottom cap vertices (normals facing up/down)
            Vertex topCapV, bottomCapV;
            topCapV.Position = glm::vec3(x, halfHeight, z);
            topCapV.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            
            bottomCapV.Position = glm::vec3(x, -halfHeight, z);
            bottomCapV.Normal = glm::vec3(0.0f, -1.0f, 0.0f);
            
            vertices.push_back(topCapV);
            vertices.push_back(bottomCapV);
        }

        // Indices
        for (int i = 0; i < sectorCount; ++i) {
            int baseIdx = sideStartIdx + i * 4; // 4 vertices per sector step
            
            // Side
            indices.push_back(baseIdx); // top
            indices.push_back(baseIdx + 1); // bottom
            indices.push_back(baseIdx + 4); // next top
            
            indices.push_back(baseIdx + 4);
            indices.push_back(baseIdx + 1);
            indices.push_back(baseIdx + 5); // next bottom
            
            // Top Cap
            indices.push_back(topCenterIdx);
            indices.push_back(baseIdx + 6); // next top cap
            indices.push_back(baseIdx + 2); // top cap
            
            // Bottom Cap
            indices.push_back(bottomCenterIdx);
            indices.push_back(baseIdx + 3); // bottom cap
            indices.push_back(baseIdx + 7); // next bottom cap
        }

        return Mesh(vertices, indices);
    }
};

#endif
