#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "Primitives.h"
#include "Material.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Scene {
public:
    // Primitives (shared across objects)
    Mesh cube;
    Mesh cylinder;
    Mesh sphere;
    Mesh plane;

    // Procedural textures
    Texture texWoodPlanks;
    Texture texDarkWood;
    Texture texLightWood;
    Texture texPlaster;
    Texture texCeilingMatte;
    Texture texTableTennis;
    Texture texCarromBoard;
    Texture texSky;
    Texture texGrass;
    Texture texFabric;
    Texture texGlass;

    // Single shared point-light world position (center of the bulb's luminous body).
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    float lightConstant, lightLinear, lightQuadratic;

    Scene();
    void Draw(Shader& phongShader, Shader& lightShader,
              const glm::mat4& projection, const glm::mat4& view,
              const glm::vec3& cameraPos, bool miniGameActive = false,
              int activeCarromBoard = 0);

private:
    // Set material uniforms on a Phong shader.
    void setMaterial(Shader& shader, const Material& m) const;

    // Draw a mesh with a material at the given transform.
    void drawMesh(Shader& shader, Mesh& mesh,
                  glm::vec3 position, glm::vec3 scale,
                  const Material& material,
                  glm::vec3 rotation = glm::vec3(0.0f));

    // Subsections of the scene
    void DrawEnvironment(Shader& shader);
    void DrawDoor(Shader& shader);
    void DrawCeilingLight(Shader& phongShader, Shader& lightShader);
    void DrawFurniture(Shader& shader);
    void DrawGameSetups(Shader& shader, bool miniGameActive = false,
                        int activeCarromBoard = 0);
    void DrawWindow(Shader& shader);
    void DrawExterior(Shader& shader);
};

#endif
