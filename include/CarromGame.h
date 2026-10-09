#ifndef CARROM_GAME_H
#define CARROM_GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Primitives.h"
#include "Material.h"
#include "HudRenderer.h"
#include <vector>
#include <string>

// Disc classification
enum class DiscType {
    STRIKER,
    QUEEN,
    WHITE,
    BLACK
};

// Physics state for an individual disc
struct CarromDisc {
    glm::vec2 pos;           // World XZ position
    glm::vec2 vel;           // World XZ velocity
    float radius;            // Collision radius
    float mass;              // Effective physical mass
    DiscType type;
    bool isActive;           // In active play
    bool isPocketed;         // Marked as pocketed
    float sinkAnim;          // Sinking animation progress [0..1]
    glm::vec2 sinkPocketPos; // Center of pocket captured into
};

// High-level game states
enum class CarromState {
    INACTIVE,
    AIMING,
    SHOT_IN_PROGRESS,
    RESETTING
};

class CarromGame {
public:
    CarromGame();

    // Lifecycle
    void Start(int boardId, glm::vec3 boardCenter);
    void Stop();
    void Update(float dt);

    // Controls
    void HandleMouseMove(double xpos, double ypos, int scrW, int scrH, const glm::mat4& view, const glm::mat4& proj);
    void HandleMouseButton(int button, int action);
    void MoveStriker(float dir, float dt);

    // Queries
    bool IsActive() const { return state != CarromState::INACTIVE; }
    int GetActiveBoardId() const { return activeBoardId; }
    CarromState GetState() const { return state; }

    glm::mat4 GetGameplayViewMatrix() const;
    glm::vec3 GetGameplayCameraPos() const;

    // Proximity checks
    static bool IsNearBoard(const glm::vec3& camPos, const glm::vec3& boardCenter);

    // Dynamic rendering
    void DrawDynamic(Shader& phongShader, Shader& lightShader, Mesh& cylinder) const;
    void DrawHUD(HudRenderer& hud, int scrW, int scrH) const;

private:
    CarromState state = CarromState::INACTIVE;
    int activeBoardId = 0;
    glm::vec3 boardCenter = glm::vec3(0.0f);
    float surfaceY = 3.55f;

    // Inner playing boundaries
    float minX = 0.0f, maxX = 0.0f;
    float minZ = 0.0f, maxZ = 0.0f;

    // Four corner pockets (world XZ)
    std::vector<glm::vec2> pockets;

    // Discs: index 0 is always the striker, 1..N are the coins
    std::vector<CarromDisc> discs;

    // Baseline constraints
    float baselineZ = 0.0f;
    float strikerMinX = 0.0f, strikerMaxX = 0.0f;

    // Aiming
    glm::vec2 aimTarget = glm::vec2(0.0f);
    glm::vec2 aimDir = glm::vec2(0.0f, -1.0f);

    // Shot power charging
    bool isCharging = false;
    float chargeTime = 0.0f;
    static constexpr float MIN_POWER = 3.0f;
    static constexpr float MAX_POWER = 15.0f;
    static constexpr float CHARGE_DURATION = 1.2f;

    // Context feedback and reset timers
    std::string feedbackMsg;
    float feedbackTimer = 0.0f;
    float resetTimer = 0.0f;

    // Fixed timestep physics accumulator
    float physicsAccumulator = 0.0f;

    void resetBoard();
    void resetStriker();
    void physicsSubstep(float dt);
    void resolveCollisions();
    bool isBoardStationary() const;

    void drawMeshRaw(Shader& shader, Mesh& mesh, const glm::mat4& model, const Material& mat) const;
};

#endif
