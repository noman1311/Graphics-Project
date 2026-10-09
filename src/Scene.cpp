#include "Scene.h"

Scene::Scene() 
    : cube(PrimitiveFactory::CreateCube()), 
      cylinder(PrimitiveFactory::CreateCylinder()), 
      sphere(PrimitiveFactory::CreateSphere()), 
      plane(PrimitiveFactory::CreatePlane()) 
{
}

void Scene::drawMesh(Mesh& mesh, Shader& shader, glm::vec3 position, glm::vec3 scale, glm::vec3 color, glm::vec3 rotation)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    if(rotation.x != 0.0f) model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    if(rotation.y != 0.0f) model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    if(rotation.z != 0.0f) model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    
    shader.setMat4("model", model);
    shader.setVec3("objectColor", color);
    
    mesh.Draw();
}

void Scene::DrawEnvironment(Shader& shader)
{
    // Floor
    drawMesh(cube, shader, glm::vec3(0.0f, -0.05f, 0.0f), glm::vec3(30.0f, 0.1f, 30.0f), glm::vec3(0.6f, 0.4f, 0.2f));
    // Ceiling
    drawMesh(cube, shader, glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(30.0f, 0.1f, 30.0f), glm::vec3(0.9f, 0.9f, 0.9f));
    // Walls
    drawMesh(cube, shader, glm::vec3(-15.0f, 7.5f, 0.0f), glm::vec3(0.1f, 15.0f, 30.0f), glm::vec3(0.8f, 0.8f, 0.8f)); // Left
    drawMesh(cube, shader, glm::vec3(15.0f, 7.5f, 0.0f), glm::vec3(0.1f, 15.0f, 30.0f), glm::vec3(0.8f, 0.8f, 0.8f)); // Right
    drawMesh(cube, shader, glm::vec3(0.0f, 7.5f, -15.0f), glm::vec3(30.0f, 15.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f)); // Back
    drawMesh(cube, shader, glm::vec3(0.0f, 7.5f, 15.0f), glm::vec3(30.0f, 15.0f, 0.1f), glm::vec3(0.7f, 0.7f, 0.7f)); // Front
}

void Scene::DrawFurniture(Shader& shader)
{
    // Table
    drawMesh(cube, shader, glm::vec3(8.0f, 2.0f, -5.0f), glm::vec3(4.0f, 0.2f, 3.0f), glm::vec3(0.5f, 0.3f, 0.1f));
    drawMesh(cube, shader, glm::vec3(9.8f, 1.0f, -3.7f), glm::vec3(0.2f, 2.0f, 0.2f), glm::vec3(0.3f, 0.2f, 0.1f));
    drawMesh(cube, shader, glm::vec3(9.8f, 1.0f, -6.3f), glm::vec3(0.2f, 2.0f, 0.2f), glm::vec3(0.3f, 0.2f, 0.1f));
    drawMesh(cube, shader, glm::vec3(6.2f, 1.0f, -3.7f), glm::vec3(0.2f, 2.0f, 0.2f), glm::vec3(0.3f, 0.2f, 0.1f));
    drawMesh(cube, shader, glm::vec3(6.2f, 1.0f, -6.3f), glm::vec3(0.2f, 2.0f, 0.2f), glm::vec3(0.3f, 0.2f, 0.1f));

    // Chair 1
    drawMesh(cube, shader, glm::vec3(8.0f, 1.2f, -2.5f), glm::vec3(1.2f, 0.2f, 1.2f), glm::vec3(0.4f, 0.2f, 0.1f)); // Seat
    drawMesh(cube, shader, glm::vec3(8.0f, 2.7f, -1.95f), glm::vec3(1.2f, 0.2f, 0.15f), glm::vec3(0.4f, 0.2f, 0.1f)); // Top rail
    drawMesh(cube, shader, glm::vec3(8.525f, 2.05f, -1.95f), glm::vec3(0.15f, 1.5f, 0.15f), glm::vec3(0.4f, 0.2f, 0.1f)); // Left stile
    drawMesh(cube, shader, glm::vec3(7.475f, 2.05f, -1.95f), glm::vec3(0.15f, 1.5f, 0.15f), glm::vec3(0.4f, 0.2f, 0.1f)); // Right stile
    drawMesh(cube, shader, glm::vec3(8.25f, 2.05f, -1.95f), glm::vec3(0.1f, 1.5f, 0.1f), glm::vec3(0.4f, 0.2f, 0.1f)); // Slat
    drawMesh(cube, shader, glm::vec3(8.0f, 2.05f, -1.95f), glm::vec3(0.1f, 1.5f, 0.1f), glm::vec3(0.4f, 0.2f, 0.1f)); // Slat
    drawMesh(cube, shader, glm::vec3(7.75f, 2.05f, -1.95f), glm::vec3(0.1f, 1.5f, 0.1f), glm::vec3(0.4f, 0.2f, 0.1f)); // Slat
    drawMesh(cube, shader, glm::vec3(8.525f, 0.6f, -1.975f), glm::vec3(0.15f, 1.2f, 0.15f), glm::vec3(0.3f, 0.15f, 0.1f)); // Legs
    drawMesh(cube, shader, glm::vec3(7.475f, 0.6f, -1.975f), glm::vec3(0.15f, 1.2f, 0.15f), glm::vec3(0.3f, 0.15f, 0.1f));
    drawMesh(cube, shader, glm::vec3(8.525f, 0.6f, -3.025f), glm::vec3(0.15f, 1.2f, 0.15f), glm::vec3(0.3f, 0.15f, 0.1f));
    drawMesh(cube, shader, glm::vec3(7.475f, 0.6f, -3.025f), glm::vec3(0.15f, 1.2f, 0.15f), glm::vec3(0.3f, 0.15f, 0.1f));

    // Chair 2
    drawMesh(cube, shader, glm::vec3(8.0f, 1.2f, -7.5f), glm::vec3(1.2f, 0.2f, 1.2f), glm::vec3(0.4f, 0.2f, 0.1f)); // Seat
    drawMesh(cube, shader, glm::vec3(8.0f, 2.7f, -8.05f), glm::vec3(1.2f, 0.2f, 0.15f), glm::vec3(0.4f, 0.2f, 0.1f)); // Top rail
    drawMesh(cube, shader, glm::vec3(8.525f, 2.05f, -8.05f), glm::vec3(0.15f, 1.5f, 0.15f), glm::vec3(0.4f, 0.2f, 0.1f)); // Left stile
    drawMesh(cube, shader, glm::vec3(7.475f, 2.05f, -8.05f), glm::vec3(0.15f, 1.5f, 0.15f), glm::vec3(0.4f, 0.2f, 0.1f)); // Right stile
    drawMesh(cube, shader, glm::vec3(8.25f, 2.05f, -8.05f), glm::vec3(0.1f, 1.5f, 0.1f), glm::vec3(0.4f, 0.2f, 0.1f)); // Slat
    drawMesh(cube, shader, glm::vec3(8.0f, 2.05f, -8.05f), glm::vec3(0.1f, 1.5f, 0.1f), glm::vec3(0.4f, 0.2f, 0.1f)); // Slat
    drawMesh(cube, shader, glm::vec3(7.75f, 2.05f, -8.05f), glm::vec3(0.1f, 1.5f, 0.1f), glm::vec3(0.4f, 0.2f, 0.1f)); // Slat
    drawMesh(cube, shader, glm::vec3(8.525f, 0.6f, -6.975f), glm::vec3(0.15f, 1.2f, 0.15f), glm::vec3(0.3f, 0.15f, 0.1f)); // Legs
    drawMesh(cube, shader, glm::vec3(7.475f, 0.6f, -6.975f), glm::vec3(0.15f, 1.2f, 0.15f), glm::vec3(0.3f, 0.15f, 0.1f));
    drawMesh(cube, shader, glm::vec3(8.525f, 0.6f, -8.025f), glm::vec3(0.15f, 1.2f, 0.15f), glm::vec3(0.3f, 0.15f, 0.1f));
    drawMesh(cube, shader, glm::vec3(7.475f, 0.6f, -8.025f), glm::vec3(0.15f, 1.2f, 0.15f), glm::vec3(0.3f, 0.15f, 0.1f));

    // Almirahs
    // Almirah 1 Body
    drawMesh(cube, shader, glm::vec3(-10.0f, 4.0f, -13.0f), glm::vec3(4.0f, 8.0f, 2.0f), glm::vec3(0.5f, 0.3f, 0.2f));
    // Almirah 1 Doors
    drawMesh(cube, shader, glm::vec3(-10.98f, 4.0f, -11.95f), glm::vec3(1.95f, 7.8f, 0.1f), glm::vec3(0.6f, 0.4f, 0.3f));
    drawMesh(cube, shader, glm::vec3(-9.02f, 4.0f, -11.95f), glm::vec3(1.95f, 7.8f, 0.1f), glm::vec3(0.6f, 0.4f, 0.3f));
    // Almirah 1 Handles
    drawMesh(cube, shader, glm::vec3(-10.2f, 4.0f, -11.85f), glm::vec3(0.1f, 0.6f, 0.1f), glm::vec3(0.8f, 0.8f, 0.8f));
    drawMesh(cube, shader, glm::vec3(-9.8f, 4.0f, -11.85f), glm::vec3(0.1f, 0.6f, 0.1f), glm::vec3(0.8f, 0.8f, 0.8f));

    // Almirah 2 Body
    drawMesh(cube, shader, glm::vec3(10.0f, 4.0f, -13.0f), glm::vec3(4.0f, 8.0f, 2.0f), glm::vec3(0.5f, 0.3f, 0.2f));
    // Almirah 2 Doors
    drawMesh(cube, shader, glm::vec3(9.02f, 4.0f, -11.95f), glm::vec3(1.95f, 7.8f, 0.1f), glm::vec3(0.6f, 0.4f, 0.3f));
    drawMesh(cube, shader, glm::vec3(10.98f, 4.0f, -11.95f), glm::vec3(1.95f, 7.8f, 0.1f), glm::vec3(0.6f, 0.4f, 0.3f));
    // Almirah 2 Handles
    drawMesh(cube, shader, glm::vec3(9.8f, 4.0f, -11.85f), glm::vec3(0.1f, 0.6f, 0.1f), glm::vec3(0.8f, 0.8f, 0.8f));
    drawMesh(cube, shader, glm::vec3(10.2f, 4.0f, -11.85f), glm::vec3(0.1f, 0.6f, 0.1f), glm::vec3(0.8f, 0.8f, 0.8f));
}

void Scene::DrawGameSetups(Shader& shader)
{
    // Carrom Board 1
    drawMesh(cube, shader, glm::vec3(-8.0f, 3.5f, 5.0f), glm::vec3(3.0f, 0.1f, 3.0f), glm::vec3(0.9f, 0.8f, 0.6f)); // Base
    drawMesh(cube, shader, glm::vec3(-9.4f, 3.65f, 5.0f), glm::vec3(0.2f, 0.2f, 3.0f), glm::vec3(0.3f, 0.1f, 0.05f)); // Left Border
    drawMesh(cube, shader, glm::vec3(-6.6f, 3.65f, 5.0f), glm::vec3(0.2f, 0.2f, 3.0f), glm::vec3(0.3f, 0.1f, 0.05f)); // Right Border
    drawMesh(cube, shader, glm::vec3(-8.0f, 3.65f, 3.6f), glm::vec3(2.6f, 0.2f, 0.2f), glm::vec3(0.3f, 0.1f, 0.05f)); // Top Border
    drawMesh(cube, shader, glm::vec3(-8.0f, 3.65f, 6.4f), glm::vec3(2.6f, 0.2f, 0.2f), glm::vec3(0.3f, 0.1f, 0.05f)); // Bottom Border
    drawMesh(cylinder, shader, glm::vec3(-9.2f, 3.56f, 3.8f), glm::vec3(0.3f, 0.1f, 0.3f), glm::vec3(0.1f, 0.1f, 0.1f)); // Pockets
    drawMesh(cylinder, shader, glm::vec3(-9.2f, 3.56f, 6.2f), glm::vec3(0.3f, 0.1f, 0.3f), glm::vec3(0.1f, 0.1f, 0.1f));
    drawMesh(cylinder, shader, glm::vec3(-6.8f, 3.56f, 3.8f), glm::vec3(0.3f, 0.1f, 0.3f), glm::vec3(0.1f, 0.1f, 0.1f));
    drawMesh(cylinder, shader, glm::vec3(-6.8f, 3.56f, 6.2f), glm::vec3(0.3f, 0.1f, 0.3f), glm::vec3(0.1f, 0.1f, 0.1f));
    // Carrom Board 1 Legs
    drawMesh(cube, shader, glm::vec3(-9.4f, 1.75f, 3.6f), glm::vec3(0.15f, 3.5f, 0.15f), glm::vec3(0.2f, 0.1f, 0.05f));
    drawMesh(cube, shader, glm::vec3(-9.4f, 1.75f, 6.4f), glm::vec3(0.15f, 3.5f, 0.15f), glm::vec3(0.2f, 0.1f, 0.05f));
    drawMesh(cube, shader, glm::vec3(-6.6f, 1.75f, 3.6f), glm::vec3(0.15f, 3.5f, 0.15f), glm::vec3(0.2f, 0.1f, 0.05f));
    drawMesh(cube, shader, glm::vec3(-6.6f, 1.75f, 6.4f), glm::vec3(0.15f, 3.5f, 0.15f), glm::vec3(0.2f, 0.1f, 0.05f));
    // Carrom Board 1 Coins
    drawMesh(cylinder, shader, glm::vec3(-8.0f, 3.575f, 5.0f), glm::vec3(0.15f, 0.05f, 0.15f), glm::vec3(0.9f, 0.9f, 0.9f)); // White coin
    drawMesh(cylinder, shader, glm::vec3(-7.8f, 3.575f, 4.8f), glm::vec3(0.15f, 0.05f, 0.15f), glm::vec3(0.1f, 0.1f, 0.1f)); // Black coin
    drawMesh(cylinder, shader, glm::vec3(-8.5f, 3.58f, 5.8f), glm::vec3(0.2f, 0.06f, 0.2f), glm::vec3(0.8f, 0.1f, 0.1f)); // Red striker

    // Carrom Board 2
    drawMesh(cube, shader, glm::vec3(8.0f, 3.5f, 5.0f), glm::vec3(3.0f, 0.1f, 3.0f), glm::vec3(0.9f, 0.8f, 0.6f)); // Base
    drawMesh(cube, shader, glm::vec3(6.6f, 3.65f, 5.0f), glm::vec3(0.2f, 0.2f, 3.0f), glm::vec3(0.3f, 0.1f, 0.05f)); // Left Border
    drawMesh(cube, shader, glm::vec3(9.4f, 3.65f, 5.0f), glm::vec3(0.2f, 0.2f, 3.0f), glm::vec3(0.3f, 0.1f, 0.05f)); // Right Border
    drawMesh(cube, shader, glm::vec3(8.0f, 3.65f, 3.6f), glm::vec3(2.6f, 0.2f, 0.2f), glm::vec3(0.3f, 0.1f, 0.05f)); // Top Border
    drawMesh(cube, shader, glm::vec3(8.0f, 3.65f, 6.4f), glm::vec3(2.6f, 0.2f, 0.2f), glm::vec3(0.3f, 0.1f, 0.05f)); // Bottom Border
    drawMesh(cylinder, shader, glm::vec3(6.8f, 3.56f, 3.8f), glm::vec3(0.3f, 0.1f, 0.3f), glm::vec3(0.1f, 0.1f, 0.1f)); // Pockets
    drawMesh(cylinder, shader, glm::vec3(6.8f, 3.56f, 6.2f), glm::vec3(0.3f, 0.1f, 0.3f), glm::vec3(0.1f, 0.1f, 0.1f));
    drawMesh(cylinder, shader, glm::vec3(9.2f, 3.56f, 3.8f), glm::vec3(0.3f, 0.1f, 0.3f), glm::vec3(0.1f, 0.1f, 0.1f));
    drawMesh(cylinder, shader, glm::vec3(9.2f, 3.56f, 6.2f), glm::vec3(0.3f, 0.1f, 0.3f), glm::vec3(0.1f, 0.1f, 0.1f));
    // Carrom Board 2 Legs
    drawMesh(cube, shader, glm::vec3(6.6f, 1.75f, 3.6f), glm::vec3(0.15f, 3.5f, 0.15f), glm::vec3(0.2f, 0.1f, 0.05f));
    drawMesh(cube, shader, glm::vec3(6.6f, 1.75f, 6.4f), glm::vec3(0.15f, 3.5f, 0.15f), glm::vec3(0.2f, 0.1f, 0.05f));
    drawMesh(cube, shader, glm::vec3(9.4f, 1.75f, 3.6f), glm::vec3(0.15f, 3.5f, 0.15f), glm::vec3(0.2f, 0.1f, 0.05f));
    drawMesh(cube, shader, glm::vec3(9.4f, 1.75f, 6.4f), glm::vec3(0.15f, 3.5f, 0.15f), glm::vec3(0.2f, 0.1f, 0.05f));
    // Carrom Board 2 Coins
    drawMesh(cylinder, shader, glm::vec3(8.0f, 3.575f, 5.0f), glm::vec3(0.15f, 0.05f, 0.15f), glm::vec3(0.9f, 0.9f, 0.9f)); // White coin
    drawMesh(cylinder, shader, glm::vec3(8.2f, 3.575f, 4.8f), glm::vec3(0.15f, 0.05f, 0.15f), glm::vec3(0.1f, 0.1f, 0.1f)); // Black coin
    drawMesh(cylinder, shader, glm::vec3(7.5f, 3.58f, 5.8f), glm::vec3(0.2f, 0.06f, 0.2f), glm::vec3(0.8f, 0.1f, 0.1f)); // Red striker

    // Table Tennis Board
    drawMesh(cube, shader, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(8.0f, 0.2f, 5.0f), glm::vec3(0.0f, 0.4f, 0.8f));
    drawMesh(cube, shader, glm::vec3(0.0f, 3.35f, 0.0f), glm::vec3(0.1f, 0.5f, 5.0f), glm::vec3(0.9f, 0.9f, 0.9f));
    drawMesh(cube, shader, glm::vec3(3.8f, 1.5f, 2.3f), glm::vec3(0.2f, 3.0f, 0.2f), glm::vec3(0.2f, 0.2f, 0.2f));
    drawMesh(cube, shader, glm::vec3(-3.8f, 1.5f, 2.3f), glm::vec3(0.2f, 3.0f, 0.2f), glm::vec3(0.2f, 0.2f, 0.2f));
    drawMesh(cube, shader, glm::vec3(3.8f, 1.5f, -2.3f), glm::vec3(0.2f, 3.0f, 0.2f), glm::vec3(0.2f, 0.2f, 0.2f));
    drawMesh(cube, shader, glm::vec3(-3.8f, 1.5f, -2.3f), glm::vec3(0.2f, 3.0f, 0.2f), glm::vec3(0.2f, 0.2f, 0.2f));

    // TT Paddles and Ball
    drawMesh(cylinder, shader, glm::vec3(2.0f, 3.5f, 0.0f), glm::vec3(0.4f, 0.05f, 0.4f), glm::vec3(0.8f, 0.1f, 0.1f), glm::vec3(90.0f, 0.0f, 0.0f)); // Red Blade
    drawMesh(cylinder, shader, glm::vec3(2.0f, 3.1f, 0.0f), glm::vec3(0.08f, 0.4f, 0.08f), glm::vec3(0.6f, 0.4f, 0.2f)); // Handle
    
    drawMesh(cylinder, shader, glm::vec3(-2.0f, 3.5f, 0.0f), glm::vec3(0.4f, 0.05f, 0.4f), glm::vec3(0.1f, 0.1f, 0.8f), glm::vec3(90.0f, 0.0f, 0.0f)); // Blue Blade
    drawMesh(cylinder, shader, glm::vec3(-2.0f, 3.1f, 0.0f), glm::vec3(0.08f, 0.4f, 0.08f), glm::vec3(0.6f, 0.4f, 0.2f)); // Handle

    drawMesh(sphere, shader, glm::vec3(0.5f, 3.15f, 0.5f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.0f, 0.6f, 0.0f));
}

void Scene::Draw(Shader& phongShader, Shader& lightShader, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& lightPos)
{
    phongShader.use();
    phongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    phongShader.setVec3("lightPos", lightPos);
    
    phongShader.setMat4("projection", projection);
    phongShader.setMat4("view", view);

    DrawEnvironment(phongShader);
    DrawFurniture(phongShader);
    DrawGameSetups(phongShader);

    // Light source
    lightShader.use();
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);
    drawMesh(sphere, lightShader, lightPos, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
}
