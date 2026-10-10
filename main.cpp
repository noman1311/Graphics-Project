#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "TableTennisGame.h"
#include "CarromGame.h"
#include "ChessGame.h"
#include "HudRenderer.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

unsigned int scrWidth = 800;
unsigned int scrHeight = 600;

// Initial camera: standing in the room, eye level, looking forward
Camera camera(glm::vec3(0.0f, 5.0f, 13.0f), glm::vec3(0.0f, 4.0f, 0.0f));
glm::vec3 savedCamPos = camera.Position;
float savedYaw = camera.Yaw;
float savedPitch = camera.Pitch;

float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

TableTennisGame game;
CarromGame carromGame;
ChessGame chessGame;
HudRenderer hud;

// Debouncing flags for discrete actions
bool key1PressedLast = false;
bool keySpacePressedLast = false;
bool keyEscPressedLast = false;
bool keyEPressedLast = false;
bool keyRPressedLast = false;
bool keyTPressedLast = false;
bool rayTracingEnabled = false;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, "Indoor Games Room", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Immediately clear and swap buffer to dark background to avoid white screen flash during texture initialization
    glClearColor(0.05f, 0.05f, 0.07f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);

    Shader phongShader("src/shaders/phong.vs", "src/shaders/phong.fs");
    Shader lightShader("src/shaders/phong.vs", "src/shaders/light.fs");
    Shader raytraceShader("src/shaders/raytrace.vs", "src/shaders/raytrace.fs");

    // Full-screen quad for Real-time Optical Ray Tracing pass
    float quadVertices[] = {
        // Positions (NDC)
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };
    unsigned int quadVAO = 0, quadVBO = 0;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    Scene scene;
    hud.Init(scrWidth, scrHeight);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Prevent physics instability from large frame time spikes (e.g. window dragging)
        if (deltaTime > 0.05f) deltaTime = 0.05f;

        processInput(window);

        // Update games simulations
        if (game.GetState() != GameState::EXPLORATION)
        {
            game.Update(deltaTime);
        }
        else if (carromGame.IsActive())
        {
            carromGame.Update(deltaTime);
        }
        else if (chessGame.IsActive())
        {
            chessGame.Update(deltaTime);
        }

        glClearColor(0.05f, 0.05f, 0.07f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)scrWidth / (float)scrHeight, 0.1f, 200.0f);

        bool ttActive = (game.GetState() != GameState::EXPLORATION);
        bool carromActive = carromGame.IsActive();
        bool chessActive = chessGame.IsActive();

        glm::mat4 view;
        glm::vec3 camPos;

        if (ttActive)
        {
            // Fixed gameplay camera for Table Tennis
            view = game.GetGameplayViewMatrix();
            camPos = glm::vec3(6.8f, 5.2f, 0.0f);
        }
        else if (carromActive)
        {
            // Fixed top-angled gameplay camera for active Carrom board
            view = carromGame.GetGameplayViewMatrix();
            camPos = carromGame.GetGameplayCameraPos();

            // Continuous mouse aiming update while in carrom
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            carromGame.HandleMouseMove(mx, my, scrWidth, scrHeight, view, projection);
        }
        else if (chessActive)
        {
            // Angled perspective gameplay camera looking over White's side of board
            view = chessGame.GetGameplayViewMatrix();
            camPos = chessGame.GetGameplayCameraPos();

            // Continuous mouse hover update while in chess
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            chessGame.HandleMouseMove(mx, my, scrWidth, scrHeight, view, projection);
        }
        else
        {
            // Free exploration first-person camera
            view = camera.GetViewMatrix();
            camPos = camera.Position;
        }

        glm::mat4 invView = glm::inverse(view);
        glm::vec3 camRight = glm::normalize(glm::vec3(invView[0]));
        glm::vec3 camUp = glm::normalize(glm::vec3(invView[1]));
        glm::vec3 camFront = -glm::normalize(glm::vec3(invView[2]));

        if (rayTracingEnabled)
        {
            glDisable(GL_DEPTH_TEST);
            raytraceShader.use();
            raytraceShader.setVec3("camPos", camPos);
            raytraceShader.setVec3("camFront", camFront);
            raytraceShader.setVec3("camRight", camRight);
            raytraceShader.setVec3("camUp", camUp);
            raytraceShader.setFloat("fovDegrees", 45.0f);
            raytraceShader.setFloat("aspectRatio", (float)scrWidth / (float)scrHeight);
            raytraceShader.setVec3("lightPos", scene.lightPos);
            raytraceShader.setVec3("lightColor", scene.lightColor);
            raytraceShader.setFloat("lightConstant", scene.lightConstant);
            raytraceShader.setFloat("lightLinear", scene.lightLinear);
            raytraceShader.setFloat("lightQuadratic", scene.lightQuadratic);
            raytraceShader.setFloat("time", (float)glfwGetTime());

            // Dynamic mini-game transforms
            raytraceShader.setInt("ttActive", ttActive ? 1 : 0);
            raytraceShader.setVec3("ttBallPos", game.GetBallPos());
            raytraceShader.setVec3("ttPlayerPaddlePos", game.GetPlayerPaddlePos());
            raytraceShader.setVec3("ttAiPaddlePos", game.GetAiPaddlePos());

            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            // Draw static environment and room
            // Passes ttActive, activeCarromBoard, and chessActive so active mini-games don't draw duplicate static pieces
            int activeCarromId = carromActive ? carromGame.GetActiveBoardId() : 0;
            scene.Draw(phongShader, lightShader, projection, view, camPos, ttActive, activeCarromId, chessActive);

            // Draw dynamic gameplay objects
            if (ttActive)
            {
                game.DrawDynamic(phongShader, scene.cylinder, scene.sphere);
            }
            else if (carromActive)
            {
                carromGame.DrawDynamic(phongShader, lightShader, scene.cylinder);
            }
            else if (chessActive)
            {
                chessGame.DrawDynamic(phongShader, lightShader, scene.cube, scene.cylinder, scene.sphere);
            }
        }

        // Draw HUDs (mini-games HUDs or exploration prompts)
        if (ttActive)
        {
            game.DrawHUD(hud, camera.Position);
        }
        else if (carromActive)
        {
            carromGame.DrawHUD(hud, scrWidth, scrHeight);
        }
        else if (chessActive)
        {
            chessGame.DrawHUD(hud, scrWidth, scrHeight);
        }
        else
        {
            // Exploration mode: Proximity prompt for closest available game setup
            glm::vec3 c1(-8.0f, 3.5f, 5.0f);
            glm::vec3 c2( 8.0f, 3.5f, 5.0f);
            glm::vec3 chessPos(8.0f, 2.05f, -5.0f);

            bool nearTT = game.IsNearTable(camera.Position);
            bool nearC1 = CarromGame::IsNearBoard(camera.Position, c1);
            bool nearC2 = CarromGame::IsNearBoard(camera.Position, c2);
            bool nearChess = ChessGame::IsNearTable(camera.Position);

            float distTT = glm::length(glm::vec2(camera.Position.x - 5.5f, camera.Position.z - 0.0f));
            float distC1 = glm::length(glm::vec2(camera.Position.x - c1.x, camera.Position.z - (c1.z + 1.8f)));
            float distC2 = glm::length(glm::vec2(camera.Position.x - c2.x, camera.Position.z - (c2.z + 1.8f)));
            float distChess = glm::length(glm::vec2(camera.Position.x - chessPos.x, camera.Position.z - (chessPos.z - 1.5f)));

            if (nearChess && (!nearTT || distChess <= distTT) && (!nearC1 || distChess <= distC1) && (!nearC2 || distChess <= distC2))
            {
                hud.BeginFrame();
                hud.RenderTextCentered("CHESS GAME", 200.0f, 3.0f, glm::vec3(1.0f, 0.95f, 0.7f));
                hud.RenderTextCentered("Press 1 to Play (White vs AI)", 250.0f, 2.0f, glm::vec3(0.8f, 0.9f, 1.0f));
                hud.RenderTextCentered("Move closer or away to interact", 290.0f, 1.5f, glm::vec3(0.7f, 0.75f, 0.85f));
                hud.EndFrame();
            }
            else if (nearC1 && (!nearC2 || distC1 <= distC2) && (!nearTT || distC1 <= distTT))
            {
                hud.BeginFrame();
                hud.RenderTextCentered("CARROM (Board 1)", 200.0f, 3.0f, glm::vec3(1.0f, 0.95f, 0.7f));
                hud.RenderTextCentered("Press 1 to Play", 250.0f, 2.0f, glm::vec3(0.8f, 0.9f, 1.0f));
                hud.RenderTextCentered("Move closer or away to interact", 290.0f, 1.5f, glm::vec3(0.7f, 0.75f, 0.85f));
                hud.EndFrame();
            }
            else if (nearC2 && (!nearTT || distC2 <= distTT))
            {
                hud.BeginFrame();
                hud.RenderTextCentered("CARROM (Board 2)", 200.0f, 3.0f, glm::vec3(1.0f, 0.95f, 0.7f));
                hud.RenderTextCentered("Press 1 to Play", 250.0f, 2.0f, glm::vec3(0.8f, 0.9f, 1.0f));
                hud.RenderTextCentered("Move closer or away to interact", 290.0f, 1.5f, glm::vec3(0.7f, 0.75f, 0.85f));
                hud.EndFrame();
            }
            else if (nearTT)
            {
                game.DrawHUD(hud, camera.Position);
            }
        }

        // Ray Tracing Mode Status Display
        if (rayTracingEnabled)
        {
            hud.BeginFrame();
            hud.RenderText("[OPTICAL RAY TRACING: ON] (Press 'T' to toggle)", 20.0f, 20.0f, 2.0f, glm::vec3(0.35f, 1.0f, 0.55f));
            hud.RenderText("Ray-Casted Shadows | Specular Reflections | Analytical Geometry", 20.0f, 44.0f, 1.5f, glm::vec3(0.85f, 0.92f, 1.0f));
            hud.EndFrame();
        }
        else if (!ttActive && !carromActive && !chessActive)
        {
            hud.BeginFrame();
            hud.RenderText("[Press 'T' for Optical Ray Tracing]", 20.0f, 20.0f, 1.5f, glm::vec3(0.70f, 0.85f, 1.0f));
            hud.EndFrame();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    // Global toggle for Real-time Optical Ray Tracing with 'T'
    bool keyTDown = (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS);
    if (keyTDown && !keyTPressedLast)
    {
        rayTracingEnabled = !rayTracingEnabled;
    }
    keyTPressedLast = keyTDown;

    bool ttActive = (game.GetState() != GameState::EXPLORATION);
    bool carromActive = carromGame.IsActive();
    bool chessActive = chessGame.IsActive();

    if (!ttActive && !carromActive && !chessActive)
    {
        // ================= ROOM EXPLORATION =================
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
            return;
        }

        // Key 1: enters mini-game if in proximity
        bool key1Down = (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS);
        if (key1Down && !key1PressedLast)
        {
            glm::vec3 c1(-8.0f, 3.5f, 5.0f);
            glm::vec3 c2( 8.0f, 3.5f, 5.0f);
            glm::vec3 chessPos(8.0f, 2.05f, -5.0f);

            bool nearTT = game.IsNearTable(camera.Position);
            bool nearC1 = CarromGame::IsNearBoard(camera.Position, c1);
            bool nearC2 = CarromGame::IsNearBoard(camera.Position, c2);
            bool nearChess = ChessGame::IsNearTable(camera.Position);

            float distTT = glm::length(glm::vec2(camera.Position.x - 5.5f, camera.Position.z - 0.0f));
            float distC1 = glm::length(glm::vec2(camera.Position.x - c1.x, camera.Position.z - (c1.z + 1.8f)));
            float distC2 = glm::length(glm::vec2(camera.Position.x - c2.x, camera.Position.z - (c2.z + 1.8f)));
            float distChess = glm::length(glm::vec2(camera.Position.x - chessPos.x, camera.Position.z - (chessPos.z - 1.5f)));

            if (nearChess && (!nearTT || distChess <= distTT) && (!nearC1 || distChess <= distC1) && (!nearC2 || distChess <= distC2))
            {
                // Enter Chess Game
                savedCamPos = camera.Position;
                savedYaw = camera.Yaw;
                savedPitch = camera.Pitch;
                chessGame.Start();
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else if (nearC1 && (!nearC2 || distC1 <= distC2) && (!nearTT || distC1 <= distTT))
            {
                // Enter Carrom Board 1
                savedCamPos = camera.Position;
                savedYaw = camera.Yaw;
                savedPitch = camera.Pitch;
                carromGame.Start(1, c1);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else if (nearC2 && (!nearTT || distC2 <= distTT))
            {
                // Enter Carrom Board 2
                savedCamPos = camera.Position;
                savedYaw = camera.Yaw;
                savedPitch = camera.Pitch;
                carromGame.Start(2, c2);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else if (nearTT)
            {
                // Enter Table Tennis
                savedCamPos = camera.Position;
                savedYaw = camera.Yaw;
                savedPitch = camera.Pitch;
                game.EnterGame();
            }
        }
        key1PressedLast = key1Down;

        // Reset gameplay key latches
        keyEscPressedLast = false;
        keyEPressedLast = false;
        keySpacePressedLast = false;
        keyRPressedLast = false;

        // Exploration WASD movement
        float cameraSpeed = static_cast<float>(8.0f * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.Position += cameraSpeed * camera.Front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.Position -= cameraSpeed * camera.Front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
    }
    else if (chessActive)
    {
        // ================= CHESS GAMEPLAY =================
        bool escDown = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
        bool eDown   = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);

        if ((escDown && !keyEscPressedLast) || (eDown && !keyEPressedLast))
        {
            chessGame.Stop();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera.Position = savedCamPos;
            camera.Yaw = savedYaw;
            camera.Pitch = savedPitch;
            camera.updateCameraVectors();
            firstMouse = true;
        }
        keyEscPressedLast = escDown;
        keyEPressedLast   = eDown;

        // R: Restart game
        bool rDown = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
        if (rDown && !keyRPressedLast)
        {
            chessGame.ResetGame();
        }
        keyRPressedLast = rDown;

        key1PressedLast = false;
    }
    else if (carromActive)
    {
        // ================= CARROM GAMEPLAY =================
        bool escDown = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
        bool eDown   = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);

        if ((escDown && !keyEscPressedLast) || (eDown && !keyEPressedLast))
        {
            carromGame.Stop();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera.Position = savedCamPos;
            camera.Yaw = savedYaw;
            camera.Pitch = savedPitch;
            camera.updateCameraVectors();
            firstMouse = true;
        }
        keyEscPressedLast = escDown;
        keyEPressedLast   = eDown;

        // Baseline striker sliding with A / D or Left / Right
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            carromGame.MoveStriker(-1.0f, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            carromGame.MoveStriker(1.0f, deltaTime);

        key1PressedLast = false;
    }
    else if (ttActive)
    {
        // ================= TABLE TENNIS GAMEPLAY =================
        bool escDown = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
        bool eDown   = (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS);

        if ((escDown && !keyEscPressedLast) || (eDown && !keyEPressedLast))
        {
            game.ExitGame();
            camera.Position = savedCamPos;
            camera.Yaw = savedYaw;
            camera.Pitch = savedPitch;
            camera.updateCameraVectors();
            firstMouse = true;
        }
        keyEscPressedLast = escDown;
        keyEPressedLast   = eDown;

        // SPACE: trigger player paddle swing
        bool spaceDown = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
        if (spaceDown && !keySpacePressedLast)
        {
            game.PlayerSwing();
        }
        keySpacePressedLast = spaceDown;

        key1PressedLast = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    scrWidth = width > 0 ? width : 1;
    scrHeight = height > 0 ? height : 1;
    hud.Resize(scrWidth, scrHeight);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    // Ignore mouse-look rotation during mini-games
    if (carromGame.IsActive() || chessGame.IsActive() || game.GetState() != GameState::EXPLORATION)
        return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.Yaw   += xoffset;
    camera.Pitch += yoffset;

    if (camera.Pitch >  89.0f) camera.Pitch =  89.0f;
    if (camera.Pitch < -89.0f) camera.Pitch = -89.0f;

    camera.updateCameraVectors();
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (carromGame.IsActive())
    {
        carromGame.HandleMouseButton(button, action);
    }
    else if (chessGame.IsActive())
    {
        chessGame.HandleMouseButton(button, action);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}
