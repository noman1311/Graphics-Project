#ifndef SCENE_H
#define SCENE_H

#include "Shader.h"
#include "Primitives.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Scene {
public:
    Mesh cube;
    Mesh cylinder;
    Mesh sphere;
    Mesh plane;

    Scene();
    void Draw(Shader& phongShader, Shader& lightShader, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& lightPos);

private:
    void drawMesh(Mesh& mesh, Shader& shader, glm::vec3 position, glm::vec3 scale, glm::vec3 color, glm::vec3 rotation = glm::vec3(0.0f));
    void DrawEnvironment(Shader& shader);
    void DrawFurniture(Shader& shader);
    void DrawGameSetups(Shader& shader);
};

#endif
